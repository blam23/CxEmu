#include "joypad.h"

namespace cx::nes
{

auto joypad::read() -> u8
{
    const auto ret{ m_current_state_read_bit };
    m_current_state_read_bit >>= 1;
    return ret & 0x1;
}

auto joypad::poll(u8 value) -> void
{
    if ((value & 0x1) == 0x1)
        m_current_state_read_bit = m_status.bits;
}

} // namespace cx::nes