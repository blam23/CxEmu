#include "joypad.h"
#include "emulator.h"

namespace cx::gbc
{

joypad::joypad(emulator* system) : m_system{ system }
{
}

auto joypad::press(key k) -> void
{
    const auto mask{ static_cast<u8>(~static_cast<u8>(k)) };
    m_state &= mask;
    m_system->m_cpu.request_interrupt(cpu::interrupt::joypad);
}

auto joypad::release(key k) -> void
{
    m_state |= static_cast<u8>(k);
}

auto joypad::get_joyp() const -> u8
{
    if ((m_select & 0b0001'0000) == 0x00)
        return static_cast<u8>(m_select | (m_state & 0x0F));
    if ((m_select & 0b0010'0000) == 0x00)
        return static_cast<u8>(m_select | (m_state >> 4));

    return m_select;
}

auto joypad::set_joyp(u8 value) -> void
{
    m_select = static_cast<u8>(value | JOYP_ALWAYS_SET_MASK);
}

} // namespace cx::gbc
