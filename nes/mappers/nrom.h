#pragma once

#include "../cart.h"
#include "../mapper.h"

namespace cx::nes
{

class nrom : public mapper
{
  public:
    nrom(cart* cart);

    u8 read(u16 addr) override;
    void write(u16 addr, u8 value) override;

    u8 read_chr_rom(u16 addr) override;
    void write_chr_rom(u16 addr, u8 value) override;

    bool is_irq_set() override;
    void scan_line() override;

  private:
    u8 m_ram[0x2000];
    u16 m_mask{ 0x3FFF };
    cart* m_cart;
};

} // namespace cx::nes