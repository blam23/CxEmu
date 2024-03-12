#pragma once

#include "apu.h"
#include "core/types.h"
#include "mapper.h"
#include "ppu.h"

namespace cx::nes
{
class cpu_bus
{
  public:
    cpu_bus(emulator*);

    auto read(u16 addr) -> u8;
    auto write(u16 addr, u8 value) -> void;

    auto read_word(u16 addr) -> u16;
    auto read_word_wrapped(u16 addr) -> u16;

  private:
    u8 m_ram[0x800]{ 0 };
    emulator* m_system;
};
} // namespace cx::nes