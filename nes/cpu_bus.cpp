#include "cpu_bus.h"
#include <iostream>

namespace cx::nes
{

cpu_bus::cpu_bus(ppu* ppu, apu* apu, mapper* mapper) : m_ppu{ ppu }, m_apu{ apu }, m_mapper{ mapper }
{
}

u8 cpu_bus::read(u16 addr)
{
    if (addr < 0x2000)
        return m_ram[addr];
    if (addr < 0x4000)
        return m_ppu->read(addr);
    if (addr == 0x4016)
        return 0; // TODO: joypad1
    if (addr == 0x4017)
        return 0; // TODO: joypad2
    if (addr < 0x4020)
        return m_apu->read(addr);

    return m_mapper->read(addr);
}

void cpu_bus::write(u16 addr, u8 value)
{
    if (addr < 0x2000)
        m_ram[addr] = value;
    else if (addr < 0x4000)
        m_ppu->write(addr, value);
    else if (addr == 0x4016)
        std::cout << "jp1 write: " << value << std::endl;
    else if (addr == 0x4017)
        std::cout << "jp2 write: " << value << std::endl;
    else if (addr < 0x4020)
        m_apu->write(addr, value);
    else
        m_mapper->write(addr, value);
}

u16 cpu_bus::read_word(u16 addr)
{
    u16 r = read(addr + 1);
    r <<= 8;
    r += read(addr);
    return r;
}

u16 cpu_bus::read_word_wrapped(u16 addr)
{
    u16 high = (addr & 0xFF) == 0xFF ? addr - 0xFF : addr + 1;
    return read(addr) | read(high) << 8;
}

} // namespace cx::nes