#include "cpu.h"
#include "reflection.h"
#include <array>
#include <meta>

namespace cx::gbc
{

// All instruction annotations live here (generated from AxEmu's CPU.Instructions.cs). Each wrapper is a member of the
// nested cpu::ops so it can reach cpu's private methods; the tables below are built from the annotations at compile
// time.
// clang-format off
struct cpu::ops
{
    [[= instruction_attr{ .op_code = 0x00, .cycles = 4 }]]
    [[= instruction_attr{ .op_code = 0xDD, .cycles = 4 }]]
    static auto nop(cpu* c) -> void { c->nop(); }

    [[= instruction_attr{ .op_code = 0x76, .cycles = 4 }]]
    static auto halt(cpu* c) -> void { c->halt(); }

    [[= instruction_attr{ .op_code = 0x10, .cycles = 4 }]]
    static auto stop(cpu* c) -> void { c->stop(); }

    [[= instruction_attr{ .op_code = 0xF3, .cycles = 4 }]]
    static auto di(cpu* c) -> void { c->di(); }

    [[= instruction_attr{ .op_code = 0xFB, .cycles = 4 }]]
    static auto ei(cpu* c) -> void { c->ei(); }

    [[= instruction_attr{ .op_code = 0x08, .cycles = 20, .input = data::sp, .output = data::ind_abs }]]
    [[= instruction_attr{ .op_code = 0x01, .cycles = 12, .input = data::abs, .output = data::bc }]]
    [[= instruction_attr{ .op_code = 0x11, .cycles = 12, .input = data::abs, .output = data::de }]]
    [[= instruction_attr{ .op_code = 0x21, .cycles = 12, .input = data::abs, .output = data::hl }]]
    [[= instruction_attr{ .op_code = 0x31, .cycles = 12, .input = data::abs, .output = data::sp }]]
    [[= instruction_attr{ .op_code = 0x02, .cycles = 8, .input = data::a, .output = data::ind_bc }]]
    [[= instruction_attr{ .op_code = 0x12, .cycles = 8, .input = data::a, .output = data::ind_de }]]
    [[= instruction_attr{ .op_code = 0x22, .cycles = 8, .input = data::a, .output = data::inc_hl }]]
    [[= instruction_attr{ .op_code = 0x32, .cycles = 8, .input = data::a, .output = data::dec_hl }]]
    [[= instruction_attr{ .op_code = 0x06, .cycles = 8, .input = data::imm, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x16, .cycles = 8, .input = data::imm, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x26, .cycles = 8, .input = data::imm, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x36, .cycles = 12, .input = data::imm, .output = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x0A, .cycles = 8, .input = data::ind_bc, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x1A, .cycles = 8, .input = data::ind_de, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x2A, .cycles = 8, .input = data::inc_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x3A, .cycles = 8, .input = data::dec_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x0E, .cycles = 8, .input = data::imm, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x1E, .cycles = 8, .input = data::imm, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x2E, .cycles = 8, .input = data::imm, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x3E, .cycles = 8, .input = data::imm, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x40, .cycles = 4, .input = data::b, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x50, .cycles = 4, .input = data::b, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x60, .cycles = 4, .input = data::b, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x70, .cycles = 8, .input = data::b, .output = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x41, .cycles = 4, .input = data::c, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x51, .cycles = 4, .input = data::c, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x61, .cycles = 4, .input = data::c, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x71, .cycles = 8, .input = data::c, .output = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x42, .cycles = 4, .input = data::d, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x52, .cycles = 4, .input = data::d, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x62, .cycles = 4, .input = data::d, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x72, .cycles = 8, .input = data::d, .output = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x43, .cycles = 4, .input = data::e, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x53, .cycles = 4, .input = data::e, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x63, .cycles = 4, .input = data::e, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x73, .cycles = 8, .input = data::e, .output = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x44, .cycles = 4, .input = data::h, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x54, .cycles = 4, .input = data::h, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x64, .cycles = 4, .input = data::h, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x74, .cycles = 8, .input = data::h, .output = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x45, .cycles = 4, .input = data::l, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x55, .cycles = 4, .input = data::l, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x65, .cycles = 4, .input = data::l, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x75, .cycles = 8, .input = data::l, .output = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x46, .cycles = 8, .input = data::ind_hl, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x56, .cycles = 8, .input = data::ind_hl, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x66, .cycles = 8, .input = data::ind_hl, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x47, .cycles = 4, .input = data::a, .output = data::b }]]
    [[= instruction_attr{ .op_code = 0x57, .cycles = 4, .input = data::a, .output = data::d }]]
    [[= instruction_attr{ .op_code = 0x67, .cycles = 4, .input = data::a, .output = data::h }]]
    [[= instruction_attr{ .op_code = 0x77, .cycles = 8, .input = data::a, .output = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x48, .cycles = 4, .input = data::b, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x58, .cycles = 4, .input = data::b, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x68, .cycles = 4, .input = data::b, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x78, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x49, .cycles = 4, .input = data::c, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x59, .cycles = 4, .input = data::c, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x69, .cycles = 4, .input = data::c, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x79, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x4A, .cycles = 4, .input = data::d, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x5A, .cycles = 4, .input = data::d, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x6A, .cycles = 4, .input = data::d, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x7A, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x4B, .cycles = 4, .input = data::e, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x5B, .cycles = 4, .input = data::e, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x6B, .cycles = 4, .input = data::e, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x7B, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x4C, .cycles = 4, .input = data::h, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x5C, .cycles = 4, .input = data::h, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x6C, .cycles = 4, .input = data::h, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x7C, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x4D, .cycles = 4, .input = data::l, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x5D, .cycles = 4, .input = data::l, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x6D, .cycles = 4, .input = data::l, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x7D, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x4E, .cycles = 8, .input = data::ind_hl, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x5E, .cycles = 8, .input = data::ind_hl, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x6E, .cycles = 8, .input = data::ind_hl, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x7E, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x4F, .cycles = 4, .input = data::a, .output = data::c }]]
    [[= instruction_attr{ .op_code = 0x5F, .cycles = 4, .input = data::a, .output = data::e }]]
    [[= instruction_attr{ .op_code = 0x6F, .cycles = 4, .input = data::a, .output = data::l }]]
    [[= instruction_attr{ .op_code = 0x7F, .cycles = 4, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xE0, .cycles = 12, .input = data::a, .output = data::ind_imm }]]
    [[= instruction_attr{ .op_code = 0xF0, .cycles = 12, .input = data::ind_imm, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xEA, .cycles = 16, .input = data::a, .output = data::ind_abs }]]
    [[= instruction_attr{ .op_code = 0xFA, .cycles = 16, .input = data::ind_abs, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xE2, .cycles = 8, .input = data::a, .output = data::ind_c }]]
    [[= instruction_attr{ .op_code = 0xF2, .cycles = 8, .input = data::ind_c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xF9, .cycles = 8, .input = data::hl, .output = data::sp }]]
    [[= instruction_attr{ .op_code = 0xF8, .cycles = 16, .input = data::sp_plus_imm, .output = data::hl }]]
    static auto ld(cpu* c) -> void { c->ld(); }

