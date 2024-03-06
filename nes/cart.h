#pragma once

#include "core/types.h"
#include <filesystem>
#include <fstream>
#include <vector>

namespace cx::nes
{
class cart
{
  public:
    enum class state
    {
        unloaded,
        loaded,
        failed_to_open,
        invalid
    };

    enum class region
    {
        NTSC,
        PAL,
        multi,
        dendy
    };

    enum class mirroring
    {
        horizontal,
        vertical,
        one_screen_lower,
        one_screen_upper
    };

    auto current_state() const
    {
        return m_state;
    }

    bool load(const std::filesystem::path& path);

    auto mapper() const
    {
        return m_mapper;
    }

    std::vector<u8> m_prg_rom{};
    std::vector<u8> m_chr_rom{};
    u8 m_prg_rom_size;

  private:
    bool try_load();

    std::vector<u8> m_rom{};
    state m_state{ state::unloaded };

    // cart data
    u8 m_mapper;
    u8 m_chr_rom_size;

    mirroring m_mirroring{ mirroring::horizontal };
    bool m_battery_present;
    region m_region;

    static constexpr u16 PRG_PAGE_SIZE{ 16 * 1024 };
    static constexpr u16 CHR_PAGE_SIZE{ 8 * 1024 };
};
} // namespace cx::nes