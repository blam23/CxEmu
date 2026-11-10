#pragma once

#include "core/types.h"
#include <filesystem>
#include <string>
#include <vector>

namespace cx::gbc
{

class cart
{
  public:
    enum class state
    {
        unloaded,
        loaded,
        failed_to_open,
        invalid,
    };

    auto current_state() const
    {
        return m_state;
    }

    auto load(const std::filesystem::path& path) -> bool;

    auto get_ram_in_kb() const -> int;

    // Saving
    auto load_ram() -> void;
    auto save_ram() -> void;

    std::vector<u8> m_rom{};
    std::vector<u8> m_ram{};

    bool m_cgb{ false };
    bool m_sgb_flag{ false };
    u8 m_cart_type{ 0 };
    u8 m_rom_size{ 0 };
    u8 m_ram_size{ 0 };

    std::string m_save_file{};

  private:
    auto validate() -> bool;

    state m_state{ state::unloaded };
};

} // namespace cx::gbc