    [[= instruction_attr{ .op_code = 0xE9, .cycles = 12, .input = data::hl }]]
    [[= instruction_attr{ .op_code = 0xC3, .cycles = 12, .input = data::abs }]]
    static auto jmp(cpu* c) -> void { c->jmp(); }

    [[= instruction_attr{ .op_code = 0xC9, .cycles = 12, .cond = condition::always }]]
    [[= instruction_attr{ .op_code = 0xC0, .cycles = 12, .cond = condition::nz }]]
    [[= instruction_attr{ .op_code = 0xD0, .cycles = 12, .cond = condition::nc }]]
    [[= instruction_attr{ .op_code = 0xC8, .cycles = 12, .cond = condition::z }]]
    [[= instruction_attr{ .op_code = 0xD8, .cycles = 12, .cond = condition::c }]]
    static auto ret(cpu* c) -> void { c->ret(); }

    [[= instruction_attr{ .op_code = 0xD9, .cycles = 8, .cond = condition::always }]]
    static auto reti(cpu* c) -> void { c->reti(); }

    [[= instruction_attr{ .op_code = 0xC7, .cycles = 12 }]]
    [[= instruction_attr{ .op_code = 0xCF, .cycles = 12 }]]
    [[= instruction_attr{ .op_code = 0xD7, .cycles = 12 }]]
    [[= instruction_attr{ .op_code = 0xDF, .cycles = 12 }]]
    [[= instruction_attr{ .op_code = 0xE7, .cycles = 12 }]]
    [[= instruction_attr{ .op_code = 0xEF, .cycles = 12 }]]
    [[= instruction_attr{ .op_code = 0xF7, .cycles = 12 }]]
    [[= instruction_attr{ .op_code = 0xFF, .cycles = 12 }]]
    static auto rst(cpu* c) -> void { c->rst(); }

