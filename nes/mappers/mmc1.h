#pragma once

#include "../emulator.h"
#include "../mapper.h"

namespace cx::nes
{

class mmc1 : public mapper
{
  public:
    mmc1(emulator* system);

    u8 read(u16 addr) override;
    void write(u16 addr, u8 value) override;

    u8 read_chr_rom(u16 addr) override;
    void write_chr_rom(u16 addr, u8 value) override;

    bool is_irq_set() override;
    void scan_line() override;

  private:
    u8 m_ram[0x2000];

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