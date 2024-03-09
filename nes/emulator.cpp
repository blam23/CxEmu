#include "emulator.h"

#include <iostream>

namespace cx::nes
{

emulator::emulator(cart&& cart)
    : cx::emulator_base(89342, 60.0), m_cart{ cart }, m_mapper{ mapper::create(m_cart.mapper(), this) }, m_ppu{ this },
      m_cpu{ this }
{
    set_mirror(m_cart.m_mirroring);
}

void emulator::clock()
{
    // Always clock PPU
    m_ppu.clock();

    // Clock APU every 6 cycles
    if (m_clock % 6 == 0)
    {
        m_apu.clock();
    }

    // Clock CPU every 3 cycles
    if (m_clock % 3 == 0)
    {
        if (m_mapper->is_irq_set())
            m_cpu.set_irq();

        m_cpu.clock();
    }

    m_clock++;
}

void emulator::reset()
{
}

void emulator::shudown()
{
}

void emulator::set_mirror(mirroring m)
{
    m_ppu.update_mirroring(m);
}

emulator emulator::load_rom(std::string_view path)
{
    cart c;

    if (c.load(path))
        std::cout << "<> ROM Loaded." << std::endl;
    else
        std::cerr << "Failed to load rom file: '" << path << "'." << std::endl;

    return { std::move(c) };
}

} // namespace cx::nes