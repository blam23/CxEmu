#include "ppu_bus.h"
#include "emulator.h"
#include <iostream>

namespace cx::nes
{

ppu_bus::ppu_bus(emulator* system) : m_system{ system }
{
}

u8 ppu_bus::read(u16 addr)
{
    if (addr < 0x2000)
    {
        if (m_system->m_cart.m_chr_rom_size == 0)
            return m_chr_ram[addr];

        return m_system->m_mapper->read_chr_rom(addr);
    }
    else if (addr < 0x3F00)
    {
        u16 naddr = addr & 0xFC00;
        if (naddr == 0x2000)
            return m_name_tables[m_name_table_map[0]][addr & 0x3FF];
        else if (naddr == 0x2400)
            return m_name_tables[m_name_table_map[1]][addr & 0x3FF];
        else if (naddr == 0x2800)
            return m_name_tables[m_name_table_map[2]][addr & 0x3FF];
        else
            return m_name_tables[m_name_table_map[3]][addr & 0x3FF];
    }
    else if (addr < 0x3FFF)
    {
        addr = get_palette_address(addr);
        return m_palette[addr];
    }

    return 0;
}

void ppu_bus::write(u16 addr, u8 value)
{
    if (addr < 0x2000 && m_system->m_cart.m_chr_rom_size == 0)
    {
        m_chr_ram[addr] = value;
        return;
    }

    if (addr >= 0x2000 && addr < 0x3F00)
    {
        u16 naddr = addr & 0xFC00;
        if (naddr == 0x2000)
            m_name_tables[m_name_table_map[0]][addr & 0x3FF] = value;
        else if (naddr == 0x2400)
            m_name_tables[m_name_table_map[1]][addr & 0x3FF] = value;
        else if (naddr == 0x2800)
            m_name_tables[m_name_table_map[2]][addr & 0x3FF] = value;
        else
            m_name_tables[m_name_table_map[3]][addr & 0x3FF] = value;
        return;
    }

    if (addr < 0x3FFF)
    {
        addr = get_palette_address(addr);
        m_palette[addr] = value;
        return;
    }

    m_system->m_mapper->write_chr_rom(addr, value);
}

u16 ppu_bus::read_word(u16 addr)
{
    u16 r = read(addr + 1);
    r <<= 8;
    r += read(addr);
    return r;
}

u16 ppu_bus::read_word_wrapped(u16 addr)
{
    u16 high = (addr & 0xFF) == 0xFF ? addr - 0xFF : addr + 1;
    return read(addr) | read(high) << 8;
}

auto ppu_bus::get_palette_address(u16 addr) -> u16
{
    addr &= 0x001F;

    if (addr == 0x0010)
        return 0x0000;
    if (addr == 0x0014)
        return 0x0004;
    if (addr == 0x0018)
        return 0x0008;
    if (addr == 0x001C)
        return 0x000C;

    return addr;
}

void ppu_bus::update_mirroring(mirroring m)
{
    switch (m)
    {
    case mirroring::horizontal:
        m_name_table_map[0] = 0;
        m_name_table_map[1] = 0;
        m_name_table_map[2] = 1;
        m_name_table_map[3] = 1;
        break;
    case mirroring::vertical:
        m_name_table_map[0] = 0;
        m_name_table_map[1] = 1;
        m_name_table_map[2] = 0;
        m_name_table_map[3] = 1;
        break;
    case mirroring::one_screen_lower:
        m_name_table_map[0] = 0;
        m_name_table_map[1] = 0;
        m_name_table_map[2] = 0;
        m_name_table_map[3] = 0;
        break;
    case mirroring::one_screen_upper:
        m_name_table_map[0] = 1;
        m_name_table_map[1] = 1;
        m_name_table_map[2] = 1;
        m_name_table_map[3] = 1;
        break;
    }
}

} // namespace cx::nes