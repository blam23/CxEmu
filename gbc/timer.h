#pragma once

#include "core/types.h"
#include "forward.h"
#include "reflection.h"

namespace cx::gbc
{

class timer
{
  public:
    timer(emulator* system);

    auto clock() -> void;

    // DIV
    [[= io_read{ 0xFF04 }]] auto get_div() const -> u8;
    [[= io_write{ 0xFF04 }]] auto set_div(u8 value) -> void;

    // Timer
    [[= io{ 0xFF05 }]] u8 m_tima{ 0 }; // Counter
    [[= io{ 0xFF06 }]] u8 m_tma{ 0 };  // Modulo

    [[= io_read{ 0xFF07 }]] auto get_tac() const -> u8;
    [[= io_write{ 0xFF07 }]] auto set_tac(u8 value) -> void;

  private:
    emulator* m_system;

    // Wrapping, so unsigned (the C# int overflowed). Every divisor is a power of two, so the results are identical.
    u32 m_clock{ 0 };

    static constexpr u32 DIV_CLOCKS{ 256 };
    u8 m_div{ 0xAC };

    bool m_timer_enable{ false };
    u32 m_timer_clocks{ 0x400 };
    u8 m_tct{ 0x00 };

    static constexpr u8 TAC_ALWAYS_SET_MASK{ 0xF8 };
};

} // namespace cx::gbc
