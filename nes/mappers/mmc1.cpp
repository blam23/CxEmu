#include "mmc1.h"

namespace cx::nes
{

mmc1::mmc1(emulator* system) : m_system(system)
{
    m_control = 0x1C;
    m_prg_page_2 = m_system->m_cart.m_prg_rom_size - 1;
}

u8 mmc1::read(u16 addr)
{
    if (addr >= 0x6000 && addr < 0x8000)
        return m_ram[addr - 0x6000];

    if (addr >= 0x8000)
    {
        if ((m_control & 0b01000) != 0)
        {
            // 2 chunk, 16k mode
            if (addr < 0xC000)
                return m_system->m_cart.m_prg_rom[m_prg_page_1 * 0x4000 + (addr & 0x3FFF)];
            else
                return m_system->m_cart.m_prg_rom[m_prg_page_2 * 0x4000 + (addr & 0x3FFF)];
        }
        else
        {
            // 1 chunk, 32k mode
            return m_system->m_cart.m_prg_rom[m_prg_page_chunk * 0x8000 + (addr & 0x7FFF)];
        }
    }

    return 0;
}

void mmc1::write(u16 addr, u8 value)
{
    if (addr >= 0x6000 && addr < 0x8000)
    {
        m_ram[addr - 0x6000] = value;
        return;
    }

    if (addr > 0x8000)
    {
        // Clear if value high bit is set
        if ((value & 0x80) != 0)
        {
            m_load = 0;
            m_load_count = 0;
            m_control |= 0x0C;
        }
        else
        {
            // Shift in value
            m_load >>= 1;
            m_load |= (value & 0x01) << 4;
            m_load_count++;

            if (m_load_count == 5)
            {
                auto target{ (addr >> 13) & 0x3 };

                switch (target)
                {
                case 0:
                    // Control Setup
                    m_control = m_load & 0x1F;

                    switch (m_control & 0x3)
                    {
                    case 0:
                        m_system->set_mirror(mirroring::one_screen_lower);
                        break;
                    case 1:
                        m_system->set_mirror(mirroring::one_screen_upper);
                        break;
                    case 2:
                        m_system->set_mirror(mirroring::vertical);
                        break;
                    case 3:
                        m_system->set_mirror(mirroring::horizontal);
                        break;
                    }

                    break;
                case 1:
                    // ChrRom High / Chunk setup
                    if ((m_control & 0b10000) != 0)
                        m_chr_page_1 = m_load & 0x1F;
                    else
                        m_chr_page_chunk = m_load & 0x1E;
                    break;
                case 2:
                    // ChrRom Hi setup
                    if ((m_control & 0b10000) != 0)
                        m_chr_page_2 = m_load & 0x1F;
                    break;
                case 3: {
                    const auto prgMode{ (m_control >> 2) & 0x3 };

                    switch (prgMode)
                    {
                    case 0:
                    case 1:
                        // 32k mode
                        m_prg_page_chunk = (m_load & 0x0E) >> 1;
                        break;
                    case 2:
                        // First page fixed, second page set to m_load
                        m_prg_page_1 = 0;
                        m_prg_page_2 = m_load & 0x0F;
                        break;
                    case 3:
                        // First page m_load, second page set to last page
                        m_prg_page_1 = m_load & 0x0F;
                        m_prg_page_2 = m_system->m_cart.m_prg_rom_size - 1;
                        break;
                    default:
                        break;
                    }

                    break;
                }
                }

                m_load = 0;
                m_load_count = 0;
            }
        }
    }
}

bool mmc1::is_irq_set()
{
    return false;
}

u8 mmc1::read_chr_rom(u16 addr)
{
    if (addr < 0x2000)
    {
        if ((m_control & 0b10000) != 0)
        {
            // 2 chunk, 4k mode
            if (addr < 0x1000)
                return m_system->m_cart.m_chr_rom[m_chr_page_1 * 0x1000 + (addr & 0x0FFF)];
            else
                return m_system->m_cart.m_chr_rom[m_chr_page_2 * 0x1000 + (addr & 0x0FFF)];
        }
        else
        {
            // 1 chunk, 8k mode
            return m_system->m_cart.m_chr_rom[m_chr_page_chunk * 0x2000 + (addr & 0x1FFF)];
        }
    }

    return 0;
}

void mmc1::write_chr_rom(u16 addr, u8 value)
{
    return;
}

void mmc1::scan_line()
{
}

}; // namespace cx::nes
