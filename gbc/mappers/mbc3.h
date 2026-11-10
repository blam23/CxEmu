#pragma once

#include "../mapper.h"
#include "../reflection.h"

namespace cx::gbc::mappers
{

// clang-format off
class [[= mbc{ 0x11 }]] [[= mbc{ 0x12 }]] [[= mbc{ 0x13 }]] mbc3 : public mapper
// clang-format on
{
  public:
    mbc3(emulator* system);

    auto read(u16 addr) -> u8 override;
    auto write(u16 addr, u8 value) -> void override;
    auto shutdown() -> void override;

  private:
    auto set_rom_bank(u8 value) -> void;
    auto set_ram_bank(u8 value) -> void;
    auto set_ram_enable(u8 value) -> void;

    // Clock (not implemented in the C# either)
    auto read_clock(u16 addr) -> u8;
    auto write_clock(u16 addr, u8 value) -> void;
    auto update_latch(u8 value) -> void;
    auto set_latch() -> void;

    cart& m_cart;

    bool m_battery{ false };
    bool m_timer{ false };

    int m_rom_bank{ 1 };
    int m_ram_bank{ 0 };
    bool m_ram_enable{ false };
    bool m_clock_mode{ false };
    bool m_latch_can_enable{ false };
    bool m_latch_enabled{ false };
};

} // namespace cx::gbc::mappers
