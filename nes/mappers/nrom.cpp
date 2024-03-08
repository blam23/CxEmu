#include "nrom.h"

namespace cx::nes
{

nrom::nrom(cart* cart) : m_cart(cart)
{
    m_mask = m_cart->m_prg_rom_size > 1 ? 0x7FFF : 0x3FFF;
}

u8 nrom::read(u16 addr)
{
    if (addr >= 0x6000 && addr < 0x8000)
        return m_ram[addr - 0x6000];

    if (addr >= 0x8000 && addr <= 0xFFFF)
        return m_cart->m_prg_rom[addr & m_mask];

    return 0;
}

void nrom::write(u16 addr, u8 value)
{
    // NROM has no mapping or ctrl

    if (addr >= 0x6000 && addr < 0x8000)
        m_ram[addr - 0x6000] = value;
}

bool nrom::is_irq_set()
{
    return false;
}

u8 nrom::read_chr_rom(u16 addr)
{
    if (addr < 0x2000)
        return m_cart->m_chr_rom[addr];

    return 0;
}

void nrom::write_chr_rom(u16 addr, u8 value)
{
    return;
}

void nrom::scan_line()
{
}

}; // namespace cx::nes
