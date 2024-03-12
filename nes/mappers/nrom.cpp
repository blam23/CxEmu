#include "nrom.h"

namespace cx::nes
{

nrom::nrom(emulator* system) : m_system(system)
{
    m_mask = m_system->m_cart.m_prg_rom_size > 1 ? 0x7FFF : 0x3FFF;
}

auto nrom::read(u16 addr) const -> u8
{
    if (addr >= 0x6000 && addr < 0x8000)
        return m_ram[addr - 0x6000];

    if (addr >= 0x8000 && addr <= 0xFFFF)
        return m_system->m_cart.m_prg_rom[addr & m_mask];

    return 0;
}

auto nrom::write(u16 addr, u8 value) -> void
{
    // NROM has no mapping or ctrl

    if (addr >= 0x6000 && addr < 0x8000)
        m_ram[addr - 0x6000] = value;
}

auto nrom::is_irq_set() -> bool
{
    return false;
}

auto nrom::read_chr_rom(u16 addr) const -> u8
{
    if (addr < 0x2000)
        return m_system->m_cart.m_chr_rom[addr];

    return 0;
}

auto nrom::write_chr_rom(u16 addr, u8 value) -> void
{
    return;
}

auto nrom::scan_line() -> void
{
}

}; // namespace cx::nes
