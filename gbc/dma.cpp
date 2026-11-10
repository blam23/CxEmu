#include "dma.h"
#include "emulator.h"

namespace cx::gbc
{

dma::dma(emulator* system) : m_system{ system }
{
}

auto dma::start(u8 value) -> void
{
    m_value = value;
    m_active = true;
    m_offset = 0;
    m_delay = 2;
}

auto dma::clock() -> void
{
    if (!m_active)
        return;

    if (m_delay > 0)
    {
        m_delay--;
        return;
    }

    auto& bus{ m_system->m_bus };

    const auto data{ bus.read(static_cast<u16>((m_value * 0x0100) + m_offset)) };
    bus.m_oam[m_offset] = data;

    m_offset++;

    m_active = m_offset <= 0x9F;
}

} // namespace cx::gbc
