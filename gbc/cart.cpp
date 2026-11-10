#include "cart.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string_view>

namespace cx::gbc
{

auto cart::load(const std::filesystem::path& path) -> bool
{
    // The save file is the path with ".axsav" appended (not replacing the extension)
    m_save_file = path.string() + ".axsav";

    std::ifstream file{ path, std::ios::binary };
    if (!file.is_open())
    {
        m_state = state::failed_to_open;
        return false;
    }

    m_rom = std::vector<u8>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    if (!validate())
    {
        m_state = state::invalid;
        return false;
    }

    m_state = state::loaded;
    return true;
}

auto cart::get_ram_in_kb() const -> int
{
    switch (m_ram_size)
    {
    case 1:
        return 2;
    case 2:
        return 8;
    case 3:
        return 32;
    case 4:
        return 128;
    case 5:
        return 64;

    default:
        return 0;
    }
}

auto cart::validate() -> bool
{
    // The header ends at 0x14F
    if (m_rom.size() < 0x150)
        return false;

    const std::string_view title_data{ reinterpret_cast<const char*>(m_rom.data()) + 0x0134, 0x0143 - 0x0134 };
    const auto title{ title_data.substr(0, title_data.find('\0')) };

    if (m_rom[0x0146] == 0x3)
        m_sgb_flag = true;

    m_cart_type = m_rom[0x0147];
    m_rom_size = m_rom[0x0148];
    m_ram_size = m_rom[0x0149];

    // CGB flag can be either 0x80 or 0xC0, bit 6 is ignored.
    m_cgb = (m_rom[0x0143] | 0x40) == 0xC0;

    m_ram = std::vector<u8>(static_cast<std::size_t>(get_ram_in_kb()) * 0x400);

    spdlog::debug("Title:    '{}'", title);
    spdlog::debug("CGB Game: {}", m_cgb ? 'T' : 'F');
    spdlog::debug("CartType: {:02X}", m_cart_type);
    spdlog::debug("ROMSize:  {:02X} ({:X}KB)", m_rom_size, (1024 * 32) << m_rom_size);
    spdlog::debug("RAMSize:  {:02X} ({}KB)", m_ram_size, get_ram_in_kb());
    spdlog::debug("SGBFlag:  {}", m_sgb_flag ? 'T' : 'F');

    return true;
}

//
// Saving
//
auto cart::load_ram() -> void
{
    if (!std::filesystem::exists(m_save_file))
        return;

    std::ifstream file{ m_save_file, std::ios::binary | std::ios::ate };
    const auto size{ static_cast<std::size_t>(file.tellg()) };

    // C# throws if the save is larger than the RAM
    if (size > m_ram.size())
        throw std::runtime_error{ "Save file is larger than the cart RAM" };

    file.seekg(0);
    file.read(reinterpret_cast<char*>(m_ram.data()), static_cast<std::streamsize>(size));
}

auto cart::save_ram() -> void
{
    std::ofstream file{ m_save_file, std::ios::binary | std::ios::trunc };
    file.write(reinterpret_cast<const char*>(m_ram.data()), static_cast<std::streamsize>(m_ram.size()));
    file.flush();
}

} // namespace cx::gbc