    [[= instruction_attr{ .op_code = 0xCD, .cycles = 20, .input = data::abs, .cond = condition::always }]]
    [[= instruction_attr{ .op_code = 0xCC, .cycles = 8, .input = data::abs, .cond = condition::z }]]
    [[= instruction_attr{ .op_code = 0xDC, .cycles = 8, .input = data::abs, .cond = condition::c }]]
    [[= instruction_attr{ .op_code = 0xC4, .cycles = 8, .input = data::abs, .cond = condition::nz }]]
    [[= instruction_attr{ .op_code = 0xD4, .cycles = 8, .input = data::abs, .cond = condition::nc }]]
    static auto call(cpu* c) -> void { c->call(); }

    [[= instruction_attr{ .op_code = 0xCA, .cycles = 12, .input = data::abs, .cond = condition::z }]]
    [[= instruction_attr{ .op_code = 0xC2, .cycles = 12, .input = data::abs, .cond = condition::nz }]]
    [[= instruction_attr{ .op_code = 0xDA, .cycles = 12, .input = data::abs, .cond = condition::c }]]
    [[= instruction_attr{ .op_code = 0xD2, .cycles = 12, .input = data::abs, .cond = condition::nc }]]
    [[= instruction_attr{ .op_code = 0x28, .cycles = 8, .input = data::imm, .cond = condition::z }]]
    [[= instruction_attr{ .op_code = 0x20, .cycles = 8, .input = data::imm, .cond = condition::nz }]]
    [[= instruction_attr{ .op_code = 0x38, .cycles = 8, .input = data::imm, .cond = condition::c }]]
    [[= instruction_attr{ .op_code = 0x30, .cycles = 8, .input = data::imm, .cond = condition::nc }]]
    [[= instruction_attr{ .op_code = 0x18, .cycles = 8, .input = data::imm, .cond = condition::always }]]
    static auto jr(cpu* c) -> void { c->jr(); }

    [[= instruction_attr{ .op_code = 0x37, .cycles = 4 }]]
    static auto scf(cpu* c) -> void { c->scf(); }

    [[= instruction_attr{ .op_code = 0x3F, .cycles = 4 }]]
    static auto ccf(cpu* c) -> void { c->ccf(); }

    [[= instruction_attr{ .op_code = 0xC1, .cycles = 12, .output = data::bc }]]
    [[= instruction_attr{ .op_code = 0xD1, .cycles = 12, .output = data::de }]]
    [[= instruction_attr{ .op_code = 0xE1, .cycles = 12, .output = data::hl }]]
    [[= instruction_attr{ .op_code = 0xF1, .cycles = 12, .output = data::af }]]
    static auto pop(cpu* c) -> void { c->pop(); }

    [[= instruction_attr{ .op_code = 0xC5, .cycles = 16, .input = data::bc }]]
    [[= instruction_attr{ .op_code = 0xD5, .cycles = 16, .input = data::de }]]
    [[= instruction_attr{ .op_code = 0xE5, .cycles = 16, .input = data::hl }]]
    [[= instruction_attr{ .op_code = 0xF5, .cycles = 16, .input = data::af }]]
    static auto push(cpu* c) -> void { c->push(); }

    [[= instruction_attr{ .op_code = 0x27, .cycles = 4, .output = data::a }]]
    static auto daa(cpu* c) -> void { c->daa(); }

    [[= instruction_attr{ .op_code = 0x09, .cycles = 8, .input = data::bc, .output = data::hl }]]
    [[= instruction_attr{ .op_code = 0x19, .cycles = 8, .input = data::de, .output = data::hl }]]
    [[= instruction_attr{ .op_code = 0x29, .cycles = 8, .input = data::hl, .output = data::hl }]]
    [[= instruction_attr{ .op_code = 0x39, .cycles = 8, .input = data::sp, .output = data::hl }]]
    static auto add_16(cpu* c) -> void { c->add_16(); }

    [[= instruction_attr{ .op_code = 0xE8, .cycles = 16, .input = data::imm, .output = data::sp }]]
    static auto add_sp_imm(cpu* c) -> void { c->add_sp_imm(); }

    [[= instruction_attr{ .op_code = 0x80, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x81, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x82, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x83, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x84, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x85, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x86, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x87, .cycles = 4, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xC6, .cycles = 8, .input = data::imm, .output = data::a }]]
    static auto add(cpu* c) -> void { c->add(); }

    [[= instruction_attr{ .op_code = 0x88, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x89, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x8A, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x8B, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x8C, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x8D, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x8E, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x8F, .cycles = 4, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xCE, .cycles = 8, .input = data::imm, .output = data::a }]]
    static auto adc(cpu* c) -> void { c->adc(); }

