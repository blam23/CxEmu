#pragma once

#include "core/types.h"
#include "forward.h"
#include "reflection.h"

namespace cx::gbc
{

class dma
{
  public:
    dma(emulator* system);

    auto transfer_active() const -> bool
    {
        return m_active;
    }

    [[= io_write{ 0xFF46 }]] auto start(u8 value) -> void;

    auto clock() -> void;

  private:
    emulator* m_system;

    bool m_active{ false };
    u8 m_offset{ 0 };
    u8 m_value{ 0 };
    u8 m_delay{ 0 };
};

} // namespace cx::gbc
