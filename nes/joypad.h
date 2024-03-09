#pragma once

#include "core/types.h"

namespace cx::nes
{

class joypad
{
  public:
    auto read() -> u8;
    auto poll(u8 value) -> void;

    // Status
    struct status_struct
    {
        bool a : 1;
        bool b : 1;
        bool select : 1;
        bool start : 1;
        bool up : 1;
        bool down : 1;
        bool left : 1;
        bool right : 1;
    };

    union status_union {
        status_struct flags;
        u8 bits;
    };

    status_union m_status{ 0 };

  private:
    u8 m_current_state_read_bit{ 0 };
};

} // namespace cx::nes