    [[= instruction_attr{ .op_code = 0x90, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x91, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x92, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x93, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x94, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x95, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x96, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x97, .cycles = 4, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xD6, .cycles = 8, .input = data::imm, .output = data::a }]]
    static auto sub(cpu* c) -> void { c->sub(); }

    [[= instruction_attr{ .op_code = 0x98, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x99, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x9A, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x9B, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x9C, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x9D, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x9E, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0x9F, .cycles = 4, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xDE, .cycles = 8, .input = data::imm, .output = data::a }]]
    static auto sbc(cpu* c) -> void { c->sbc(); }

    [[= instruction_attr{ .op_code = 0xB8, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xB9, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xBA, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xBB, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xBC, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xBD, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xBE, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xBF, .cycles = 4, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xFE, .cycles = 8, .input = data::imm, .output = data::a }]]
    static auto cp(cpu* c) -> void { c->cp(); }

    [[= instruction_attr{ .op_code = 0xA8, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xA9, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xAA, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xAB, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xAC, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xAD, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xAE, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xAF, .cycles = 4, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xEE, .cycles = 8, .input = data::imm, .output = data::a }]]
    static auto _xor(cpu* c) -> void { c->_xor(); }

    [[= instruction_attr{ .op_code = 0xB0, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xB1, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xB2, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xB3, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xB4, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xB5, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xB6, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xB7, .cycles = 8, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xF6, .cycles = 8, .input = data::imm, .output = data::a }]]
    static auto _or(cpu* c) -> void { c->_or(); }

    [[= instruction_attr{ .op_code = 0xA0, .cycles = 4, .input = data::b, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xA1, .cycles = 4, .input = data::c, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xA2, .cycles = 4, .input = data::d, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xA3, .cycles = 4, .input = data::e, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xA4, .cycles = 4, .input = data::h, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xA5, .cycles = 4, .input = data::l, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xA6, .cycles = 8, .input = data::ind_hl, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xA7, .cycles = 8, .input = data::a, .output = data::a }]]
    [[= instruction_attr{ .op_code = 0xE6, .cycles = 8, .input = data::imm, .output = data::a }]]
    static auto _and(cpu* c) -> void { c->_and(); }

    [[= instruction_attr{ .op_code = 0x05, .cycles = 4, .input = data::b }]]
    [[= instruction_attr{ .op_code = 0x15, .cycles = 4, .input = data::d }]]
    [[= instruction_attr{ .op_code = 0x25, .cycles = 4, .input = data::h }]]
    [[= instruction_attr{ .op_code = 0x35, .cycles = 12, .input = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x0D, .cycles = 4, .input = data::c }]]
    [[= instruction_attr{ .op_code = 0x1D, .cycles = 4, .input = data::e }]]
    [[= instruction_attr{ .op_code = 0x2D, .cycles = 4, .input = data::l }]]
    [[= instruction_attr{ .op_code = 0x3D, .cycles = 4, .input = data::a }]]
    [[= instruction_attr{ .op_code = 0x0B, .cycles = 8, .input = data::bc }]]
    [[= instruction_attr{ .op_code = 0x1B, .cycles = 8, .input = data::de }]]
    [[= instruction_attr{ .op_code = 0x2B, .cycles = 8, .input = data::hl }]]
    [[= instruction_attr{ .op_code = 0x3B, .cycles = 8, .input = data::sp }]]
    static auto dec(cpu* c) -> void { c->dec(); }

    [[= instruction_attr{ .op_code = 0x04, .cycles = 4, .input = data::b }]]
    [[= instruction_attr{ .op_code = 0x14, .cycles = 4, .input = data::d }]]
    [[= instruction_attr{ .op_code = 0x24, .cycles = 4, .input = data::h }]]
    [[= instruction_attr{ .op_code = 0x34, .cycles = 12, .input = data::ind_hl }]]
    [[= instruction_attr{ .op_code = 0x0C, .cycles = 4, .input = data::c }]]
    [[= instruction_attr{ .op_code = 0x1C, .cycles = 4, .input = data::e }]]
    [[= instruction_attr{ .op_code = 0x2C, .cycles = 4, .input = data::l }]]
    [[= instruction_attr{ .op_code = 0x3C, .cycles = 4, .input = data::a }]]
    [[= instruction_attr{ .op_code = 0x03, .cycles = 8, .input = data::bc }]]
    [[= instruction_attr{ .op_code = 0x13, .cycles = 8, .input = data::de }]]
    [[= instruction_attr{ .op_code = 0x23, .cycles = 8, .input = data::hl }]]
    [[= instruction_attr{ .op_code = 0x33, .cycles = 8, .input = data::sp }]]
    static auto inc(cpu* c) -> void { c->inc(); }

