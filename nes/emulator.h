#pragma once

#include "core/emulator_base.h"

#include "apu.h"
#include "cart.h"
#include "cpu.h"
#include "cpu_bus.h"
#include "mapper.h"
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

    static emulator load_rom(std::string_view path);

  private:
    cart m_cart;
    ppu m_ppu;
    apu m_apu;
    std::unique_ptr<mapper> m_mapper;

    cpu_bus m_cpu_bus;
    cpu m_cpu;

    u64 m_clock{ 0 };
};

} // namespace cx::nes
