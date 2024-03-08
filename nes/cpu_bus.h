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

    u8 read(u16 addr);
    void write(u16 addr, u8 value);

    u16 read_word(u16 addr);
    u16 read_word_wrapped(u16 addr);

  private:
    u8 m_ram[0x800];
    emulator* m_system;
};
} // namespace cx::nes