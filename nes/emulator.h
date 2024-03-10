#pragma once

#include "core/emulator_base.h"

#include "apu.h"
#include "cart.h"
#include "cpu.h"
#include "cpu_bus.h"
#include "joypad.h"
#include "mapper.h"
#include "mirroring.h"
#include "ppu.h"

namespace cx::nes
{

class emulator : public cx::emulator_base
{
  public:
    emulator(cart&& cart);
    auto clock() -> u8 override;
    void reset() override;
    void shudown() override;

    void set_mirror(mirroring);

    static emulator load_rom(std::string_view path);

    cart m_cart;
    std::unique_ptr<mapper> m_mapper;
    ppu m_ppu;
    apu m_apu;
    cpu m_cpu;
    joypad m_jp1{};
    joypad m_jp2{};

  private:
    u64 m_clock{ 0 };
};

} // namespace cx::nes
