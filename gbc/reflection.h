#pragma once

#include "core/types.h"

// Annotation types for C++26 reflection. These replace AxEmu's [Instruction], [IO] and [MBC] C# attributes.
// Annotations are read at compile time (std::meta::annotations_of_with_type), so a duplicate registration
// is a compile error rather than the runtime InvalidDataException of the C# version.

namespace cx::gbc
{

// Operand kinds (CPU.Data)
enum class data
{
    none,
    a,
    b,
    c,
    d,
    e,
    h,
    l,
    af,
    bc,
    de,
    hl,
    sp,
    imm,     // Byte after instruction
    ind_hl,  // Indirect using HL reg
    inc_hl,  // Indirect using HL reg (and increment HL reg)
    dec_hl,  // Indirect using HL reg (and decrement HL reg)
    ind_bc,  // Indirect using BC reg
    ind_de,  // Indirect using DE reg
    ind_c,   // Indirect using ((0xFF00) | C reg)
    ind_imm, // Indirect using ((0xFF00) | Imm)
    ind_abs, // Indirect using (Abs)
    abs,     // Word after instruction

    // Specific bit targets, i.e. for SETing or RESing bits
    bit0,
    bit1,
    bit2,
    bit3,
    bit4,
    bit5,
    bit6,
    bit7,

    sp_plus_imm, // Only used by instr 0xF8
};

enum class condition
{
    none,
    z,
    nz,
    c,
    nc,
    always
};

// [Instruction]: on a cpu::ops function, one annotation per opcode row (stackable).
// Members are in this order so designated initialisers read the same as the C# attribute.
struct instruction_attr
{
    u8 op_code{ 0 };
    int cycles{ 0 };
    data input{ data::none };
    data output{ data::none };
    condition cond{ condition::none };
    bool prefix{ false };
};

// [IO(Address = x)]: on a data member, readable and writable at x.
struct io
{
    u16 address;
};

// [IO(Address = x, Type = IOType.Read)]: on a member function `auto f() -> u8`.
struct io_read
{
    u16 address;
};

// [IO(Address = x, Type = IOType.Write)]: on a member function `auto f(u8) -> void`.
struct io_write
{
    u16 address;
};

// [MBC(CartType = x)]: on a mapper class (stackable).
struct mbc
{
    u8 cart_type;
};

} // namespace cx::gbc
