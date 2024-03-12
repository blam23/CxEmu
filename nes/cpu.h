#pragma once

#include "core/types.h"
#include "cpu_bus.h"
#include "forward.h"

namespace cx::nes
{

class cpu
{
  public:
    cpu(emulator*) noexcept;
    auto clock() -> u64;
    auto set_nmi() -> void;
    auto set_irq() -> void;

    cpu_bus m_bus;

  private:
    emulator* m_system;

    static constexpr u16 NMI_VECTOR{ 0xFFFA };
    static constexpr u16 RESET_VECTOR{ 0xFFFC };
    static constexpr u16 IRQ_VECTOR{ 0xFFFE };

    auto set_status(u8 inp, bool set_break = false) -> void;
    auto set_initial_state() -> void;
    auto check_interrupts() -> void;
    auto nmi() -> void;
    auto irq() -> void;

    enum class mode
    {
        none,
        imp,
        imm,
        zp,
        zpx,
        zpy,
        indx,
        indy,
        abs,
        absx,
        absy,
        ind,
        rel
    };

    // Status
    struct status_struct
    {
        bool carry : 1;
        bool zero : 1;
        bool interrupt_disable : 1;
        bool decimal : 1;
        bool break_bit : 1;
        bool ignored : 1;
        bool overflow : 1;
        bool negative : 1;
    };

    union status_union {
        status_struct flags;
        u8 bits;
    };

    status_union m_status{ 0 };

    // Registers
    u8 a{ 0 };
    u8 x{ 0 };
    u8 y{ 0 };
    u8 sp{ 0 };
    u16 pc{ 0 };

    // Clock
    u64 m_total_clock{ 7 };
    u64 m_cycles{ 0 };
    u64 m_clock{ 0 };
    u64 m_instrs{ 0 };

    // Interrupts
    bool m_nmi{ false };
    bool m_irq{ false };

    // Op Code Table
    using op_func = std::function<void(cpu*)>;
    op_func m_op_table[0xFF];
    auto setup_op_codes() -> void;

    // Helpers
    auto push(u8 value) -> void;
    auto push_word(u16 value) -> void;
    auto push_status() -> void;
    auto pull() -> u8;
    auto pull_word() -> u16;
    auto pull_status() -> void;

    auto get_address(mode mode, bool watch_page_boundary) -> u16;
    auto read_next(mode mode, bool watch_page_boundary = false) -> u8;
    auto write_next(mode mode, u8 value, bool watch_page_boundary = false) -> void;
    auto set_negative_and_zero(u8 value) -> void;
    auto load(mode mode) -> u8;
    auto store(u8 value, mode mode) -> void;
    auto add_arth_clock_time(mode) -> void;
    auto add_shift_clock_time(mode) -> void;

    auto _or(u8, mode) -> u8;
    auto _xor(u8, mode) -> u8;
    auto _and(u8, mode) -> u8;
    auto add_carry(u8, u8) -> u8;
    auto add(u8, mode) -> u8;
    auto sub(u8, mode) -> u8;
    auto cmp(u8, mode) -> void;
    auto dec(u8) -> u8;
    auto dec_addr(mode) -> void;
    auto inc(u8) -> u8;
    auto inc_addr(mode) -> void;
    auto asl(u8) -> u8;
    auto asl_addr(mode) -> void;
    auto lsr(u8) -> u8;
    auto lsr_addr(mode) -> void;
    auto rol(u8) -> u8;
    auto rol_addr(mode) -> void;
    auto ror(u8) -> u8;
    auto ror_addr(mode) -> void;
    auto nop(u64 cycles = 2) -> void;
    auto test_bit(mode) -> void;
    auto jump(mode) -> void;
    auto brk() -> void;
    auto rti() -> void;
    auto rts() -> void;
    auto jsr() -> void;
    auto branch(bool cond) -> void;
    auto tax() -> void;
    auto txa() -> void;
    auto tay() -> void;
    auto tya() -> void;
    auto tsx() -> void;
    auto txs() -> void;
    auto pla() -> void;
    auto pha() -> void;
    auto plp() -> void;
    auto php() -> void;
};

} // namespace cx::nes