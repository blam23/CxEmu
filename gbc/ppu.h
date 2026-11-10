#pragma once

#include "core/types.h"
#include "fifo.h"
#include "forward.h"
#include "reflection.h"
#include <array>
#include <chrono>
#include <functional>
#include <vector>

namespace cx::gbc
{

class ppu
{
  public:
    ppu(emulator* system);

    auto clock() -> void;

    auto vram_accessible() const -> bool
    {
        return !m_vram_blocked;
    }

    // Constants
    static constexpr u32 ONE_FRAME_IN_DOTS{ 70224 };
    static constexpr u32 LINES_PER_FRAME{ 154 };
    static constexpr u32 DOTS_PER_LINE{ 456 };
    static constexpr u32 RENDER_WIDTH{ 160 };
    static constexpr u32 RENDER_HEIGHT{ 144 };

    using frame_event = std::function<void(u8*, size_t)>;
    auto set_frame_complete_callback(frame_event) -> void;

    //
    // IO
    //
    // https://gbdev.io/pandocs/LCDC.html#ff40--lcdc-lcd-control
    // Bit  Name                          Usage notes
    // 7    LCD and PPU enable            0=Off, 1=On
    // 6    Window tile map area          0=9800-9BFF, 1=9C00-9FFF
    // 5    Window enable                 0=Off, 1=On
    // 4    BG and Window tile data area  0=8800-97FF, 1=8000-8FFF
    // 3    BG tile map area              0=9800-9BFF, 1=9C00-9FFF
    // 2    OBJ size                      0=8x8, 1=8x16
    // 1    OBJ enable                    0=Off, 1=On
    // 0    BG and Window enable/priority 0=Off, 1=On
    [[= io{ 0xFF40 }]] u8 m_lcdc{ 0x91 };

    // https://gbdev.io/pandocs/Scrolling.html
    [[= io{ 0xFF42 }]] u8 m_scy{ 0 };
    [[= io{ 0xFF43 }]] u8 m_scx{ 0 };
    [[= io{ 0xFF44 }]] u8 m_ly{ 0 };
    [[= io{ 0xFF45 }]] u8 m_lyc{ 0 };
    [[= io{ 0xFF4A }]] u8 m_wy{ 0 };
    [[= io{ 0xFF4B }]] u8 m_wx{ 7 };

    [[= io_read{ 0xFF41 }]] auto get_lcd_status() const -> u8;
    [[= io_write{ 0xFF41 }]] auto set_lcd_status(u8 value) -> void;

    [[= io_write{ 0xFF47 }]] auto set_bg_palette(u8 value) -> void;
    [[= io_write{ 0xFF48 }]] auto set_oam_palette_1(u8 value) -> void;
    [[= io_write{ 0xFF49 }]] auto set_oam_palette_2(u8 value) -> void;

  private:
    friend class fifo;

    enum class mode
    {
        horizontal_blank,
        vertical_blank,
        oam_scan,
        drawing,
    };

    emulator* m_system;

    // PPU STAT interrupts
    bool m_ly_compare_interrupt{ false };
    bool m_oam_interrupt{ false };
    bool m_vblank_interrupt{ false };
    bool m_hblank_interrupt{ false };

    std::array<u8, 4> m_dmg_palette{ 0, 1, 2, 3 };
    std::array<std::array<u8, 4>, 2> m_palette_oam{ { { 0, 1, 2, 3 }, { 0, 1, 2, 3 } } };
    auto set_oam_palette(int palette, u8 value) -> void;

    // Events
    frame_event m_frame_callback{};
    auto on_frame_completed() -> void;

    std::array<u8, RENDER_WIDTH * RENDER_HEIGHT * 3> m_bgr24_bitmap{};

    // Status
    int m_dot{ 0 };
    int m_line_dot{ 0 };

    auto show_window() const -> bool;
    int m_window_line{ 0 };

    mode m_mode{ mode::oam_scan };
    bool m_vram_blocked{ false };
    fifo m_fifo;

    // Frame limiter
    std::chrono::steady_clock::time_point m_frame_timer{ std::chrono::steady_clock::now() };

    auto change_mode(mode m) -> void;
    auto next_line() -> void;
    auto hblank_clock() -> void;
    auto vblank_clock() -> void;
    auto oam_scan_clock() -> void;
    auto draw_clock() -> void;

    std::vector<oam_entry> m_sprites_on_line{};
    auto get_sprites() -> void;

    auto rgb_lookup(u8 c) const -> colour;
};

} // namespace cx::gbc
