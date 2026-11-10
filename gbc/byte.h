#pragma once

#include "core/types.h"
#include <utility>

namespace cx::gbc::byte
{

constexpr auto bit(u8 a, u8 b) -> bool
{
    return (a & b) == b;
}

constexpr auto get(u8 value, int index) -> bool
{
    return (value & (1 << index)) != 0;
}

constexpr auto get(u16 value, int index) -> bool
{
    return (value & (1 << index)) != 0;
}

constexpr auto from(bool b, int shift) -> u8
{
    return static_cast<u8>((b ? 1 : 0) << shift);
}

constexpr auto upper(u16 a) -> u8
{
    return static_cast<u8>(a >> 8);
}

constexpr auto lower(u16 a) -> u8
{
    return static_cast<u8>(a & 0xFF);
}

constexpr auto combine(u8 msb, u8 lsb) -> u16
{
    return static_cast<u16>(msb << 8 | lsb);
}

constexpr auto combine_r(u8 lsb, u8 msb) -> u16
{
    return static_cast<u16>(msb << 8 | lsb);
}

constexpr auto swap(u8 a) -> u8
{
    return static_cast<u8>(((a >> 4) & 0x0F) | (a << 4));
}

constexpr auto ror(u8 a, bool carry_in) -> std::pair<u8, bool>
{
    const auto carry_out{ (a & 0x1) == 0x1 };
    auto res{ static_cast<u8>(a >> 1) };

    if (carry_in)
        res |= 0x80;

    return { res, carry_out };
}

constexpr auto rol(u8 a, bool carry_in) -> std::pair<u8, bool>
{
    const auto carry_out{ (a & 0x80) == 0x80 };
    auto res{ static_cast<u8>(a << 1) };

    if (carry_in)
        res |= 0x01;

    return { res, carry_out };
}

} // namespace cx::gbc::byte
