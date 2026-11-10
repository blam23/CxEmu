#pragma once

#include "core/types.h"
#include "forward.h"
#include <array>
#include <span>
#include <vector>

namespace cx::gbc
{

struct colour
{
    u8 r;
    u8 g;
    u8 b;
};

struct oam_entry
{
    u8 x{ 0 };
    u8 y{ 0 };
    u8 tile{ 0 };

    u8 palette{ 0 };
    bool vram_bank{ false };
    bool flip_x{ false };
    bool flip_y{ false };
    bool bg_prio{ false };

    oam_entry() = default;
    explicit oam_entry(std::span<const u8, 4> data);
};

// The pixel queue holds at most 16 colours (it is only added to when it has 8 or fewer, and then adds 8)
class colour_queue
{
  public:
    auto size() const -> std::size_t
    {
        return m_count;
    }

    auto enqueue(colour c) -> void
    {
        m_data[(m_head + m_count) % m_data.size()] = c;
        m_count++;
    }

    auto dequeue() -> colour
    {
        const auto c{ m_data[m_head] };
        m_head = (m_head + 1) % m_data.size();
        m_count--;
        return c;
    }

    auto clear() -> void
    {
        m_head = 0;
        m_count = 0;
    }

  private:
    std::array<colour, 16> m_data{};
    std::size_t m_head{ 0 };
    std::size_t m_count{ 0 };
};

class fifo
{
  public:
    fifo(ppu* ppu);

    auto reset() -> void;
    auto clock() -> void;

    u8 m_pushed_x{ 0 };

  private:
    ppu* m_ppu;

    enum class mode
    {
        tile,
        data_low,
        data_high,
        sleep,
        push
    };

    colour_queue m_fifo{};

    u8 m_fetch_x{ 0 };
    u8 m_map_x{ 0 };
    u8 m_map_y{ 0 };
    u8 m_tile_y{ 0 };
    u8 m_line_x{ 0 };
    u8 m_fifo_x{ 0 };

    mode m_mode{ mode::tile };

    u8 m_current_tile{ 0 };
    u8 m_tile_hi{ 0 };
    u8 m_tile_lo{ 0 };
    bool m_in_window{ false };

    std::vector<oam_entry> m_sprites{};
    std::vector<u8> m_sprite_data{};

    auto adjust_tile() -> void;
    auto get_tile() -> void;
    auto get_window() -> void;
    auto get_sprites() -> void;
    auto get_sprite_data() -> void;
    auto get_tile_low() -> void;
    auto get_tile_high() -> void;
    auto push() -> void;
    auto add() -> bool;
    auto fetch_sprite_pixel(u8 bg_colour) -> std::pair<bool, u8>;
    auto fetch() -> void;
};

} // namespace cx::gbc
