#pragma once

#include "../mapper.h"
#include "../reflection.h"

namespace cx::gbc::mappers
{

// clang-format off
class [[= mbc{ 0x00 }]] no_mbc : public mapper
// clang-format on
{
  public:
    no_mbc(emulator* system);

    auto read(u16 addr) -> u8 override;
    auto write(u16 addr, u8 value) -> void override;

  private:
    cart& m_cart;
};

} // namespace cx::gbc::mappers