    [[= instruction_attr{ .op_code = 0x08, .cycles = 4, .input = data::b, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x09, .cycles = 4, .input = data::c, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x0A, .cycles = 4, .input = data::d, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x0B, .cycles = 4, .input = data::e, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x0C, .cycles = 4, .input = data::h, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x0D, .cycles = 4, .input = data::l, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x0E, .cycles = 12, .input = data::ind_hl, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x0F, .cycles = 4, .input = data::a, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x0F, .cycles = 4, .input = data::a, .output = data::a }]]
    static auto rrc(cpu* c) -> void { c->rrc(); }

    [[= instruction_attr{ .op_code = 0x18, .cycles = 4, .input = data::b, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x19, .cycles = 4, .input = data::c, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x1A, .cycles = 4, .input = data::d, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x1B, .cycles = 4, .input = data::e, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x1C, .cycles = 4, .input = data::h, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x1D, .cycles = 4, .input = data::l, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x1E, .cycles = 12, .input = data::ind_hl, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x1F, .cycles = 4, .input = data::a, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x1F, .cycles = 4, .input = data::a, .output = data::a }]]
    static auto rr(cpu* c) -> void { c->rr(); }

    [[= instruction_attr{ .op_code = 0x00, .cycles = 4, .input = data::b, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x01, .cycles = 4, .input = data::c, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x02, .cycles = 4, .input = data::d, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x03, .cycles = 4, .input = data::e, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x04, .cycles = 4, .input = data::h, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x05, .cycles = 4, .input = data::l, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x06, .cycles = 12, .input = data::ind_hl, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x07, .cycles = 4, .input = data::a, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x07, .cycles = 4, .input = data::a, .output = data::a }]]
    static auto rlc(cpu* c) -> void { c->rlc(); }

    [[= instruction_attr{ .op_code = 0x10, .cycles = 4, .input = data::b, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x11, .cycles = 4, .input = data::c, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x12, .cycles = 4, .input = data::d, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x13, .cycles = 4, .input = data::e, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x14, .cycles = 4, .input = data::h, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x15, .cycles = 4, .input = data::l, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x16, .cycles = 12, .input = data::ind_hl, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x17, .cycles = 4, .input = data::a, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x17, .cycles = 4, .input = data::a, .output = data::a }]]
    static auto rl(cpu* c) -> void { c->rl(); }

    [[= instruction_attr{ .op_code = 0x20, .cycles = 8, .input = data::b, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x21, .cycles = 8, .input = data::c, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x22, .cycles = 8, .input = data::d, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x23, .cycles = 8, .input = data::e, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x24, .cycles = 8, .input = data::h, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x25, .cycles = 8, .input = data::l, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x26, .cycles = 16, .input = data::ind_hl, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x27, .cycles = 8, .input = data::a, .output = data::a, .prefix = true }]]
    static auto sla(cpu* c) -> void { c->sla(); }

    [[= instruction_attr{ .op_code = 0x28, .cycles = 8, .input = data::b, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x29, .cycles = 8, .input = data::c, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x2A, .cycles = 8, .input = data::d, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x2B, .cycles = 8, .input = data::e, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x2C, .cycles = 8, .input = data::h, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x2D, .cycles = 8, .input = data::l, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x2E, .cycles = 16, .input = data::ind_hl, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x2F, .cycles = 8, .input = data::a, .output = data::a, .prefix = true }]]
    static auto sra(cpu* c) -> void { c->sra(); }

    [[= instruction_attr{ .op_code = 0x38, .cycles = 8, .input = data::b, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x39, .cycles = 8, .input = data::c, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x3A, .cycles = 8, .input = data::d, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x3B, .cycles = 8, .input = data::e, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x3C, .cycles = 8, .input = data::h, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x3D, .cycles = 8, .input = data::l, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x3E, .cycles = 16, .input = data::ind_hl, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x3F, .cycles = 8, .input = data::a, .output = data::a, .prefix = true }]]
    static auto srl(cpu* c) -> void { c->srl(); }

