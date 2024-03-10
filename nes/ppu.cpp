#include "ppu.h"
#include "cpu.h"
#include "emulator.h"
#include <print>

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
    if (m_mask.flags.show_background)
        solid_bg = render_pixel_background();

    if (m_mask.flags.show_sprites)
        render_pixel_sprite(solid_bg);

    m_clock++;

    if (m_scan_line == VBLANK_LINE && m_scan_x == 1)
    {
        m_vblank = true;
        m_frame++;
        on_frame_completed();

        if (m_nmi_on_vblank)
            m_system->m_cpu.set_nmi();
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
    if (addr == 0x2002)
        return get_status_byte();
    if (addr == 0x2007)
        return read_addr_value();

    std::println("PPU Read of unknown address: {}", addr);
    return 0;
}

void ppu::write(u16 addr, u8 value)
{
    auto instr{ addr & 0x000Fu };

    switch (instr)
    {
    case 0x0:
        write_ctrl(value);
        break;
    case 0x1:
        write_mask(value);
        break;
    case 0x2: // status, read only.
        break;
    case 0x3:
        write_oam_addr(value);
        break;
    case 0x4:
        write_oam(value);
        break;
    case 0x5:
        write_scroll(value);
        break;
    case 0x6:
        write_addr(value);
        break;
    case 0x7:
        write_addr_value(value);
        break;
    default:
        break;
    }
}

void ppu::oam_dma(u8 value)
{
    // TODO: CPU cycles

    u16 from{ static_cast<u16>(value << 8) };
    for (u16 i = 0; i <= 0xFF; i++)
    {
        // FIXME: Remove?
        // m_oam_addr %= sizeof(m_oam);

        m_oam[m_oam_addr] = m_system->m_cpu.m_bus.read(from);
        m_oam_addr++;
        from++;
    }
}

void ppu::write_ctrl(u8 value)
{
    switch (value & 0x3)
    {
    default:
    case 0x0:
        m_base_table_addr = 0x2000;
        break;
    case 0x1:
        m_base_table_addr = 0x2400;
        break;
    case 0x2:
        m_base_table_addr = 0x2800;
        break;
    case 0x3:
        m_base_table_addr = 0x2C00;
        break;
    }

    m_vram_table_addr_inc = (value & 0x04) == 0x04 ? 0x0020 : 0x0001;
    m_sprite_table_addr = (value & 0x08) == 0x08 ? 0x1000 : 0x0000;
    m_back_table_addr = (value & 0x10) == 0x10 ? 0x1000 : 0x0000;
    m_current_sprite_size = (value & 0x20) == 0x20 ? sprite_size::s8x16 : sprite_size::s8x8;
    m_write_ext_pins = (value & 0x40) == 0x40;
    m_nmi_on_vblank = (value & 0x80) == 0x80;
}

void ppu::write_mask(u8 value)
{
    m_mask.bits = value;
}

void ppu::write_oam_addr(u8 value)
{
    m_oam_addr = value;
}

void ppu::write_oam(u8 value)
{
    // FIXME: Remove?
    // m_oam_addr %= sizeof(m_oam);

    m_oam[m_oam_addr] = value;
    m_oam_addr++;
}

void ppu::write_scroll(u8 value)
{
    if (m_scrolling_x)
        m_next_scroll_x = value;
    else
        m_next_scroll_y = value;

    m_scrolling_x = !m_scrolling_x;
}

void ppu::write_addr(u8 value)
{
    if (m_write_addr_upper)
        m_addr = static_cast<u16>(value << 8);
    else
        m_addr |= value;

    m_write_addr_upper = !m_write_addr_upper;

    // Reset scroll as on hardware these share a register
    m_next_scroll_x = 0;
    m_next_scroll_y = 0;
    m_scrolling_x = true;
}

auto ppu::get_status_byte() -> u8
{
    u8 status{ 0 };

    if (m_vblank)
        status |= 0x80;

    if (m_sprite_zero_hit)
        status |= 0x40;

    m_vblank = false;
    m_write_addr_upper = true;

    if (m_scan_line == 241 && (m_scan_x >= 1 || m_scan_x <= 3))
        m_dont_vblank = true;

    return status;
}

