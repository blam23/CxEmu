#include "ppu.h"

namespace cx::nes
{

auto ppu::lookup_colour(u8 c) -> colour
{
    // https://www.nesdev.org/w/images/default/5/59/Savtool-swatches.png
    switch (c)
    {
    case 0x00:
        return { 101, 101, 101 };
    case 0x01:
        return { 0, 45, 105 };
    case 0x02:
        return { 19, 31, 127 };
    case 0x03:
        return { 60, 19, 124 };
    case 0x04:
        return { 96, 11, 98 };
    case 0x05:
        return { 115, 10, 55 };
    case 0x06:
        return { 113, 15, 7 };
    case 0x07:
        return { 90, 26, 0 };
    case 0x08:
        return { 52, 40, 0 };
    case 0x09:
        return { 11, 52, 0 };
    case 0x0A:
        return { 0, 60, 0 };
    case 0x0B:
        return { 0, 61, 16 };
    case 0x0C:
        return { 0, 56, 64 };
    case 0x0D:
        return { 0, 0, 0 };
    case 0x0E:
        return { 0, 0, 0 };
    case 0x0F:
        return { 0, 0, 0 };
    case 0x10:
        return { 174, 174, 174 };
    case 0x11:
        return { 15, 99, 179 };
    case 0x12:
        return { 64, 81, 208 };
    case 0x13:
        return { 120, 65, 204 };
    case 0x14:
        return { 167, 54, 169 };
    case 0x15:
        return { 192, 52, 112 };
    case 0x16:
        return { 189, 60, 48 };
    case 0x17:
        return { 159, 74, 0 };
    case 0x18:
        return { 109, 92, 0 };
    case 0x19:
        return { 54, 109, 0 };
    case 0x1A:
        return { 7, 119, 4 };
    case 0x1B:
        return { 0, 121, 61 };
    case 0x1C:
        return { 0, 114, 125 };
    case 0x1D:
        return { 78, 78, 78 };
    case 0x1E:
        return { 0, 0, 0 };
    case 0x1F:
        return { 0, 0, 0 };
    case 0x20:
        return { 254, 254, 255 };
    case 0x21:
        return { 93, 179, 255 };
    case 0x22:
        return { 143, 161, 255 };
    case 0x23:
        return { 200, 144, 255 };
    case 0x24:
        return { 247, 133, 250 };
    case 0x25:
        return { 255, 131, 192 };
    case 0x26:
        return { 255, 139, 127 };
    case 0x27:
        return { 239, 154, 73 };
    case 0x28:
        return { 189, 172, 44 };
    case 0x29:
        return { 133, 188, 47 };
    case 0x2A:
        return { 85, 199, 83 };
    case 0x2B:
        return { 60, 201, 140 };
    case 0x2C:
        return { 62, 194, 205 };
    case 0x2D:
        return { 78, 78, 78 };
    case 0x2E:
        return { 0, 0, 0 };
    case 0x2F:
        return { 0, 0, 0 };
    case 0x30:
        return { 254, 254, 255 };
    case 0x31:
        return { 188, 223, 255 };
    case 0x32:
        return { 42, 42, 42 };
    case 0x33:
        return { 232, 209, 255 };
    case 0x34:
        return { 251, 205, 253 };
    case 0x35:
        return { 255, 204, 229 };
    case 0x36:
        return { 251, 205, 253 };
    case 0x37:
        return { 84, 67, 75 };
    case 0x38:
        return { 255, 204, 229 };
    case 0x39:
        return { 122, 118, 90 };
    case 0x3A:
        return { 185, 232, 184 };
    case 0x3B:
        return { 174, 232, 208 };
    case 0x3C:
        return { 175, 229, 234 };
    case 0x3D:
        return { 133, 188, 47 };
    case 0x3E:
        return { 0, 0, 0 };
    case 0x3F:
        return { 0, 0, 0 };
    default:
        return { 101, 101, 101 };
    }
}

} // namespace cx::nes