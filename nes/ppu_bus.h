#pragma once

#include "cart.h"
#include "core/types.h"
#include "forward.h"
#include "mapper.h"

namespace cx::nes
{

class ppu_bus
{
  public:
    ppu_bus(emulator* system);

    u8 read(u16 addr);
    void write(u16 addr, u8 value);

    u16 read_word(u16 addr);
    u16 read_word_wrapped(u16 addr);
    void update_mirroring(mirroring);

  private:
    u8 m_chr_ram[0x2000];

    u8 m_name_tables[4][0x400];
    u8 m_name_table_map[4];
    u8 m_palette[0x20];

    auto get_palette_address(u16 addr) -> u16;

    emulator* m_system;
};

} // namespace cx::nes