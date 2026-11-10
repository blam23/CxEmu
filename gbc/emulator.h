#pragma once

#include "core/emulator_base.h"

#include "cart.h"
#include "cpu.h"
#include "dma.h"
#include "joypad.h"
#include "mapper.h"
#include "memory_bus.h"
#include "ppu.h"
#include "timer.h"

#include <memory>
#include <string_view>

namespace cx::gbc
{

class emulator : public cx::emulator_base
{
  public:
    emulator(cart&& cart);

    // The components point back at the emulator, so it can't be moved or copied
    emulator(const emulator&) = delete;
    emulator(emulator&&) = delete;

    auto clock() -> u8 override;
    auto reset() -> void override;
    auto shudown() -> void override;

    // Throws if the cart can't be opened or parsed
    static auto load_rom(std::string_view path) -> emulator;

    static constexpr int SCREEN_WIDTH{ 160 };
    static constexpr int SCREEN_HEIGHT{ 144 };

    bool m_limit_frames{ false };

    cart m_cart;
    std::unique_ptr<mapper> m_mapper;
    memory_bus m_bus;
    ppu m_ppu;
    dma m_dma;
    timer m_timer;
    cpu m_cpu;
    joypad m_jp1;
};

} // namespace cx::gbc