    [[= instruction_attr{ .op_code = 0x30, .cycles = 4, .input = data::b, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x31, .cycles = 4, .input = data::c, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x32, .cycles = 4, .input = data::d, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x33, .cycles = 4, .input = data::e, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x34, .cycles = 4, .input = data::h, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x35, .cycles = 4, .input = data::l, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x36, .cycles = 12, .input = data::ind_hl, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x37, .cycles = 4, .input = data::a, .output = data::a, .prefix = true }]]
    static auto swap(cpu* c) -> void { c->swap(); }

    [[= instruction_attr{ .op_code = 0x2F, .cycles = 8, .output = data::a }]]
    static auto cpl(cpu* c) -> void { c->cpl(); }

    [[= instruction_attr{ .op_code = 0x40, .cycles = 4, .input = data::bit0, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x48, .cycles = 4, .input = data::bit1, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x50, .cycles = 4, .input = data::bit2, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x58, .cycles = 4, .input = data::bit3, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x60, .cycles = 4, .input = data::bit4, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x68, .cycles = 4, .input = data::bit5, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x70, .cycles = 4, .input = data::bit6, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x78, .cycles = 4, .input = data::bit7, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x41, .cycles = 4, .input = data::bit0, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x49, .cycles = 4, .input = data::bit1, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x51, .cycles = 4, .input = data::bit2, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x59, .cycles = 4, .input = data::bit3, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x61, .cycles = 4, .input = data::bit4, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x69, .cycles = 4, .input = data::bit5, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x71, .cycles = 4, .input = data::bit6, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x79, .cycles = 4, .input = data::bit7, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x42, .cycles = 4, .input = data::bit0, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x4A, .cycles = 4, .input = data::bit1, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x52, .cycles = 4, .input = data::bit2, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x5A, .cycles = 4, .input = data::bit3, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x62, .cycles = 4, .input = data::bit4, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x6A, .cycles = 4, .input = data::bit5, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x72, .cycles = 4, .input = data::bit6, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x7A, .cycles = 4, .input = data::bit7, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x43, .cycles = 4, .input = data::bit0, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x4B, .cycles = 4, .input = data::bit1, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x53, .cycles = 4, .input = data::bit2, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x5B, .cycles = 4, .input = data::bit3, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x63, .cycles = 4, .input = data::bit4, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x6B, .cycles = 4, .input = data::bit5, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x73, .cycles = 4, .input = data::bit6, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x7B, .cycles = 4, .input = data::bit7, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x44, .cycles = 4, .input = data::bit0, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x4C, .cycles = 4, .input = data::bit1, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x54, .cycles = 4, .input = data::bit2, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x5C, .cycles = 4, .input = data::bit3, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x64, .cycles = 4, .input = data::bit4, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x6C, .cycles = 4, .input = data::bit5, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x74, .cycles = 4, .input = data::bit6, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x7C, .cycles = 4, .input = data::bit7, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x45, .cycles = 4, .input = data::bit0, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x4D, .cycles = 4, .input = data::bit1, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x55, .cycles = 4, .input = data::bit2, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x5D, .cycles = 4, .input = data::bit3, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x65, .cycles = 4, .input = data::bit4, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x6D, .cycles = 4, .input = data::bit5, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x75, .cycles = 4, .input = data::bit6, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x7D, .cycles = 4, .input = data::bit7, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x46, .cycles = 12, .input = data::bit0, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x4E, .cycles = 12, .input = data::bit1, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x56, .cycles = 12, .input = data::bit2, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x5E, .cycles = 12, .input = data::bit3, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x66, .cycles = 12, .input = data::bit4, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x6E, .cycles = 12, .input = data::bit5, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x76, .cycles = 12, .input = data::bit6, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x7E, .cycles = 12, .input = data::bit7, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x47, .cycles = 4, .input = data::bit0, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x4F, .cycles = 4, .input = data::bit1, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x57, .cycles = 4, .input = data::bit2, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x5F, .cycles = 4, .input = data::bit3, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x67, .cycles = 4, .input = data::bit4, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x6F, .cycles = 4, .input = data::bit5, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x77, .cycles = 4, .input = data::bit6, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x7F, .cycles = 4, .input = data::bit7, .output = data::a, .prefix = true }]]
    static auto bit(cpu* c) -> void { c->bit(); }

