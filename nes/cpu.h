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
    void clock();
    void set_nmi();
    void set_irq();

    cpu_bus m_bus;

  private:
    static constexpr u16 NMI_VECTOR{ 0xFFFA };
    static constexpr u16 RESET_VECTOR{ 0xFFFC };
    static constexpr u16 IRQ_VECTOR{ 0xFFFE };

    void set_status(u8 inp, bool set_break = false);
    void set_initial_state();
    void check_interrupts();
    void nmi();
    void irq();

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
    u64 m_total_clock{ 0 };
    u64 m_cycles{ 0 };
    u64 m_clock{ 0 };
    u64 m_instrs{ 0 };

    // Interrupts
    bool m_nmi{ false };
    bool m_irq{ false };

    // Op Code Table
    using op_func = std::function<void(cpu*)>;
    op_func m_op_table[0xFF];
    void setup_op_codes();

    // Helpers
    void push(u8 value);
    void push_word(u16 value);
    void push_status();
    auto pull() -> u8;
    auto pull_word() -> u16;
    void pull_status();

    auto get_address(mode mode, bool watch_page_boundary) -> u16;
    auto read_next(mode mode, bool watch_page_boundary = false) -> u8;
    void write_next(mode mode, u8 value, bool watch_page_boundary = false);
    void set_negative_and_zero(u8 value);
    auto load(mode mode) -> u8;
    void store(u8 value, mode mode);
    void add_arth_clock_time(mode);
    void add_shift_clock_time(mode);

    auto _or(u8, mode) -> u8;
    auto _xor(u8, mode) -> u8;
    auto _and(u8, mode) -> u8;
    auto add_carry(u8, u8) -> u8;
    auto add(u8, mode) -> u8;
    auto sub(u8, mode) -> u8;
    void cmp(u8, mode);
    auto dec(u8) -> u8;
    void dec_addr(mode);
    auto inc(u8) -> u8;
    void inc_addr(mode);
    auto asl(u8) -> u8;
    void asl_addr(mode);
    auto lsr(u8) -> u8;
    void lsr_addr(mode);
    auto rol(u8) -> u8;
    void rol_addr(mode);
    auto ror(u8) -> u8;
    void ror_addr(mode);
    void nop(u64 cycles = 2);
    void test_bit(mode);
    void jump(mode);
    void brk();
    void rti();
    void rts();
    void jsr();
    void branch(bool cond);
    void tax();
    void txa();
    void tay();
    void tya();
    void tsx();
    void txs();
    void pla();
    void pha();
    void plp();
    void php();
};

} // namespace cx::nes