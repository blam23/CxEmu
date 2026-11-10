#include "no_mbc.h"
#include "../emulator.h"
#include <stdexcept>

namespace cx::gbc::mappers
{

no_mbc::no_mbc(emulator* system) : m_cart{ system->m_cart }
{
}

auto no_mbc::read(u16 addr) -> u8
{
    if (addr < 0x8000)
    {
        if (addr >= m_cart.m_rom.size())
            return 0;

        return m_cart.m_rom[addr];
    }

    if (addr >= 0xA000 && addr < 0xC000)
    {
        addr -= 0xA000;

        if (addr >= m_cart.m_ram.size())
            return 0;

        return m_cart.m_ram[addr];
    }

    throw std::runtime_error{ "Invalid operation" };
}

auto no_mbc::write(u16 addr, u8 value) -> void
{
    if (addr >= 0xA000 && addr < 0xC000)
    {
        addr -= 0xA000;

        if (addr >= m_cart.m_ram.size())
            return;

        m_cart.m_ram[addr] = value;
    }
}

} // namespace cx::gbc::mappers
