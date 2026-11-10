#include "ppu.h"
#include "byte.h"
#include "emulator.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <stdexcept>
#include <thread>

namespace cx::gbc
{

ppu::ppu(emulator* system) : m_system{ system }, m_fifo{ this }
{
    // At most 10 sprites a line
    m_sprites_on_line.reserve(16);
}

auto ppu::set_frame_complete_callback(frame_event callback) -> void
{
    m_frame_callback = std::move(callback);
}

auto ppu::on_frame_completed() -> void
{
    if (m_frame_callback)
        m_frame_callback(m_bgr24_bitmap.data(), m_bgr24_bitmap.size());
}

//
// IO
//
auto ppu::get_lcd_status() const -> u8
{
    u8 ret{ 0 };
    switch (m_mode)
    {
    case mode::horizontal_blank:
        ret = 0;
        break;
    case mode::vertical_blank:
        ret = 1;
        break;
    case mode::oam_scan:
        ret = 2;
        break;
    case mode::drawing:
        ret = 3;
        break;

    default:
        throw std::runtime_error{ "Invalid operation" };
    }

    if (m_ly == m_lyc)
        ret |= 0x04;
    if (m_hblank_interrupt)
        ret |= 0x08;
    if (m_vblank_interrupt)
        ret |= 0x10;
    if (m_oam_interrupt)
        ret |= 0x20;
    if (m_ly_compare_interrupt)
        ret |= 0x40;

    // Always set
    ret |= 0x80;

    return ret;
}

auto ppu::set_lcd_status(u8 value) -> void
{
    m_hblank_interrupt = (value & 0x08) == 0x08;
    m_vblank_interrupt = (value & 0x10) == 0x10;
    m_oam_interrupt = (value & 0x20) == 0x20;
    m_ly_compare_interrupt = (value & 0x40) == 0x40;
}

auto ppu::set_bg_palette(u8 value) -> void
{
    m_dmg_palette[3] = static_cast<u8>((value & 0xC0) >> 6);
    m_dmg_palette[2] = static_cast<u8>((value & 0x30) >> 4);
    m_dmg_palette[1] = static_cast<u8>((value & 0x0C) >> 2);
    m_dmg_palette[0] = static_cast<u8>(value & 0x03);
}

auto ppu::set_oam_palette(int palette, u8 value) -> void
{
    m_palette_oam[palette][3] = static_cast<u8>((value & 0xC0) >> 6);
    m_palette_oam[palette][2] = static_cast<u8>((value & 0x30) >> 4);
    m_palette_oam[palette][1] = static_cast<u8>((value & 0x0C) >> 2);
    m_palette_oam[palette][0] = static_cast<u8>(value & 0x03);
}

auto ppu::set_oam_palette_1(u8 value) -> void
{
    set_oam_palette(0, value);
}

auto ppu::set_oam_palette_2(u8 value) -> void
{
    set_oam_palette(1, value);
}

auto ppu::show_window() const -> bool
{
    return byte::get(m_lcdc, 5) && m_wx <= RENDER_WIDTH && m_wy <= RENDER_HEIGHT;
}

//
// State machine
//
auto ppu::change_mode(mode m) -> void
{
    m_mode = m;

    auto& processor{ m_system->m_cpu };

    // VBlank interrupt
    if (m == mode::vertical_blank)
    {
        processor.request_interrupt(cpu::interrupt::vblank);
    }

    // STAT interrupts
    if (m == mode::oam_scan && m_oam_interrupt)
    {
        processor.request_interrupt(cpu::interrupt::stat);
    }
    if (m == mode::vertical_blank && m_vblank_interrupt)
    {
        processor.request_interrupt(cpu::interrupt::stat);
    }
    if (m == mode::horizontal_blank && m_hblank_interrupt)
    {
        processor.request_interrupt(cpu::interrupt::stat);
    }
}

auto ppu::next_line() -> void
{
    if (show_window() && m_ly >= m_wy)
        m_window_line++;

    m_ly++;

    if (m_ly == m_lyc && m_ly_compare_interrupt)
    {
        m_system->m_cpu.request_interrupt(cpu::interrupt::stat);
        spdlog::debug("LYC interrupt on line {}", m_ly);
    }

    m_line_dot = 0;
}

auto ppu::hblank_clock() -> void
{
    if (m_line_dot >= static_cast<int>(DOTS_PER_LINE))
    {
        next_line();

        if (m_ly >= RENDER_HEIGHT)
            change_mode(mode::vertical_blank);
        else
            change_mode(mode::oam_scan);
    }
}

auto ppu::vblank_clock() -> void
{
    if (m_line_dot >= static_cast<int>(DOTS_PER_LINE))
    {
        next_line();

        if (m_ly >= LINES_PER_FRAME)
        {
            if (m_system->m_limit_frames)
            {
                const auto elapsed{ std::chrono::duration_cast<std::chrono::milliseconds>(
                                        std::chrono::steady_clock::now() - m_frame_timer)
                                        .count() };
                if (elapsed < 13)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(13 - elapsed));
                }
            }

            on_frame_completed();

            if (m_dot != static_cast<int>(ONE_FRAME_IN_DOTS))
                spdlog::debug("Invalid dots in frame: {}.", m_dot);

            change_mode(mode::oam_scan);
            m_ly = 0;
            m_dot = 0;
            m_line_dot = 0;
            m_window_line = 0;
            m_fifo.reset();
            m_frame_timer = std::chrono::steady_clock::now();
        }
    }
}

auto ppu::oam_scan_clock() -> void
{
    if (m_line_dot == 1)
    {
        m_sprites_on_line.clear();
        get_sprites();
    }

    if (m_line_dot >= 80)
    {
        change_mode(mode::drawing);
        m_fifo.reset();
    }
}

auto ppu::get_sprites() -> void
{
    const auto y{ m_ly };
    const u8 height{ static_cast<u8>(byte::get(m_lcdc, 2) ? 16 : 8) };
    const auto& oam{ m_system->m_bus.m_oam };

    for (auto i = 0; i < 0x28; i++)
    {
        const oam_entry sprite{ std::span<const u8, 4>{ oam.data() + i * 4, 4 } };

        if (sprite.x == 0)
            continue;

        if (sprite.y <= y + 16 && sprite.y + height > y + 16)
        {
            m_sprites_on_line.push_back(sprite);

            if (m_sprites_on_line.size() > 9)
                break;
        }
    }

    // OrderBy is stable, so sprites with the same X keep their OAM order
    std::stable_sort(m_sprites_on_line.begin(), m_sprites_on_line.end(),
                     [](const oam_entry& a, const oam_entry& b) { return a.x < b.x; });
}

auto ppu::draw_clock() -> void
{
    m_fifo.clock();

    if (m_fifo.m_pushed_x >= RENDER_WIDTH)
    {
        m_fifo.reset();
        change_mode(mode::horizontal_blank);
    }
}

auto ppu::clock() -> void
{
    m_line_dot++;
    m_dot++;

    switch (m_mode)
    {
    case mode::horizontal_blank:
        hblank_clock();
        break;
    case mode::vertical_blank:
        vblank_clock();
        break;
    case mode::oam_scan:
        oam_scan_clock();
        break;
    case mode::drawing:
        draw_clock();
        break;
    default:
        throw std::runtime_error{ "Invalid operation" };
    }
}

} // namespace cx::gbc
