#pragma once

#include "../mapper.h"
#include "../reflection.h"

namespace cx::gbc::mappers
{

// clang-format off
class [[= mbc{ 0x01 }]] [[= mbc{ 0x02 }]] [[= mbc{ 0x03 }]] mbc1 : public mapper
// clang-format on
{
  public:
    mbc1(emulator* system);

    auto read(u16 addr) -> u8 override;
    auto write(u16 addr, u8 value) -> void override;
    auto shutdown() -> void override;

  private:
    auto set_rom_bank(u8 value) -> void;
    auto set_ram_bank(u8 value) -> void;
    auto set_ram_enable(u8 value) -> void;

    cart& m_cart;

    bool m_battery{ false };

    int m_rom_bank{ 1 };
    int m_ram_bank{ 0 };
    bool m_ram_enable{ false };
};

} // namespace cx::gbc::mappers
