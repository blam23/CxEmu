#pragma once

#include "../emulator.h"
#include "../mapper.h"

namespace cx::nes
{

class uxrom : public mapper
{
  public:
    uxrom(emulator* system);

    u8 read(u16 addr) override;
    void write(u16 addr, u8 value) override;

    u8 read_chr_rom(u16 addr) override;
    void write_chr_rom(u16 addr, u8 value) override;

    bool is_irq_set() override;
    void scan_line() override;

  private:
    u8 m_ram[0x2000];

    u8 m_page_1{ 0x0 };
    u8 m_page_2{ 0x0 };

    emulator* m_system;
};

} // namespace cx::nes