auto ppu::read_addr_value() -> u8
{
    u8 ret{ 0 };

    if (m_addr < 0x3EFF)
        ret = buffered_read(m_bus.read(m_addr));
    else
        ret = m_bus.read(m_addr);

    m_addr += m_vram_table_addr_inc;
    return ret;
}

auto ppu::buffered_read(u8 new_value) -> u8
{
    u8 ret{ m_read_buffer };
    m_read_buffer = new_value;
    return ret;
}

void ppu::write_addr_value(u8 value)
{
    m_bus.write(m_addr, value);
    m_addr += m_vram_table_addr_inc;
}

void ppu::update_mirroring(mirroring m)
{
    m_bus.update_mirroring(m);
}

void ppu::set_frame_complete_callback(frame_event e)
{
    m_frame_completed = e;
}

void ppu::setup_sprites_on_next_line()
{
    const u64 line{ m_scan_line + 1 };
    m_current_sprites.clear();

    const u32 height{ (m_current_sprite_size == sprite_size::s8x16 ? 16u : 8u) };

    for (size_t i = 0; i < sizeof(m_oam); i += 4)
    {
        const u8 y{ static_cast<u8>(m_oam[i] + 1) };

        if (y > 0xEF)
            continue;

        if (line >= y && line < y + height)
        {
            m_current_sprites.push_back({ /* x = */ m_oam[i + 3],
                                          /* y = */ y,
                                          /* attr = */ m_oam[i + 2],
                                          /* idx = */ m_oam[i + 1], i == 0 });
        }

        if (m_current_sprites.size() == 8)
            return;
    }
}

auto ppu::render_pixel_background() -> bool
{
    if (m_scan_x >= RENDER_WIDTH || m_scan_line >= RENDER_HEIGHT)
        return false;

    auto ox{ m_scan_x + m_scroll_x };
    auto oy{ m_scan_line + m_scroll_y };
    auto bta{ m_base_table_addr };

    // TODO: Add vertical mirroring support
    if (ox >= RENDER_WIDTH)
    {
        bta ^= 0x400;
        ox -= RENDER_WIDTH;
    }
    if (oy >= RENDER_HEIGHT)
    {
        oy -= RENDER_HEIGHT;
    }

    auto tile_x{ ox / 8 };
    auto tile_y{ oy / 8 };
    auto ntl{ static_cast<u16>(bta + tile_y * 32 + tile_x) };
    auto name_table_entry{ m_bus.read(ntl) };

    auto attr_x{ tile_x / 4 };
    auto attr_y{ tile_y / 4 };
    auto attr{ m_bus.read(static_cast<u16>(bta + ATTR_TABLE_ADDR + attr_y * 8 + attr_x)) };

    auto quad_x{ (tile_x % 4) / 2 };
    auto quad_y{ (tile_y % 4) / 2 };

    auto colour_bits{ (attr >> ((quad_x * 2) + (quad_y * 4))) & 0x3 };
    auto entry{ static_cast<u16>(m_back_table_addr + name_table_entry * 0x10 + (oy % 8)) };

    auto first_plane_byte{ m_bus.read(entry) };
    auto second_plane_byte{ m_bus.read(entry + 8) };

    auto first_plane_bit{ (first_plane_byte >> (7 - (ox % 8))) & 1 };
    auto second_plane_bit{ (second_plane_byte >> (7 - (ox % 8))) & 1 };

    colour pixel{ 0, 0, 0 };
    bool solid{ false };
    if (first_plane_bit == 0 && second_plane_bit == 0)
        pixel = lookup_bg_palette(0);
    else
    {
        auto pi{ first_plane_bit + (second_plane_bit * 2) + (colour_bits * 4) };
        pixel = lookup_bg_palette(pi);
        solid = true;
    }

    m_bgr24bmp[(m_scan_x + m_scan_line * RENDER_WIDTH) * 3 + 0] = pixel.b;
    m_bgr24bmp[(m_scan_x + m_scan_line * RENDER_WIDTH) * 3 + 1] = pixel.g;
    m_bgr24bmp[(m_scan_x + m_scan_line * RENDER_WIDTH) * 3 + 2] = pixel.r;

    return solid;
}

