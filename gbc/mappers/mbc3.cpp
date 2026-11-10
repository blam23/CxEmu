#include "mbc3.h"
#include "../emulator.h"
#include "spdlog/spdlog.h"
#include <stdexcept>

namespace cx::gbc::mappers
{

mbc3::mbc3(emulator* system) : m_cart{ system->m_cart }
{
    // Only 0x11-0x13 are registered, so the 0x0F/0x10 checks are never true for a created mapper (as in the C#)
    m_battery = m_cart.m_cart_type == 0x0F || m_cart.m_cart_type == 0x10 || m_cart.m_cart_type == 0x13;
    m_timer = m_cart.m_cart_type == 0x0F || m_cart.m_cart_type == 0x10;

    if (m_battery)
        m_cart.load_ram();
}

auto mbc3::shutdown() -> void
{
    if (m_battery)
        m_cart.save_ram();
}

auto mbc3::read(u16 addr) -> u8
{
    // Fixed ROM bank 0
    if (addr < 0x4000)
    {
        if (addr >= m_cart.m_rom.size())
            return 0;

        return m_cart.m_rom[addr];
    }

    // Swappable ROM bank 01-7F
    if (addr < 0x8000)
    {
        const auto rom_addr{ (addr - 0x4000) + (m_rom_bank * 0x4000) };

        // The bank clamp allows bank == bank count, so this can run off the end (C# throws)
        return m_cart.m_rom.at(rom_addr);
    }

    // RAM banks 00-03
    if (addr >= 0xA000 && addr < 0xC000)
    {
        if (!m_ram_enable)
            return 0xFF;

        if (m_clock_mode)
            return read_clock(addr);

        const auto ram_addr{ (addr - 0xA000) + (m_ram_bank * 0x2000) };
        return m_cart.m_ram.at(ram_addr);
    }

    throw std::runtime_error{ "Invalid operation" };
}

auto mbc3::write(u16 addr, u8 value) -> void
{
    if (addr < 0x2000)
        set_ram_enable(value);

    else if (addr < 0x4000)
        set_rom_bank(value);

    else if (addr < 0x6000)
        set_ram_bank(value);

    else if (addr < 0x8000)
        update_latch(value);

    else if (addr >= 0xA000 && addr < 0xC000)
    {
        if (!m_ram_enable)
            return;

        if (m_clock_mode)
        {
            write_clock(addr, value);
        }
        else
        {
            const auto ram_addr{ (addr - 0xA000) + (m_ram_bank * 0x2000) };
            if (ram_addr >= static_cast<int>(m_cart.m_ram.size()))
                return;

            m_cart.m_ram[ram_addr] = value;
        }
    }
}

auto mbc3::set_rom_bank(u8 value) -> void
{
    int bank{ value & 0x7F };

    if (bank == 0x00)
        bank = 0x01;
    if (bank == 0x20)
        bank = 0x21;
    if (bank == 0x40)
        bank = 0x41;
    if (bank == 0x60)
        bank = 0x61;

    if (bank > static_cast<int>(m_cart.m_rom.size()) / 0x4000)
        bank = static_cast<int>(m_cart.m_rom.size()) / 0x4000;

    m_rom_bank = bank;
}

auto mbc3::set_ram_bank(u8 value) -> void
{
    if (value <= 0x03)
    {
        const int bank{ value & 0x03 };

        if (bank > static_cast<int>(m_cart.m_ram.size()) / 0x2000)
            return;

        m_ram_bank = bank;
    }
    else
    {
        // ClockMode is never cleared again
        if (value >= 0x08 && value <= 0x0C)
            m_clock_mode = true;
    }
}

auto mbc3::set_ram_enable(u8 value) -> void
{
    m_ram_enable = (value & 0x0F) == 0x0A;
}

//
// Clock
//
// TODO: This
//
auto mbc3::read_clock(u16 addr) -> u8
{
    const u8 value{ 0x00 };
    spdlog::debug("Clock Read: ${:04X} -> {:02X}", addr, value);
    return value;
}

auto mbc3::write_clock(u16 addr, u8 value) -> void
{
    spdlog::debug("Clock Write: ${:04X} <- {:02X}", addr, value);
}

auto mbc3::update_latch(u8 value) -> void
{
    if (m_latch_can_enable && value == 0x01)
    {
        m_latch_can_enable = false;
        set_latch();
        m_latch_enabled = true;
    }
    if (!m_latch_can_enable && value == 0x00)
    {
        m_latch_enabled = false;
        m_latch_can_enable = true;
    }
}

auto mbc3::set_latch() -> void
{
    spdlog::debug("Latch Clock");
}

} // namespace cx::gbc::mappers
