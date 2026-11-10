#pragma once

#include "core/types.h"
#include "forward.h"
#include "reflection.h"
#include <array>
#include <string>

namespace cx::gbc
{

// One entry of an op table, built at compile time from the annotations in op_codes.cpp
struct instruction
{
    void (*action)(cpu*){ nullptr };
    u8 op_code{ 0 };
    bool prefix{ false };
    int cycles{ 0 };
    u16 size{ 0 };
    data input{ data::none };
    data output{ data::none };
    condition cond{ condition::none };
};

// Sharp LR25902 (clone of Z80)
class cpu
{
  public:
    cpu(emulator*) noexcept;

    auto clock() -> void;
    auto reset() -> void;

    enum class interrupt
    {
        vblank,
        stat,
        timer,
        serial,
        joypad
    };
    auto request_interrupt(interrupt type) -> void;

    auto stopped() const -> bool
    {
        return m_stopped;
    }
    auto halted() const -> bool
    {
        return m_halted;
    }
    auto cycles_last_clock() const -> u32
    {
        return m_cycles_last_clock;
    }

    // Interrupt registers
    [[= io{ 0xFFFF }]] u8 m_ie{ 0 };
    [[= io_read{ 0xFF0F }]] auto get_if() const -> u8;
    [[= io_write{ 0xFF0F }]] auto set_if(u8 value) -> void;

#ifdef NOISY
    // GB Doctor format, taken before each clock
    auto trace_line() const -> std::string;
#endif

    // Holds the annotated instruction functions, see op_codes.cpp
    struct ops;

  private:
    emulator* m_system;

    // Flags: the lower 4 bits are always zero
    struct flags_struct
    {
        bool pad0 : 1;
        bool pad1 : 1;
        bool pad2 : 1;
        bool pad3 : 1;
        bool c : 1; // carry
        bool h : 1; // (BCD) carry lower 4 bits
        bool n : 1; // (BCD) last instr was addition or subtraction?
        bool z : 1; // zero
    };

    union flags_union {
        flags_struct flags;
        u8 bits;
    };

    // Registers
    u16 sp{ 0 };
    u16 pc{ 0 };

    u8 a{ 0 };
    u8 b{ 0 };
    u8 c{ 0 };
    u8 d{ 0 };
    u8 e{ 0 };
    u8 h{ 0 };
    u8 l{ 0 };

    flags_union m_flags{ 0 };

    auto flags_as_byte() const -> u8;
    auto set_flags(u8 value) -> void;

    auto af() const -> u16;
    auto set_af(u16 value) -> void;
    auto bc() const -> u16;
    auto set_bc(u16 value) -> void;
    auto de() const -> u16;
    auto set_de(u16 value) -> void;
    auto hl() const -> u16;
    auto set_hl(u16 value) -> void;

    // HL++ and HL-- as C# properties: use the old value, store the new one
    auto hl_post_increment() -> u16;
    auto hl_post_decrement() -> u16;

    bool m_ime{ true };
    bool m_halted{ false };
    bool m_stopped{ false };

    auto service_interrupt() -> void;

    // Current state
    const instruction* m_current{ nullptr };
    bool m_jumped{ false };
    u32 m_cycles_last_clock{ 0 };

    // Op tables (256 entries each, null where unregistered)
    static auto instruction_table(bool prefixed) -> const std::array<instruction, 0x100>&;

    // Helpers
    auto push_byte(u8 value) -> void;
    auto push_word(u16 value) -> void;
    auto pop_byte() -> u8;
    auto pop_word() -> u16;

    auto imm() -> u8;
    auto abs() -> u16;

    auto data_as_byte(data which) -> u8;
    auto out_byte() -> u8;
    auto in_byte() -> u8;
    auto in_word() -> u16;
    auto write_byte(u8 value) -> void;
    auto write_word(u16 value) -> void;
    auto write() -> void;
    auto get_cond() -> bool;
    auto get_bit() -> u8;

    auto jmp_cond_impl() -> void;
    auto jmp_impl() -> void;
    auto ret_impl() -> void;
    auto get_sp_with_imm_offset_and_set_flags() -> u16;
    auto add_sp_imm_impl() -> void;
    auto add_16_impl() -> void;
    auto add_impl(bool use_carry) -> void;
    auto sub_impl(bool use_carry) -> void;
    auto cp_impl() -> void;
    auto daa_impl() -> void;
    auto xor_impl() -> void;
    auto cpl_impl() -> void;
    auto bit_impl() -> void;
    auto set_bit_impl() -> void;
    auto res_bit_impl() -> void;
    auto or_impl() -> void;
    auto and_impl() -> void;
    auto dec_impl() -> void;
    auto inc_impl() -> void;
    auto rr_impl(bool carry, bool keep_high_bit_high, bool move_high_to_low) -> void;
    auto rl_impl(bool carry, bool arth = false) -> void;
    auto sla_impl() -> void;
    auto sra_impl() -> void;
    auto srl_impl() -> void;
    auto swap_impl(bool carry) -> void;
    auto rst_impl() -> void;
    auto call_impl() -> void;

    // Instructions (wrapped and annotated in op_codes.cpp)
    auto nop() -> void;
    auto halt() -> void;
    auto stop() -> void;
    auto di() -> void;
    auto ei() -> void;
    auto ld() -> void;
    auto jmp() -> void;
    auto ret() -> void;
    auto reti() -> void;
    auto rst() -> void;
    auto call() -> void;
    auto jr() -> void;
    auto scf() -> void;
    auto ccf() -> void;
    auto pop() -> void;
    auto push() -> void;
    auto daa() -> void;
    auto add_16() -> void;
    auto add_sp_imm() -> void;
    auto add() -> void;
    auto adc() -> void;
    auto sub() -> void;
    auto sbc() -> void;
    auto cp() -> void;
    auto _xor() -> void;
    auto _or() -> void;
    auto _and() -> void;
    auto dec() -> void;
    auto inc() -> void;
    auto rrc() -> void;
    auto rr() -> void;
    auto rlc() -> void;
    auto rl() -> void;
    auto sla() -> void;
    auto sra() -> void;
    auto srl() -> void;
    auto swap() -> void;
    auto cpl() -> void;
    auto bit() -> void;
    auto res() -> void;
    auto set() -> void;

    u8 m_if{ 0 };
};

} // namespace cx::gbc
