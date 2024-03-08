#pragma once

#include "core/emulator_base.h"

#include "apu.h"
#include "cart.h"
#include "cpu.h"
#include "cpu_bus.h"
#include "mapper.h"
#include "mirroring.h"
#include "ppu.h"

namespace cx::nes
{

class emulator : cx::emulator_base
{
  public:
    emulator(cart&& cart);
    void clock() override;
    void reset() override;
    void shudown() override;

    void set_mirror(mirroring);

    static emulator load_rom(std::string_view path);

    cart m_cart;
    std::unique_ptr<mapper> m_mapper;
    ppu m_ppu;
    apu m_apu;
    cpu m_cpu;

  private:
    u64 m_clock{ 0 };
};

} // namespace cx::nes
