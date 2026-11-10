#include "ppu.h"
#include <array>

namespace cx::gbc
{

namespace
{

// System.Drawing colours
constexpr colour WHITE{ 255, 255, 255 };
constexpr colour LIGHT_GRAY{ 211, 211, 211 };
constexpr colour DARK_GRAY{ 169, 169, 169 };
constexpr colour BLACK{ 0, 0, 0 };
constexpr colour DODGER_BLUE{ 30, 144, 255 };

constexpr std::array<colour, 4> BLACK_AND_WHITE{ WHITE, LIGHT_GRAY, DARK_GRAY, BLACK };

} // namespace

auto ppu::rgb_lookup(u8 c) const -> colour
{
    switch (c)
    {
    case 0:
        return BLACK_AND_WHITE[0];
    case 1:
        return BLACK_AND_WHITE[1];
    case 2:
        return BLACK_AND_WHITE[2];
    case 3:
        return BLACK_AND_WHITE[3];

    default:
        return DODGER_BLUE;
    }
}

} // namespace cx::gbc
