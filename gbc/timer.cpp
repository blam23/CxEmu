#include "timer.h"
#include "emulator.h"
#include <stdexcept>

namespace cx::gbc
{

timer::timer(emulator* system) : m_system{ system }
{
}

auto timer::get_div() const -> u8
{
    return m_div;
}

auto timer::set_div(u8) -> void
{
    m_div = 0;
}

auto timer::get_tac() const -> u8
{
    return static_cast<u8>(((m_timer_enable ? 0x04 : 0x00) + m_tct) | TAC_ALWAYS_SET_MASK);
}

auto timer::set_tac(u8 value) -> void
{
    m_timer_enable = (value & 0x04) == 0x04;
    m_tct = static_cast<u8>(value & 0x03);

    switch (m_tct)
    {
    case 0b00:
        m_timer_clocks = 0x400;
        break;
    case 0b01:
        m_timer_clocks = 0x010;
        break;
    case 0b10:
        m_timer_clocks = 0x040;
        break;
    case 0b11:
        m_timer_clocks = 0x100;
        break;

    default:
        throw std::runtime_error{ "Invalid data" };
    }
}

auto timer::clock() -> void
{
    if (m_system->m_cpu.stopped())
        return;

    if (m_clock % DIV_CLOCKS == 0)
        m_div++;

    if (m_timer_enable)
    {
        if (m_clock % m_timer_clocks == 0)
        {
            m_tima++;

            if (m_tima == 0x00)
            {
                m_tima = m_tma;
                m_system->m_cpu.request_interrupt(cpu::interrupt::timer);
            }
        }
    }

    m_clock++;
}

} // namespace cx::gbc