    [[= instruction_attr{ .op_code = 0x80, .cycles = 4, .input = data::bit0, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x88, .cycles = 4, .input = data::bit1, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x90, .cycles = 4, .input = data::bit2, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x98, .cycles = 4, .input = data::bit3, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA0, .cycles = 4, .input = data::bit4, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA8, .cycles = 4, .input = data::bit5, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB0, .cycles = 4, .input = data::bit6, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB8, .cycles = 4, .input = data::bit7, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x81, .cycles = 4, .input = data::bit0, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x89, .cycles = 4, .input = data::bit1, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x91, .cycles = 4, .input = data::bit2, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x99, .cycles = 4, .input = data::bit3, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA1, .cycles = 4, .input = data::bit4, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA9, .cycles = 4, .input = data::bit5, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB1, .cycles = 4, .input = data::bit6, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB9, .cycles = 4, .input = data::bit7, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x82, .cycles = 4, .input = data::bit0, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x8A, .cycles = 4, .input = data::bit1, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x92, .cycles = 4, .input = data::bit2, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x9A, .cycles = 4, .input = data::bit3, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA2, .cycles = 4, .input = data::bit4, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xAA, .cycles = 4, .input = data::bit5, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB2, .cycles = 4, .input = data::bit6, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xBA, .cycles = 4, .input = data::bit7, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x83, .cycles = 4, .input = data::bit0, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x8B, .cycles = 4, .input = data::bit1, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x93, .cycles = 4, .input = data::bit2, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x9B, .cycles = 4, .input = data::bit3, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA3, .cycles = 4, .input = data::bit4, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xAB, .cycles = 4, .input = data::bit5, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB3, .cycles = 4, .input = data::bit6, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xBB, .cycles = 4, .input = data::bit7, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x84, .cycles = 4, .input = data::bit0, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x8C, .cycles = 4, .input = data::bit1, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x94, .cycles = 4, .input = data::bit2, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x9C, .cycles = 4, .input = data::bit3, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA4, .cycles = 4, .input = data::bit4, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xAC, .cycles = 4, .input = data::bit5, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB4, .cycles = 4, .input = data::bit6, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xBC, .cycles = 4, .input = data::bit7, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x85, .cycles = 4, .input = data::bit0, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x8D, .cycles = 4, .input = data::bit1, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x95, .cycles = 4, .input = data::bit2, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x9D, .cycles = 4, .input = data::bit3, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA5, .cycles = 4, .input = data::bit4, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xAD, .cycles = 4, .input = data::bit5, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB5, .cycles = 4, .input = data::bit6, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xBD, .cycles = 4, .input = data::bit7, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x86, .cycles = 12, .input = data::bit0, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x8E, .cycles = 12, .input = data::bit1, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x96, .cycles = 12, .input = data::bit2, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x9E, .cycles = 12, .input = data::bit3, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA6, .cycles = 12, .input = data::bit4, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xAE, .cycles = 12, .input = data::bit5, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB6, .cycles = 12, .input = data::bit6, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xBE, .cycles = 12, .input = data::bit7, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x87, .cycles = 4, .input = data::bit0, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x8F, .cycles = 4, .input = data::bit1, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x97, .cycles = 4, .input = data::bit2, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0x9F, .cycles = 4, .input = data::bit3, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xA7, .cycles = 4, .input = data::bit4, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xAF, .cycles = 4, .input = data::bit5, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xB7, .cycles = 4, .input = data::bit6, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xBF, .cycles = 4, .input = data::bit7, .output = data::a, .prefix = true }]]
    static auto res(cpu* c) -> void { c->res(); }

