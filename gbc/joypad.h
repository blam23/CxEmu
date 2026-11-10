#pragma once

#include "core/types.h"
#include "forward.h"
#include "reflection.h"

namespace cx::gbc
{

class joypad
{
  public:
    joypad(emulator* system);

    enum class key : u8
    {
        right = 0b0000'0001,
        left = 0b0000'0010,
        up = 0b0000'0100,
        down = 0b0000'1000,
        a = 0b0001'0000,
        b = 0b0010'0000,
        select = 0b0100'0000,
        start = 0b1000'0000,
    };

    auto press(key k) -> void;
    auto release(key k) -> void;

    [[= io_read{ 0xFF00 }]] auto get_joyp() const -> u8;
    [[= io_write{ 0xFF00 }]] auto set_joyp(u8 value) -> void;

  private:
    emulator* m_system;

    u8 m_state{ 0xFF };
    u8 m_select{ 0x00 };

    static constexpr u8 JOYP_ALWAYS_SET_MASK{ 0xC0 };
};

} // namespace cx::gbc
