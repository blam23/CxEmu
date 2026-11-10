#include "emulator.h"
#include <stdexcept>

namespace cx::gbc
{

emulator::emulator(cart&& cart)
    : cx::emulator_base(ppu::ONE_FRAME_IN_DOTS, 59.7), m_cart{ std::move(cart) },
      m_mapper{ mapper::create(m_cart.m_cart_type, this) }, m_bus{ this }, m_ppu{ this }, m_dma{ this },
      m_timer{ this }, m_cpu{ this }, m_jp1{ this }
{
    reset();
}

auto emulator::clock() -> u8
{
    m_cpu.clock();

    if (!m_cpu.stopped())
    {
        m_dma.clock();

        for (u32 i = 0; i < m_cpu.cycles_last_clock(); i += 4)
        {
            m_ppu.clock();
            m_timer.clock();
        }
    }

    return static_cast<u8>(m_cpu.cycles_last_clock());
}

auto emulator::reset() -> void
{
    m_cpu.reset();
}

auto emulator::shudown() -> void
{
    // Save any state for battery backed carts, etc.
    m_bus.shutdown();
}

auto emulator::load_rom(std::string_view path) -> emulator
{
    cart c;
    c.load(path);

    if (c.current_state() == cart::state::failed_to_open)
        throw std::runtime_error{ "Unable to open ROM file" };

    if (c.current_state() == cart::state::invalid)
        throw std::runtime_error{ "Unable to parse ROM file" };

    if (c.current_state() != cart::state::loaded)
        throw std::runtime_error{ "Error loading ROM file" };

    return { std::move(c) };
}

} // namespace cx::gbc