    [[= instruction_attr{ .op_code = 0xC0, .cycles = 4, .input = data::bit0, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC8, .cycles = 4, .input = data::bit1, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD0, .cycles = 4, .input = data::bit2, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD8, .cycles = 4, .input = data::bit3, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE0, .cycles = 4, .input = data::bit4, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE8, .cycles = 4, .input = data::bit5, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF0, .cycles = 4, .input = data::bit6, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF8, .cycles = 4, .input = data::bit7, .output = data::b, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC1, .cycles = 4, .input = data::bit0, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC9, .cycles = 4, .input = data::bit1, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD1, .cycles = 4, .input = data::bit2, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD9, .cycles = 4, .input = data::bit3, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE1, .cycles = 4, .input = data::bit4, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE9, .cycles = 4, .input = data::bit5, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF1, .cycles = 4, .input = data::bit6, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF9, .cycles = 4, .input = data::bit7, .output = data::c, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC2, .cycles = 4, .input = data::bit0, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xCA, .cycles = 4, .input = data::bit1, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD2, .cycles = 4, .input = data::bit2, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xDA, .cycles = 4, .input = data::bit3, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE2, .cycles = 4, .input = data::bit4, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xEA, .cycles = 4, .input = data::bit5, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF2, .cycles = 4, .input = data::bit6, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xFA, .cycles = 4, .input = data::bit7, .output = data::d, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC3, .cycles = 4, .input = data::bit0, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xCB, .cycles = 4, .input = data::bit1, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD3, .cycles = 4, .input = data::bit2, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xDB, .cycles = 4, .input = data::bit3, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE3, .cycles = 4, .input = data::bit4, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xEB, .cycles = 4, .input = data::bit5, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF3, .cycles = 4, .input = data::bit6, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xFB, .cycles = 4, .input = data::bit7, .output = data::e, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC4, .cycles = 4, .input = data::bit0, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xCC, .cycles = 4, .input = data::bit1, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD4, .cycles = 4, .input = data::bit2, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xDC, .cycles = 4, .input = data::bit3, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE4, .cycles = 4, .input = data::bit4, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xEC, .cycles = 4, .input = data::bit5, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF4, .cycles = 4, .input = data::bit6, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xFC, .cycles = 4, .input = data::bit7, .output = data::h, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC5, .cycles = 4, .input = data::bit0, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xCD, .cycles = 4, .input = data::bit1, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD5, .cycles = 4, .input = data::bit2, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xDD, .cycles = 4, .input = data::bit3, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE5, .cycles = 4, .input = data::bit4, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xED, .cycles = 4, .input = data::bit5, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF5, .cycles = 4, .input = data::bit6, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xFD, .cycles = 4, .input = data::bit7, .output = data::l, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC6, .cycles = 12, .input = data::bit0, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xCE, .cycles = 12, .input = data::bit1, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD6, .cycles = 12, .input = data::bit2, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xDE, .cycles = 12, .input = data::bit3, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE6, .cycles = 12, .input = data::bit4, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xEE, .cycles = 12, .input = data::bit5, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF6, .cycles = 12, .input = data::bit6, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xFE, .cycles = 12, .input = data::bit7, .output = data::ind_hl, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xC7, .cycles = 4, .input = data::bit0, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xCF, .cycles = 4, .input = data::bit1, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xD7, .cycles = 4, .input = data::bit2, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xDF, .cycles = 4, .input = data::bit3, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xE7, .cycles = 4, .input = data::bit4, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xEF, .cycles = 4, .input = data::bit5, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xF7, .cycles = 4, .input = data::bit6, .output = data::a, .prefix = true }]]
    [[= instruction_attr{ .op_code = 0xFF, .cycles = 4, .input = data::bit7, .output = data::a, .prefix = true }]]
    static auto set(cpu* c) -> void { c->set(); }
};
// clang-format on

namespace
{

consteval auto data_len(data d) -> u8
{
    switch (d)
    {
    case data::imm:
    case data::ind_imm:
    case data::sp_plus_imm:
        return 1;
    case data::abs:
    case data::ind_abs:
        return 2;
    default:
        return 0;
    }
}

consteval auto build_table(bool prefixed) -> std::array<instruction, 0x100>
{
    std::array<instruction, 0x100> table{};

    template for (constexpr auto fn :
                  std::define_static_array(std::meta::members_of(^^cpu::ops, std::meta::access_context::unchecked())))
    {
        template for (constexpr auto annotation :
                      std::define_static_array(std::meta::annotations_of_with_type(fn, ^^instruction_attr)))
        {
            constexpr auto attr{ std::meta::extract<instruction_attr>(annotation) };
            if (attr.prefix == prefixed)
            {
                // A duplicate opcode is a compile error
                if (table[attr.op_code].action != nullptr)
                    throw "duplicate opcode registered";

                table[attr.op_code] = instruction{
                    .action = &[:fn:],
                                     .op_code = attr.op_code,
                                     .prefix = attr.prefix,
                                     .cycles = attr.cycles,
                                     .size = static_cast<u16>(1 + data_len(attr.input) + data_len(attr.output)),
                                     .input = attr.input,
                                     .output = attr.output,
                                     .cond = attr.cond,
                };
            }
        }
    }

    return table;
}

consteval auto count(const std::array<instruction, 0x100>& table) -> int
{
    int n{ 0 };
    for (const auto& i : table)
        n += i.action != nullptr ? 1 : 0;
    return n;
}

constexpr std::array<instruction, 0x100> s_instructions{ build_table(false) };
constexpr std::array<instruction, 0x100> s_prefixed_instructions{ build_table(true) };

static_assert(count(s_instructions) == 245, "unprefixed instruction count");
static_assert(count(s_prefixed_instructions) == 256, "prefixed instruction count");

} // namespace

auto cpu::instruction_table(bool prefixed) -> const std::array<instruction, 0x100>&
{
    return prefixed ? s_prefixed_instructions : s_instructions;
}

} // namespace cx::gbc