bool ppu::should_ignore_sprite(const sprite& sprite)
{
    return sprite.y == 0 || sprite.y > RENDER_HEIGHT - 1 || m_scan_x < sprite.x || m_scan_x >= sprite.x + 8 ||
           m_scan_x < 8 && !m_mask.flags.show_sprites_leftmost;
}

void ppu::render_pixel_sprite(bool render_bg)
{
    if (m_scan_x >= RENDER_WIDTH || m_scan_line >= RENDER_HEIGHT)
        return;

    const bool tall_sprites{ m_current_sprite_size == sprite_size::s8x16 };

    for (const auto& sprite : m_current_sprites)
    {
        if (should_ignore_sprite(sprite))
            continue;

        u16 tile{ sprite.idx };
        if (tall_sprites)
            tile = tile & ~0x1;
        tile *= 16;

        auto palette{ sprite.attr & 0x3 };
        auto front{ (sprite.attr & 0x20) == 0 };
        auto flip_x{ (sprite.attr & 0x40) == 0x40 };
        auto flip_y{ (sprite.attr & 0x80) == 0x80 };

        u8 pixel_x{ static_cast<u8>(m_scan_x - sprite.x) };
        u8 pixel_y{ static_cast<u8>(m_scan_line - sprite.y) };

        u16 table{ (tall_sprites ? static_cast<u16>((sprite.idx & 1) * 1000) : m_sprite_table_addr) };

        if (tall_sprites)
        {
            if (pixel_y >= 8)
            {
                pixel_y -= 8;
                if (!flip_y)
                    tile += 16;
                flip_y = false;
            }
            if (flip_y)
                tile += 16;
        }

        pixel_x = flip_x ? 7 - pixel_x : pixel_x;
        pixel_y = flip_y ? 7 - pixel_y : pixel_y;

        u16 address{ static_cast<u16>(table + tile + pixel_y) };

        u8 upper_bit{ static_cast<u8>((m_bus.read(address + 8) & (0x80u >> pixel_x)) >> (7 - pixel_x)) };
        u8 lower_bit{ static_cast<u8>((m_bus.read(address) & (0x80u >> pixel_x)) >> (7 - pixel_x)) };
        u8 colour{ static_cast<u8>((upper_bit << 1) | lower_bit) };

        if (colour > 0)
        {
            if (sprite.is_sprite_zero && (m_scan_x > 8 || m_mask.flags.show_sprites_leftmost) && (!render_bg) &&
                (!m_sprite_zero_hit) && x != 255)
            {
                m_sprite_zero_hit = true;
            }

            if (m_mask.flags.show_background && (front || !render_bg))
            {
                u8 palette_index{ static_cast<u8>(palette * 4 + colour) };
                auto pixel_colour{ lookup_sprite_palette(palette_index) };

                m_bgr24bmp[(m_scan_x + m_scan_line * RENDER_WIDTH) * 3 + 0] = pixel_colour.b;
                m_bgr24bmp[(m_scan_x + m_scan_line * RENDER_WIDTH) * 3 + 1] = pixel_colour.g;
                m_bgr24bmp[(m_scan_x + m_scan_line * RENDER_WIDTH) * 3 + 2] = pixel_colour.r;

                return;
            }
        }
    }
}

auto ppu::lookup_bg_palette(u8 index) -> colour
{
    return lookup_colour(m_bus.read(0x3F00 + index));
}

auto ppu::lookup_sprite_palette(u8 index) -> colour
{
    u16 addr = 0x3F00;

    if (index == 0x0 || index == 0x4 || index == 0x8 || index == 0xC)
        addr += index;
    else
        addr = 0x3F10 + index;

    return lookup_colour(m_bus.read(addr));
}

void ppu::on_frame_completed()
{
    if (m_frame_completed)
        m_frame_completed(m_bgr24bmp, sizeof(m_bgr24bmp));
}

} // namespace cx::nes