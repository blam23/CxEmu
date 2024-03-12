#pragma once

#include "../emulator.h"
#include "../mapper.h"

namespace cx::nes
{

class mmc1 : public mapper
{
  public:
    mmc1(emulator* system);

    auto read(u16 addr) const -> u8 override;
    auto write(u16 addr, u8 value) -> void override;

    auto read_chr_rom(u16 addr) const -> u8 override;
    auto write_chr_rom(u16 addr, u8 value) -> void override;

    auto is_irq_set() -> bool override;
    auto scan_line() -> void override;

  private:
    u8 m_ram[0x2000]{ 0 };

    u8 m_chr_page_1{ 0x0 };
    u8 m_chr_page_2{ 0x0 };
    u8 m_chr_page_chunk{ 0x0 };

    u8 m_prg_page_1{ 0x0 };
    u8 m_prg_page_2{ 0x0 };
    u8 m_prg_page_chunk{ 0x0 };

    // Registers
    u8 m_load{ 0 };
    u8 m_load_count{ 0 };
    u8 m_control{ 0 };

    emulator* m_system;
};

} // namespace cx::nes