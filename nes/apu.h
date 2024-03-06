#pragma once

#include "core/types.h"

namespace cx::nes
{
class apu
{
  public:
    void clock();
    u8 read(u16 addr);
    void write(u16 addr, u8 value);
};
} // namespace cx::nes