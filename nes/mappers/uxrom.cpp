#include "uxrom.h"

namespace cx::nes
{

uxrom::uxrom(emulator* system) : m_system(system)
{
    m_page_2 = m_system->m_cart.m_prg_rom_size - 1;
}

auto uxrom::read(u16 addr) const -> u8
{
    if (addr >= 0x6000 && addr < 0x8000)
        return m_ram[addr - 0x6000];

    if (addr >= 0x8000 && addr <= 0xBFFF)
        return m_system->m_cart.m_prg_rom[m_page_1 * 0x4000 + (addr & 0x3FFF)];

    if (addr >= 0xC000)
        return m_system->m_cart.m_prg_rom[m_page_2 * 0x4000 + (addr & 0x3FFF)];

    return 0;
}

auto uxrom::write(u16 addr, u8 value) -> void
{
    if (addr >= 0x6000 && addr < 0x8000)
        m_ram[addr - 0x6000] = value;

    if (addr >= 0x8000)
        m_page_1 = value & 0x0F;
}

auto uxrom::is_irq_set() -> bool
{
    return false;
}

auto uxrom::read_chr_rom(u16 addr) const -> u8
{
    if (addr < 0x2000)
        return m_system->m_cart.m_chr_rom[addr];

    return 0;
}

auto uxrom::write_chr_rom(u16 addr, u8 value) -> void
{
    return;
}

auto uxrom::scan_line() -> void
{
}

}; // namespace cx::nes
