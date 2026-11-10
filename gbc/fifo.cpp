#include "fifo.h"
#include "byte.h"
#include "emulator.h"
#include "ppu.h"
#include <stdexcept>

namespace cx::gbc
{

oam_entry::oam_entry(std::span<const u8, 4> data)
{
    y = data[0];
    x = data[1];
    tile = data[2];

    const auto flags{ data[3] };
    vram_bank = byte::get(flags, 3);
    palette = byte::get(flags, 4) ? 1 : 0;
    flip_x = byte::get(flags, 5);
    flip_y = byte::get(flags, 6);
    bg_prio = byte::get(flags, 7);
}

fifo::fifo(ppu* ppu) : m_ppu{ ppu }
{
    // At most 10 sprites a line
    m_sprites.reserve(16);
    m_sprite_data.reserve(32);
}

auto fifo::adjust_tile() -> void
{
    m_current_tile += 128;
}

auto fifo::get_tile() -> void
{
    const auto tile_map{ byte::get(m_ppu->m_lcdc, 3) ? 0x9C00 : 0x9800 };

    const auto addr{ static_cast<u16>(tile_map + (m_map_x / 8) + ((m_map_y / 8) * 32)) };

    m_current_tile = m_ppu->m_system->m_bus.read(addr);

    if (!byte::get(m_ppu->m_lcdc, 4))
        adjust_tile();
}

auto fifo::get_window() -> void
{
    const int wy{ m_ppu->m_wy };
    const int wx{ m_ppu->m_wx };

    if (m_fetch_x + 7 >= wx && m_fetch_x + 7 < wx + static_cast<int>(ppu::RENDER_WIDTH) + 14 && m_ppu->m_ly >= wy &&
        m_ppu->m_ly < wy + static_cast<int>(ppu::RENDER_HEIGHT))
    {
        const auto ty{ m_ppu->m_window_line / 8 };

        const auto win_map{ byte::get(m_ppu->m_lcdc, 6) ? 0x9C00 : 0x9800 };
        const auto addr{ static_cast<u16>((win_map + (m_fetch_x + 7 - wx) / 8) + ty * 32) };
        m_current_tile = m_ppu->m_system->m_bus.read(addr);

        if (!byte::get(m_ppu->m_lcdc, 4))
            adjust_tile();

        m_in_window = true;
    }
}

auto fifo::get_sprites() -> void
{
    for (const auto& sprite : m_ppu->m_sprites_on_line)
    {
        const auto sx{ (sprite.x - 8) + (m_ppu->m_scx % 8) };

        if ((sx >= m_fetch_x && sx < m_fetch_x + 8) || ((sx + 8) >= m_fetch_x && (sx + 8) < m_fetch_x + 8))
        {
            m_sprites.push_back(sprite);
        }
    }
}

auto fifo::get_sprite_data() -> void
{
    auto& bus{ m_ppu->m_system->m_bus };

    const auto cy{ m_ppu->m_ly };
    const u8 height{ static_cast<u8>(byte::get(m_ppu->m_lcdc, 2) ? 16 : 8) };

    for (const auto& sprite : m_sprites)
    {
        auto ty{ (cy + 16 - sprite.y) * 2 };

        if (sprite.flip_y)
            ty = (height * 2) - 2 - ty;

        auto tile{ sprite.tile };
        if (height == 16)
            tile &= 0xFE; // remove last bit

        auto addr{ static_cast<u16>(0x8000 + (tile * 16) + ty) };
        m_sprite_data.push_back(bus.read(addr));
        m_sprite_data.push_back(bus.read(++addr));
    }
}

auto fifo::get_tile_low() -> void
{
    const auto map{ byte::get(m_ppu->m_lcdc, 4) ? 0x8000 : 0x8800 };
    const auto addr{ static_cast<u16>(map + (m_current_tile * 16) + m_tile_y) };
    m_tile_lo = m_ppu->m_system->m_bus.read(addr);
}

auto fifo::get_tile_high() -> void
{
    const auto map{ byte::get(m_ppu->m_lcdc, 4) ? 0x8000 : 0x8800 };
    const auto addr{ static_cast<u16>(map + (m_current_tile * 16) + m_tile_y + 1) };
    m_tile_hi = m_ppu->m_system->m_bus.read(addr);
}

// One pixel per clock
// Pauses unless it has > 8 pixels
auto fifo::push() -> void
{
    if (m_fifo.size() <= 8)
        return;

    const auto p{ m_fifo.dequeue() };

    if (m_line_x > (m_ppu->m_scx % 8))
    {
        if (m_ppu->m_ly != 0x90)
        {
            const auto comb{ m_pushed_x + (m_ppu->m_ly * static_cast<int>(ppu::RENDER_WIDTH)) };

            // LY is writable by the CPU, so this can run off the end of the bitmap (C# throws)
            m_ppu->m_bgr24_bitmap.at(comb * 3 + 0) = p.b;
            m_ppu->m_bgr24_bitmap.at(comb * 3 + 1) = p.g;
            m_ppu->m_bgr24_bitmap.at(comb * 3 + 2) = p.r;
        }

        m_pushed_x++;
    }

    m_line_x++;
}

auto fifo::add() -> bool
{
    // Full
    if (m_fifo.size() > 8)
        return false;

    const auto x{ m_in_window ? m_fetch_x - (8 - (m_ppu->m_scx % 8)) : static_cast<int>(m_fetch_x) };

    for (auto i = 0; i < 8; i++)
    {
        const auto bit{ 7 - i };

        const u8 hi{ static_cast<u8>(byte::get(m_tile_hi, bit) ? 1 : 0) };
        const u8 lo{ static_cast<u8>(byte::get(m_tile_lo, bit) ? 1 : 0) };
        u8 shade{ m_ppu->m_dmg_palette[hi << 1 | lo] };

        if (!byte::get(m_ppu->m_lcdc, 0))
            shade = 0;

        if (byte::get(m_ppu->m_lcdc, 1))
        {
            const auto [sprite_set, sprite_c]{ fetch_sprite_pixel(shade) };
            if (sprite_set)
                shade = sprite_c;
        }

        const auto pixel_colour{ m_ppu->rgb_lookup(shade) };
        if (x >= 0)
        {
            m_fifo.enqueue(pixel_colour);
            m_fifo_x++;
        }
    }

    return true;
}

auto fifo::fetch_sprite_pixel(u8 bg_colour) -> std::pair<bool, u8>
{
    for (std::size_t i = 0; i < m_sprites.size(); i++)
    {
        const auto& sprite{ m_sprites[i] };
        const auto x{ sprite.x - 8 + (m_ppu->m_scx % 8) };

        if (x + 8 < m_fifo_x)
            continue;

        const auto offset{ m_fifo_x - x };

        if (offset < 0 || offset > 7)
            continue;

        const auto sbit{ sprite.flip_x ? offset : (7 - offset) };

        const u8 lo{ static_cast<u8>(byte::get(m_sprite_data[i * 2], sbit) ? 1 : 0) };
        const u8 hi{ static_cast<u8>(byte::get(m_sprite_data[(i * 2) + 1], sbit) ? 1 : 0) };
        const auto idx{ hi << 1 | lo };

        // transparent
        if (hi == 0 && lo == 0)
            continue;

        if (!sprite.bg_prio || bg_colour == 0)
        {
            return { true, m_ppu->m_palette_oam[sprite.palette][idx] };
        }
    }

    return { false, 0 };
}

// 3 clocks to fetch 8 pixels
// pauses in 4th clock unless space in data
auto fifo::fetch() -> void
{
    switch (m_mode)
    {
    case mode::tile:

        m_sprites.clear();
        m_sprite_data.clear();
        m_in_window = false;

        if (byte::get(m_ppu->m_lcdc, 0))
            get_tile();

        if (byte::get(m_ppu->m_lcdc, 1) && !m_ppu->m_sprites_on_line.empty())
            get_sprites();

        if (m_ppu->show_window())
            get_window();

        m_mode = mode::data_low;
        m_fetch_x += 8;
        break;

    case mode::data_low:
        get_tile_low();
        get_sprite_data(); // should just get low sprite data, YOLO
        m_mode = mode::data_high;
        break;

    case mode::data_high:
        get_tile_high();
        m_mode = mode::sleep;
        break;

    case mode::sleep:
        m_mode = mode::push;
        break;

    case mode::push:
        if (add())
            m_mode = mode::tile;
        break;

    default:
        throw std::runtime_error{ "Invalid operation" };
    }
}

auto fifo::reset() -> void
{
    m_fifo.clear();
    m_map_x = 0;
    m_map_y = 0;
    m_fetch_x = 0;
    m_tile_y = 0;
    m_line_x = 0;
    m_fifo_x = 0;
    m_pushed_x = 0;
    m_mode = mode::tile;
}

auto fifo::clock() -> void
{
    m_map_x = static_cast<u8>(m_fetch_x + m_ppu->m_scx);
    m_map_y = static_cast<u8>(m_ppu->m_ly + m_ppu->m_scy);
    m_tile_y = static_cast<u8>((m_map_y % 8) * 2);

    if ((m_ppu->m_line_dot % 2) == 0)
    {
        fetch();
    }

    push();
}

} // namespace cx::gbc
