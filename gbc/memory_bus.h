#pragma once

#include "core/types.h"
#include "forward.h"
#include <array>

namespace cx::gbc
{

class memory_bus
{
  public:
    memory_bus(emulator* system);

    auto write(u16 addr, u8 data) -> void;
    auto read(u16 addr) -> u8;

    auto read_word(u16 addr) -> u16;
    auto write_word(u16 addr, u16 val) -> void;

    auto shutdown() -> void;

    // Memory chips
    std::array<u8, 0x2000> m_vram{}; // 8KB Video RAM
    std::array<u8, 0x2000> m_wram{}; // 8KB Work  RAM
    std::array<u8, 0x007F> m_hram{}; // High Ram
    std::array<u8, 0x00A0> m_oam{};  // Object Attribute Memory

  private:
    emulator* m_system;
};

} // namespace cx::gbc
