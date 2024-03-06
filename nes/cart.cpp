#include "cart.h"

#include <iostream>

namespace cx::nes
{

bool cart::load(const std::filesystem::path& path)
{
    try
    {
        std::ifstream file{ path, std::ios::binary };
        m_rom = std::vector<u8>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }
    catch (const std::exception& ex)
    {
        m_state = state::failed_to_open;
        std::cerr << ex.what() << std::endl;
        return false;
    }

    return try_load();
}

bool cart::try_load()
{
    //
    // Basic verification
    //

    if (m_rom.size() < 16)
        return false;

    if (m_rom[0] != 'N' || m_rom[1] != 'E' || m_rom[2] != 'S' || m_rom[3] != 0x1A)
        return false;

    //
    // Get the important data
    //

    m_prg_rom_size = m_rom[4];
    m_chr_rom_size = m_rom[5];

    auto flags6 = m_rom[6];
    m_mirroring = (flags6 & 0x1) == 0x1 ? mirroring::vertical : mirroring::horizontal;
    m_battery_present = (flags6 & 0x2) == 0x2;
    m_mapper = (flags6) >> 4;

    auto flags7 = m_rom[7];
    m_mapper |= flags7 & 0xF0;

    m_region = static_cast<region>(m_rom[12] & 0x3);

    //
    // Page setup
    //

    std::ptrdiff_t pos = 16;
    m_prg_rom = std::vector<u8>(PRG_PAGE_SIZE * m_prg_rom_size);
    std::memcpy(m_prg_rom.data(), m_rom.data() + pos, m_prg_rom.size());
    pos += m_prg_rom.size();

    if (m_chr_rom_size > 0)
    {
        m_chr_rom = std::vector<u8>(CHR_PAGE_SIZE * m_chr_rom_size);
        std::memcpy(m_chr_rom.data(), m_rom.data() + pos, m_chr_rom.size());
    }

    m_state = state::loaded;
    return true;
}

} // namespace cx::nes