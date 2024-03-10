#pragma once

#include "cart.h"
#include "core/types.h"
#include "forward.h"
#include "mapper.h"
#include "ppu_bus.h"
#include <vector>

namespace cx::nes
{

class ppu
{
  public:
    ppu(emulator*);

    void clock();

    auto read(u16 addr) -> u8;
    void write(u16 addr, u8 value);
    void oam_dma(u8 value);

    void update_mirroring(mirroring);

    ppu_bus m_bus;

    // Constants
    static constexpr u32 CLOCKS_PER_LINE{ 341 };
    static constexpr u32 LINES_PER_FRAME{ 262 };
    static constexpr u32 VBLANK_LINE{ 241 };
    static constexpr u32 ATTR_TABLE_ADDR{ 0x3C0 };
    static constexpr u32 RENDER_WIDTH{ 256 };
    static constexpr u32 RENDER_HEIGHT{ 240 };

    using frame_event = std::function<void(u8[], size_t)>;
    void set_frame_complete_callback(frame_event);

    u64 m_scan_x{ 21 };
    u64 m_scan_line{ 0 };

  private:
    emulator* m_system;

    void setup_sprites_on_next_line();
    auto render_pixel_background() -> bool;
    void render_pixel_sprite(bool render_bg);

    struct colour
    {
        u8 r;
        u8 g;
        u8 b;
    };
    auto lookup_bg_palette(u8 index) -> colour;
    auto lookup_sprite_palette(u8 index) -> colour;
    auto lookup_colour(u8 c) -> colour;

    enum class sprite_size
    {
        s8x8,
        s8x16
    };

    struct sprite
    {
        u8 x;
        u8 y;
        u8 attr;
        u8 idx;
        bool is_sprite_zero;
    };
    auto should_ignore_sprite(const sprite& sprite) -> bool;

    struct mask_struct
    {
        bool greyscale : 1;
        bool show_background_leftmost : 1;
        bool show_sprites_leftmost : 1;
        bool show_background : 1;
        bool show_sprites : 1;
        bool emphasize_red : 1;
        bool emphasize_green : 1;
        bool emphasize_blue : 1;
    };

    union mask_union {
        mask_struct flags;
        u8 bits;
    };

    mask_union m_mask{ 0 };

    void write_ctrl(u8 value);
    void write_mask(u8 value);
    void write_oam_addr(u8 value);
    void write_oam(u8 value);
    void write_scroll(u8 value);
    void write_addr(u8 value);
    void write_addr_value(u8 value);

    auto get_status_byte() -> u8;
    auto read_addr_value() -> u8;

    u8 m_read_buffer{ 0 };
    auto buffered_read(u8 new_value) -> u8;

    // Registers
    u8 v; // Current VRAM Address
    u8 t; // Temporary VRAM Address (top left onscreen tile)
    u8 x; // Fine scroll (3 bit)
    u8 w; // First or second write toggle
    bool m_write_addr_upper{ true };
    u16 m_addr{ 0 };

    // Frame info
    u64 m_frame{ 0 };
    u64 m_clock{ 22 };
    bool m_odd_even_flag{ false };
    bool m_vblank{ false };
    bool m_dont_vblank{ false };
    bool m_rendering_enabled{ false };
    bool m_sprite_zero_hit{ false };

    // Pixel data

    bool m_scrolling_x{ true };
    u8 m_scroll_x{ 0 };
    u8 m_next_scroll_x{ 0 };
    u8 m_scroll_y{ 0 };
    u8 m_next_scroll_y{ 0 };

    // Sprite data
    std::vector<sprite> m_current_sprites{};

    // Control Info
    u8 m_oam_addr{ 0x0 };
    u16 m_base_table_addr{ 0x2000 };
    u16 m_vram_table_addr_inc{ 0x1 };
    u16 m_sprite_table_addr{ 0x0 };
    u16 m_back_table_addr{ 0x0 };
    sprite_size m_current_sprite_size{ sprite_size::s8x8 };
    bool m_write_ext_pins{ false };
    bool m_nmi_on_vblank{ false };

    // Data
    u8 m_bgr24bmp[RENDER_WIDTH * RENDER_HEIGHT * 3];
    u8 m_oam[0x100];

    // Events
    frame_event m_frame_completed;
    void on_frame_completed();
};

} // namespace cx::nes