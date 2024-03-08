#include "ppu.h"
#include "cpu.h"
#include "emulator.h"

namespace cx::nes
{

ppu::ppu(emulator* system) : m_system{ system }, m_bus{ system }
{
}

void ppu::clock()
{
    m_scan_x = m_clock % CLOCKS_PER_LINE;
    m_scan_line = m_clock / CLOCKS_PER_LINE;

    if (m_scan_x == 320 && m_scan_line >= 0 && m_scan_line < RENDER_HEIGHT)
        setup_sprites_on_next_line();

    if (m_scan_x == 260 && m_scan_line < 240)
        m_system->m_mapper->scan_line();

    m_scroll_x = m_next_scroll_x;
    m_scroll_y = m_next_scroll_y;

    auto solid_bg{ false };
    if (m_show_background)
        solid_bg = render_pixel_background();

    if (m_show_sprites)
        render_pixel_sprite(solid_bg);

    m_clock++;

    if (m_scan_line == VBLANK_LINE && m_scan_x == 1)
    {
        m_vblank = true;
        m_frame++;
        on_frame_completed();

        if (m_nmi_on_vblank)
        {
            m_system->m_cpu.set_nmi();
        }
    }

    if (m_scan_line == VBLANK_LINE && m_scan_x == 3)
        m_dont_vblank = false;

    if (m_scan_line == 261 && m_scan_x == 1)
    {
        m_vblank = false;
        m_sprite_zero_hit = false;
    }

    if (m_scan_x == 339 && m_scan_line == 261 && m_rendering_enabled && !m_odd_even_flag)
        m_clock = 0;

    if (m_scan_x == 340 && m_scan_line == 261)
        m_clock = 0;

    m_odd_even_flag = !m_odd_even_flag;
}

auto ppu::read(u16 addr) -> u8
{
    return u8();
}

void ppu::write(u16 addr, u8 value)
{
}

void ppu::update_mirroring(mirroring m)
{
    m_bus.update_mirroring(m);
}

void ppu::set_frame_complete_callback(frame_event e)
{
    m_frame_completed = e;
}

void ppu::on_frame_completed()
{
    if (m_frame_completed)
        m_frame_completed(m_bgr24bmp);
}

} // namespace cx::nes