#include "memory_bus.h"
#include "byte.h"
#include "emulator.h"
#include "reflection.h"
#include <array>
#include <meta>

namespace cx::gbc
{

namespace
{

using io_reader = auto (*)(emulator*) -> u8;
using io_writer = auto (*)(emulator*, u8) -> void;

struct io_tables
{
    std::array<io_reader, 0x100> readers{};
    std::array<io_writer, 0x100> writers{};
};

// Thunks, one per annotated member. They take the emulator (like the C# static methods did) and reach the
// component through the reflected emulator member `comp`.
template <std::meta::info comp, std::meta::info member> auto read_data(emulator* e) -> u8
{
    return e->[:comp:].[:member:];
}

template <std::meta::info comp, std::meta::info member> auto write_data(emulator* e, u8 value) -> void
{
    e->[:comp:].[:member:] = value;
}

template <std::meta::info comp, std::meta::info member> auto read_function(emulator* e) -> u8
{
    return (e->[:comp:].[:member:])();
}

template <std::meta::info comp, std::meta::info member> auto write_function(emulator* e, u8 value) -> void
{
    (e->[:comp:].[:member:])(value);
}

// Replaces RegisterIOProperties/RegisterStaticIOMethods. Walks the components held by the emulator and registers
// each annotated member. A duplicate address is a compile error.
consteval auto build_io_tables() -> io_tables
{
    io_tables tables{};

    template for (constexpr auto comp : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^emulator, std::meta::access_context::unchecked())))
    {
        // Only components (plain classes); skips the cart pointer-ish members and library templates
        if constexpr (std::meta::is_class_type(std::meta::type_of(comp)) &&
                      !std::meta::has_template_arguments(std::meta::type_of(comp)))
        {
            template for (constexpr auto member : std::define_static_array(
                              std::meta::members_of(std::meta::type_of(comp), std::meta::access_context::unchecked())))
            {
                // [IO] on a property: readable and writable
                template for (constexpr auto annotation :
                              std::define_static_array(std::meta::annotations_of_with_type(member, ^^io)))
                {
                    constexpr auto addr{ std::meta::extract<io>(annotation).address - 0xFF00 };

                    if (tables.writers[addr] != nullptr)
                        throw "Duplicate IO write for address";
                    if (tables.readers[addr] != nullptr)
                        throw "Duplicate IO read for address";

                    tables.writers[addr] = &write_data<comp, member>;
                    tables.readers[addr] = &read_data<comp, member>;
                }

                template for (constexpr auto annotation :
                              std::define_static_array(std::meta::annotations_of_with_type(member, ^^io_read)))
                {
                    constexpr auto addr{ std::meta::extract<io_read>(annotation).address - 0xFF00 };

                    if (tables.readers[addr] != nullptr)
                        throw "Duplicate IO read for address";

                    tables.readers[addr] = &read_function<comp, member>;
                }

                template for (constexpr auto annotation :
                              std::define_static_array(std::meta::annotations_of_with_type(member, ^^io_write)))
                {
                    constexpr auto addr{ std::meta::extract<io_write>(annotation).address - 0xFF00 };

                    if (tables.writers[addr] != nullptr)
                        throw "Duplicate IO write for address";

                    tables.writers[addr] = &write_function<comp, member>;
                }
            }
        }
    }

    return tables;
}

constexpr io_tables s_io{ build_io_tables() };

} // namespace

memory_bus::memory_bus(emulator* system) : m_system{ system }
{
}

auto memory_bus::write(u16 addr, u8 data) -> void
{
    // ROM
    if (addr < 0x8000)
        m_system->m_mapper->write(addr, data);
    else if (addr < 0xA000)
    {
        if (m_system->m_ppu.vram_accessible())
            m_vram[addr - 0x8000] = data;
    }
    else if (addr < 0xC000)
        m_system->m_mapper->write(addr, data);
    else if (addr < 0xE000)
        m_wram[addr - 0xC000] = data;
    else if (addr < 0xFE00) // Echo RAM
        m_wram[(addr - 0xC000) % 0x2000] = data;
    else if (addr < 0xFEA0)
    {
        if (m_system->m_dma.transfer_active())
            return;

        m_oam[addr - 0xFE00] = data;
    }
    else if (addr < 0xFF00) // Unusable memory
        return;
    else if (addr >= 0xFF80 && addr < 0xFFFF)
        m_hram[addr - 0xFF80] = data;
    else
    {
        const auto action{ s_io.writers[addr - 0xFF00] };

        if (action != nullptr)
            action(m_system, data);
    }
}

auto memory_bus::read(u16 addr) -> u8
{
    if (addr < 0x8000)
        return m_system->m_mapper->read(addr);

    if (addr < 0xA000)
    {
        if (m_system->m_ppu.vram_accessible())
            return m_vram[addr - 0x8000];
        else
            return 0xFF;
    }

    if (addr < 0xC000)
        return m_system->m_mapper->read(addr);

    // TODO: Swappable RAM
    if (addr < 0xE000)
        return m_wram[addr - 0xC000];

    // Echo RAM
    if (addr < 0xFE00)
        return m_wram[addr - 0xE000];

    if (addr < 0xFEA0)
        return m_oam[addr - 0xFE00];

    // Unusable memory
    if (addr < 0xFF00)
        return 0xFF;

    const auto action{ s_io.readers[addr - 0xFF00] };

    if (action != nullptr)
        return action(m_system);

    if (addr >= 0xFF80 && addr < 0xFFFE)
        return m_hram[addr - 0xFF80];

    return 0xFF;
}

auto memory_bus::read_word(u16 addr) -> u16
{
    u16 r{ read(static_cast<u16>(addr + 1)) };
    r = static_cast<u16>(r << 8);
    r += read(addr);
    return r;
}

auto memory_bus::write_word(u16 addr, u16 val) -> void
{
    write(addr, byte::lower(val));
    write(++addr, byte::upper(val));
}

auto memory_bus::shutdown() -> void
{
    m_system->m_mapper->shutdown();
}

} // namespace cx::gbc
