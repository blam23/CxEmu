#pragma once

#include "../emulator.h"
#include "../mapper.h"

namespace cx::nes
{

class nrom : public mapper
{
  public:
    nrom(emulator* system);

    auto read(u16 addr) const -> u8 override;
    auto write(u16 addr, u8 value) -> void override;

    auto read_chr_rom(u16 addr) const -> u8 override;
    auto write_chr_rom(u16 addr, u8 value) -> void override;

    auto is_irq_set() -> bool override;
    auto scan_line() -> void override;

  private:
    u8 m_ram[0x2000];
    u16 m_mask{ 0x3FFF };
    emulator* m_system;
};

} // namespace cx::nes