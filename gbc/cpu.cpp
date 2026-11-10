#include "cpu.h"
#include "byte.h"
#include "emulator.h"
#include <format>
#include <stdexcept>

#ifdef NOISY
#include "spdlog/spdlog.h"
#endif

namespace cx::gbc
{

cpu::cpu(emulator* system) noexcept : m_system{ system }
{
}

//
// Flags and registers
//
auto cpu::flags_as_byte() const -> u8
{
    return m_flags.bits;
}

auto cpu::set_flags(u8 value) -> void
{
    m_flags.bits = value & 0xF0;
}

auto cpu::af() const -> u16
{
    return byte::combine(a, flags_as_byte());
}

auto cpu::set_af(u16 value) -> void
{
    a = byte::upper(value);
    set_flags(byte::lower(value));
}

auto cpu::bc() const -> u16
{
    return byte::combine(b, c);
}

auto cpu::set_bc(u16 value) -> void
{
    b = byte::upper(value);
    c = byte::lower(value);
}

auto cpu::de() const -> u16
{
    return byte::combine(d, e);
}

auto cpu::set_de(u16 value) -> void
{
    d = byte::upper(value);
    e = byte::lower(value);
}

auto cpu::hl() const -> u16
{
    return byte::combine(h, l);
}

auto cpu::set_hl(u16 value) -> void
{
    h = byte::upper(value);
    l = byte::lower(value);
}

auto cpu::hl_post_increment() -> u16
{
    const auto old{ hl() };
    set_hl(static_cast<u16>(old + 1));
    return old;
}

auto cpu::hl_post_decrement() -> u16
{
    const auto old{ hl() };
    set_hl(static_cast<u16>(old - 1));
    return old;
}

//
// Interrupts
//

// IF reads with the upper 3 bits set, but m_if holds whatever was written (see request_interrupt)
auto cpu::get_if() const -> u8
{
    return static_cast<u8>(m_if | 0xE0);
}

auto cpu::set_if(u8 value) -> void
{
    m_if = value;
}

auto cpu::request_interrupt(interrupt type) -> void
{
    if (m_stopped && type == interrupt::joypad)
        m_stopped = false;

    switch (type)
    {
    case interrupt::vblank:
        set_if(get_if() | 0x01);
        break;
    case interrupt::stat:
        set_if(get_if() | 0x02);
        break;
    case interrupt::timer:
        set_if(get_if() | 0x04);
        break;
    case interrupt::serial:
        set_if(get_if() | 0x08);
        break;
    case interrupt::joypad:
        set_if(get_if() | 0x10);
        break;
    }
}

auto cpu::service_interrupt() -> void
{
    m_ime = false;
    m_halted = false;
    m_cycles_last_clock += 5;

    // Manually push word becuase of stupid logic edge case:
    // -> If the pushing of the *upper* byte of PC blanked out our IE bits, jump to $0000
    auto ie{ m_ie };
    push_byte(byte::upper(pc));

    if (ie != m_ie)
        ie = m_ie;

    push_byte(byte::lower(pc));

    // Only service the first enabled interrupt
    const auto eif{ m_if & ie };

    if (eif == 0x0000)
    {
        pc = 0x00;
        return;
    }

    if ((eif & 0x01) == 0x01)
    {
        set_if(get_if() & 0xFE); // Clear bit
        pc = 0x40;               // Goto VBlank handler
    }
    else if ((eif & 0x02) == 0x02)
    {
        set_if(get_if() & 0xFD); // Clear bit
        pc = 0x48;               // Goto Stat handler
    }
    else if ((eif & 0x04) == 0x04)
    {
        set_if(get_if() & 0xFB); // Clear bit
        pc = 0x50;               // Goto Timer handler
    }
    else if ((eif & 0x08) == 0x08)
    {
        set_if(get_if() & 0xF7); // Clear bit
        pc = 0x58;               // Goto Serial handler
    }
    else if ((eif & 0x10) == 0x10)
    {
        set_if(get_if() & 0xEF); // Clear bit
        pc = 0x60;               // Goto Joypad handler
    }
}

//
// Main loop
//
auto cpu::clock() -> void
{
#ifdef NOISY
    if (auto log{ spdlog::get("async") })
        log->debug("{}", trace_line());
#endif

    // Reset state
    m_jumped = false;

    if (m_halted && (m_if & m_ie) > 0)
        m_halted = false;

    if (m_ime && (m_if & m_ie) > 0)
        service_interrupt();

    if (!m_halted && !m_stopped)
    {
        // Get instruction
        auto op{ m_system->m_bus.read(pc) };

        auto prefixed{ false };
        if (op == 0xCB)
        {
            op = m_system->m_bus.read(++pc);
            prefixed = true;
        }

        const auto& instr{ instruction_table(prefixed)[op] };

        // Carry out instruction
        if (instr.action == nullptr)
            throw std::runtime_error{ std::format("Unknown opcode: {:02X}, prefixed? {}", op, prefixed ? 'Y' : 'N') };

        m_current = &instr;
        pc += m_current->size;
        instr.action(this);

        // Store how many cycles the instruction took
        m_cycles_last_clock = static_cast<u32>(instr.cycles + (m_jumped ? 4 : 0));
    }
    else
    {
        m_cycles_last_clock = 4;
    }
}

auto cpu::reset() -> void
{
    // These are BGB default values, need to see if they're correct
    // Presumably, running boot rom would setup these values
    a = 0x01;
    b = 0x00;
    c = 0x13;
    d = 0x00;
    e = 0xD8;
    h = 0x01;
    l = 0x4D;

    m_flags.flags.c = true;
    m_flags.flags.h = true;
    m_flags.flags.n = false;
    m_flags.flags.z = true;

    sp = 0xFFFE;
    pc = 0x0100;
}

//
// Helpers
//
auto cpu::push_byte(u8 value) -> void
{
    m_system->m_bus.write(--sp, value);
}

auto cpu::push_word(u16 value) -> void
{
    m_system->m_bus.write(--sp, byte::upper(value));
    m_system->m_bus.write(--sp, byte::lower(value));
}

auto cpu::pop_byte() -> u8
{
    return m_system->m_bus.read(sp++);
}

auto cpu::pop_word() -> u16
{
    // C# evaluates arguments left to right, C++ does not
    const auto lsb{ pop_byte() };
    const auto msb{ pop_byte() };
    return byte::combine_r(lsb, msb);
}

auto cpu::imm() -> u8
{
    return m_system->m_bus.read(static_cast<u16>(pc - 1));
}

auto cpu::abs() -> u16
{
    return m_system->m_bus.read_word(static_cast<u16>(pc - 2));
}

auto cpu::data_as_byte(data which) -> u8
{
    auto& bus{ m_system->m_bus };

    switch (which)
    {
    case data::a:
        return a;
    case data::b:
        return b;
    case data::c:
        return c;
    case data::d:
        return d;
    case data::e:
        return e;
    case data::h:
        return h;
    case data::l:
        return l;

    case data::imm:
        return imm();

    case data::ind_abs:
        return bus.read(abs());
    case data::ind_imm:
        return bus.read(byte::combine(0xFF, imm()));
    case data::ind_c:
        return bus.read(byte::combine(0xFF, c));
    case data::ind_bc:
        return bus.read(bc());
    case data::ind_de:
        return bus.read(de());
    case data::ind_hl:
        return bus.read(hl());

    case data::inc_hl:
        return bus.read(hl_post_increment());
    case data::dec_hl:
        return bus.read(hl_post_decrement());

    default:
        throw std::runtime_error{ "Invalid operation" };
    }
}

auto cpu::out_byte() -> u8
{
    return data_as_byte(m_current->output);
}

auto cpu::in_byte() -> u8
{
    return data_as_byte(m_current->input);
}

auto cpu::in_word() -> u16
{
    switch (m_current->input)
    {
    case data::abs:
        return abs();
    case data::bc:
        return bc();
    case data::de:
        return de();
    case data::hl:
        return hl();
    case data::af:
        return af();
    case data::sp:
        return sp;

    case data::sp_plus_imm:
        return get_sp_with_imm_offset_and_set_flags();

    default:
        throw std::runtime_error{ "Invalid operation" };
    }
}

auto cpu::write_byte(u8 val) -> void
{
    auto& bus{ m_system->m_bus };

    switch (m_current->output)
    {
    case data::none:
        throw std::runtime_error{ "Invalid operation" };

    case data::a:
        a = val;
        break;
    case data::b:
        b = val;
        break;
    case data::c:
        c = val;
        break;
    case data::d:
        d = val;
        break;
    case data::e:
        e = val;
        break;
    case data::h:
        h = val;
        break;
    case data::l:
        l = val;
        break;

    case data::ind_abs:
        bus.write(abs(), val);
        break;
    case data::ind_imm:
        bus.write(byte::combine(0xFF, imm()), val);
        break;
    case data::ind_c:
        bus.write(byte::combine(0xFF, c), val);
        break;
    case data::ind_bc:
        bus.write(bc(), val);
        break;
    case data::ind_de:
        bus.write(de(), val);
        break;
    case data::ind_hl:
        bus.write(hl(), val);
        break;

    case data::inc_hl:
        bus.write(hl_post_increment(), val);
        break;
    case data::dec_hl:
        bus.write(hl_post_decrement(), val);
        break;

    case data::imm:
        bus.write(imm(), val);
        break;
    case data::abs:
        bus.write(abs(), val);
        break;

    default:
        throw std::runtime_error{ "Invalid operation" };
    }
}

auto cpu::write_word(u16 val) -> void
{
    switch (m_current->output)
    {
    case data::none:
        throw std::runtime_error{ "Invalid operation" };

    case data::bc:
        set_bc(val);
        break;
    case data::de:
        set_de(val);
        break;
    case data::hl:
        set_hl(val);
        break;
    case data::af:
        set_af(val);
        break;
    case data::sp:
        sp = val;
        break;

    case data::ind_abs:
        m_system->m_bus.write_word(abs(), val);
        break;

    default:
        throw std::runtime_error{ "Invalid operation" };
    }
}

auto cpu::write() -> void
{
    // Precedence kept exactly as the C#: the final && only binds to the HL comparison
    if (m_current->input == data::sp || m_current->input == data::bc || m_current->input == data::de ||
        (m_current->input == data::hl && m_current->output == data::ind_abs))
    {
        write_word(in_word());
        return;
    }

    switch (m_current->output)
    {
    case data::none:
        throw std::runtime_error{ "Invalid operation" };

    case data::a:
    case data::b:
    case data::c:
    case data::d:
    case data::e:
    case data::h:
    case data::l:
    case data::ind_abs:
    case data::ind_imm:
    case data::ind_c:
    case data::ind_bc:
    case data::ind_de:
    case data::ind_hl:
    case data::inc_hl:
    case data::dec_hl:
    case data::imm:
    case data::abs:
        write_byte(in_byte());
        break;

    case data::bc:
    case data::de:
    case data::hl:
    case data::sp:
        write_word(in_word());
        break;

    default:
        break;
    }
}

auto cpu::get_cond() -> bool
{
    switch (m_current->cond)
    {
    case condition::always:
        return true;
    case condition::z:
        return m_flags.flags.z;
    case condition::nz:
        return !m_flags.flags.z;
    case condition::c:
        return m_flags.flags.c;
    case condition::nc:
        return !m_flags.flags.c;

    default:
        throw std::runtime_error{ "Invalid operation" };
    }
}

auto cpu::jmp_cond_impl() -> void
{
    const auto cond{ get_cond() };

    if (cond)
    {
        m_jumped = true;
        jmp_impl();
    }
}

auto cpu::jmp_impl() -> void
{
    m_jumped = true;

    // Jump with offset
    if (m_current->input == data::imm)
        pc = static_cast<u16>(pc + static_cast<i8>(in_byte()));
    else // Jump using 16 bit data (such as an absolute value or combined register)
        pc = in_word();
}

auto cpu::ret_impl() -> void
{
    const auto cond{ get_cond() };

    if (cond)
    {
        m_jumped = true;

        pc = pop_word();
    }
}

auto cpu::get_sp_with_imm_offset_and_set_flags() -> u16
{
    const auto operand{ static_cast<i8>(imm()) };
    const int result{ sp + operand };

    m_flags.flags.z = false;
    m_flags.flags.c = (((sp & 0xFF) + (operand & 0xFF)) & 0x100) == 0x100;
    m_flags.flags.n = false;
    m_flags.flags.h = (((sp & 0xF) + (operand & 0xF)) & 0x10) == 0x10;

    return static_cast<u16>(result & 0xFFFF);
}

// Easier to have this as a specific method as it has various quirks
auto cpu::add_sp_imm_impl() -> void
{
    if (m_current->output != data::sp && m_current->input != data::imm)
        throw std::runtime_error{ "Invalid operation" };

    sp = get_sp_with_imm_offset_and_set_flags();
}

auto cpu::add_16_impl() -> void
{
    // Only HL is used as output for 16-bit addition
    if (m_current->output != data::hl)
        throw std::runtime_error{ "Invalid operation" };

    // Get our operand
    const auto operand{ in_word() };

    // Add operand to HL and store in int so we can check if we need to carry
    const int result{ hl() + operand };

    // Set flags
    m_flags.flags.c = result > 0xFFFF;
    m_flags.flags.n = false;
    m_flags.flags.h = ((hl() & 0x0FFF) + (operand & 0x0FFF)) > 0x0FFF;

    // Cast back to ushort and store in HL
    set_hl(static_cast<u16>(result & 0xFFFF));
}

auto cpu::add_impl(bool use_carry) -> void
{
    // Only A is used as output for addition
    if (m_current->output != data::a)
        throw std::runtime_error{ "Invalid operation" };

    const auto carry{ static_cast<u8>((use_carry && m_flags.flags.c) ? 1 : 0) };
    const auto prev_a{ a };

    // Get our operand
    const auto operand{ in_byte() };

    // Add operand to A and store in int so we can check if we need to carry
    a = static_cast<u8>((prev_a + operand + carry) & 0xFF);

    // Set flags
    m_flags.flags.z = a == 0;
    m_flags.flags.c = (prev_a + operand + carry) > 0xFF;
    m_flags.flags.n = false;
    m_flags.flags.h = ((prev_a & 0xF) + (operand & 0xF) + carry) > 0x0F;
}

auto cpu::sub_impl(bool use_carry) -> void
{
    // Only A is used as output for subtraction
    if (m_current->output != data::a)
        throw std::runtime_error{ "Invalid operation" };

    const auto carry{ static_cast<u8>((use_carry && m_flags.flags.c) ? 1 : 0) };
    const auto prev_a{ a };

    // Get our operand
    const auto operand{ in_byte() };

    // Add operand to A and store in int so we can check if we need to carry
    a = static_cast<u8>((prev_a - operand - carry) & 0xFF);

    // Set flags
    m_flags.flags.z = a == 0;
    m_flags.flags.c = (prev_a - operand - carry) < 0x00;
    m_flags.flags.n = true;
    m_flags.flags.h = ((prev_a & 0xF) - (operand & 0xF) - carry) < 0x00;
}

auto cpu::cp_impl() -> void
{
    // Only A is used as output for subtraction
    if (m_current->output != data::a)
        throw std::runtime_error{ "Invalid operation" };

    // Get our operand
    const auto operand{ in_byte() };

    // Sub operand from A and store in int so we went negative
    const int result{ a - operand };

    // Set flags
    m_flags.flags.c = result < 0x00;
    m_flags.flags.h = (((a & 0xF) - (operand & 0xF)) & 0x10) == 0x10;
    m_flags.flags.n = true;
    m_flags.flags.z = result == 0;
}

auto cpu::daa_impl() -> void
{
    if (m_flags.flags.n) // After Subtraction
    {
        if (m_flags.flags.c)
            a = static_cast<u8>(a - 0x60);

        if (m_flags.flags.h)
            a = static_cast<u8>(a - 0x06);
    }
    else // After Addition
    {
        if (m_flags.flags.c || a > 0x99)
        {
            a = static_cast<u8>(a + 0x60);
            m_flags.flags.c = true;
        }
        if (m_flags.flags.h || (a & 0x0F) > 0x09)
        {
            a = static_cast<u8>(a + 0x06);
        }
    }

    // Set flags
    m_flags.flags.h = false;
    m_flags.flags.z = a == 0;
}

auto cpu::xor_impl() -> void
{
    // Only A is used as output for XOR
    if (m_current->output != data::a)
        throw std::runtime_error{ "Invalid operation" };

    // Get our operand
    const auto operand{ in_byte() };

    // Do XOR
    a ^= operand;

    // Set flags
    m_flags.flags.z = a == 0;
    m_flags.flags.n = false;
    m_flags.flags.h = false;
    m_flags.flags.c = false;
}

auto cpu::cpl_impl() -> void
{
    // Only A is used as output for CPL
    if (m_current->output != data::a)
        throw std::runtime_error{ "Invalid operation" };

    a = static_cast<u8>(~a);

    m_flags.flags.n = true;
    m_flags.flags.h = true;
}

auto cpu::get_bit() -> u8
{
    switch (m_current->input)
    {
    case data::bit0:
        return 0b00000001;
    case data::bit1:
        return 0b00000010;
    case data::bit2:
        return 0b00000100;
    case data::bit3:
        return 0b00001000;
    case data::bit4:
        return 0b00010000;
    case data::bit5:
        return 0b00100000;
    case data::bit6:
        return 0b01000000;
    case data::bit7:
        return 0b10000000;

    default:
        throw std::runtime_error{ "Invalid operation" };
    }
}

auto cpu::bit_impl() -> void
{
    const auto bit{ get_bit() };
    const auto val{ out_byte() };

    m_flags.flags.z = (bit & val) == 0;
    m_flags.flags.n = false;
    m_flags.flags.h = true;
}

auto cpu::set_bit_impl() -> void
{
    const auto bit{ get_bit() };
    const auto val{ out_byte() };

    write_byte(static_cast<u8>(val | bit));
}

auto cpu::res_bit_impl() -> void
{
    const int bit{ ~get_bit() };
    const auto val{ out_byte() };

    write_byte(static_cast<u8>(val & bit));
}

auto cpu::or_impl() -> void
{
    // Only A is used as output for XOR
    if (m_current->output != data::a)
        throw std::runtime_error{ "Invalid operation" };

    // Get our operand
    const auto operand{ in_byte() };

    // Do OR
    a |= operand;

    // Set flags
    m_flags.flags.z = a == 0;
    m_flags.flags.n = false;
    m_flags.flags.h = false;
    m_flags.flags.c = false;
}

auto cpu::and_impl() -> void
{
    // Only A is used as output for XOR
    if (m_current->output != data::a)
        throw std::runtime_error{ "Invalid operation" };

    // Get our operand
    const auto operand{ in_byte() };

    // Do OR
    a &= operand;

    // Set flags
    m_flags.flags.z = a == 0;
    m_flags.flags.n = false;
    m_flags.flags.h = true;
    m_flags.flags.c = false;
}

auto cpu::dec_impl() -> void
{
    auto& bus{ m_system->m_bus };
    u16 result{ 0 };

    switch (m_current->input)
    {
    case data::a:
        result = --a;
        break;
    case data::b:
        result = --b;
        break;
    case data::c:
        result = --c;
        break;
    case data::d:
        result = --d;
        break;
    case data::e:
        result = --e;
        break;
    case data::h:
        result = --h;
        break;
    case data::l:
        result = --l;
        break;
    case data::bc:
        set_bc(static_cast<u16>(bc() - 1));
        result = bc();
        break;
    case data::de:
        set_de(static_cast<u16>(de() - 1));
        result = de();
        break;
    case data::hl:
        set_hl(static_cast<u16>(hl() - 1));
        result = hl();
        break;
    case data::sp:
        result = --sp;
        break;

    case data::ind_hl: {
        const auto t{ static_cast<u8>(bus.read(hl()) - 1) };
        bus.write(hl(), t);
        result = t;
        break;
    }

    default:
        throw std::runtime_error{ "Invalid operation" };
    }

    const bool u16_op{ m_current->input == data::bc || m_current->input == data::de || m_current->input == data::hl ||
                       m_current->input == data::sp };

    // Don't set flags for u16 ops
    if (u16_op)
        return;

    m_flags.flags.z = result == 0;
    m_flags.flags.n = true;

    m_flags.flags.h = ((((result + 1) & 0xF) - 1) & 0x10) == 0x10;
}

auto cpu::inc_impl() -> void
{
    auto& bus{ m_system->m_bus };
    u16 result{ 0 };

    switch (m_current->input)
    {
    case data::a:
        result = ++a;
        break;
    case data::b:
        result = ++b;
        break;
    case data::c:
        result = ++c;
        break;
    case data::d:
        result = ++d;
        break;
    case data::e:
        result = ++e;
        break;
    case data::h:
        result = ++h;
        break;
    case data::l:
        result = ++l;
        break;
    case data::bc:
        set_bc(static_cast<u16>(bc() + 1));
        result = bc();
        break;
    case data::de:
        set_de(static_cast<u16>(de() + 1));
        result = de();
        break;
    case data::hl:
        set_hl(static_cast<u16>(hl() + 1));
        result = hl();
        break;
    case data::sp:
        result = ++sp;
        break;

    case data::ind_hl: {
        const auto t{ static_cast<u8>(bus.read(hl()) + 1) };
        bus.write(hl(), t);
        result = t;
        break;
    }

    default:
        throw std::runtime_error{ "Invalid operation" };
    }

    const bool u16_op{ m_current->input == data::bc || m_current->input == data::de || m_current->input == data::hl ||
                       m_current->input == data::sp };

    // Don't set flags for u16 ops
    if (u16_op)
        return;

    m_flags.flags.z = result == 0;
    m_flags.flags.n = false;

    m_flags.flags.h = ((((result - 1) & 0xF) + 1) & 0x10) == 0x10;
}

auto cpu::rr_impl(bool carry, bool keep_high_bit_high, bool move_high_to_low) -> void
{
    auto val{ in_byte() };
    const auto carry_in{ (carry && m_flags.flags.c) || (keep_high_bit_high && (val & 0x80) == 0x80) ||
                         (move_high_to_low && (val & 0x01) == 0x01) };
    const auto [res, carry_out]{ byte::ror(val, carry_in) };
    val = res;
    m_flags.flags.c = carry_out;
    write_byte(val);

    m_flags.flags.z = m_current->prefix && val == 0;
    m_flags.flags.h = false;
    m_flags.flags.n = false;
}

auto cpu::rl_impl(bool carry, bool arth) -> void
{
    auto val{ in_byte() };
    const auto carry_in{ (carry && m_flags.flags.c) || (arth && (val & 0x80) == 0x80) };
    const auto [res, carry_out]{ byte::rol(val, carry_in) };
    val = res;
    m_flags.flags.c = carry_out;
    write_byte(val);

    m_flags.flags.z = m_current->prefix && val == 0;
    m_flags.flags.h = false;
    m_flags.flags.n = false;
}

auto cpu::sla_impl() -> void
{
    auto val{ in_byte() };
    const auto [res, carry_out]{ byte::rol(val, false) };
    val = res;
    m_flags.flags.c = carry_out;
    write_byte(val);

    m_flags.flags.z = m_current->prefix && val == 0;
    m_flags.flags.h = false;
    m_flags.flags.n = false;
}

auto cpu::sra_impl() -> void
{
    auto val{ in_byte() };
    const auto [res, carry_out]{ byte::ror(val, (val & 0x80) == 0x80) };
    val = res;
    m_flags.flags.c = carry_out;
    write_byte(val);

    m_flags.flags.z = m_current->prefix && val == 0;
    m_flags.flags.h = false;
    m_flags.flags.n = false;
}

auto cpu::srl_impl() -> void
{
    auto val{ in_byte() };
    const auto [res, carry_out]{ byte::ror(val, false) };
    val = res;
    m_flags.flags.c = carry_out;
    write_byte(val);

    m_flags.flags.z = m_current->prefix && val == 0;
    m_flags.flags.h = false;
    m_flags.flags.n = false;
}

auto cpu::swap_impl(bool) -> void
{
    auto val{ in_byte() };
    val = byte::swap(val);
    write_byte(val);

    m_flags.flags.z = val == 0;
    m_flags.flags.c = false;
    m_flags.flags.h = false;
    m_flags.flags.n = false;
}

auto cpu::rst_impl() -> void
{
    push_word(pc);

    // Could be done with maffs innit, oh well
    u8 n{ 0 };
    switch (m_current->op_code)
    {
    case 0xC7:
        n = 0x00;
        break;
    case 0xD7:
        n = 0x10;
        break;
    case 0xE7:
        n = 0x20;
        break;
    case 0xF7:
        n = 0x30;
        break;
    case 0xCF:
        n = 0x08;
        break;
    case 0xDF:
        n = 0x18;
        break;
    case 0xEF:
        n = 0x28;
        break;
    case 0xFF:
        n = 0x38;
        break;

    default:
        throw std::runtime_error{ "Invalid operation" };
    }

    pc = byte::combine(0x00, n);

    m_jumped = true;
}

auto cpu::call_impl() -> void
{
    const auto cond{ get_cond() };

    if (cond)
    {
        push_word(pc);
        pc = in_word();
        m_jumped = true;
    }
}

//
// Instructions
//
auto cpu::nop() -> void
{
}

auto cpu::halt() -> void
{
    m_halted = true;
}

auto cpu::stop() -> void
{
    // If a button is being held
    if ((m_system->m_bus.read(0xFF00) & 0x0F) != 0x0F)
    {
        // And there's no interrupts pending
        if ((m_ie & get_if()) == 0)
        {
            m_halted = true;
            pc++;
        }
    }
    else
    {
        if ((m_ie & get_if()) != 0)
        {
            pc++;
        }

        m_stopped = true;
        m_system->m_bus.write(0xFF04, 0x00); // Reset DIV
    }
}

auto cpu::di() -> void
{
    m_ime = false;
}

auto cpu::ei() -> void
{
    m_ime = true;
}

auto cpu::ld() -> void
{
    write();
}

auto cpu::jmp() -> void
{
    jmp_impl();
}

auto cpu::ret() -> void
{
    ret_impl();
}

auto cpu::reti() -> void
{
    ret_impl();
    m_ime = true;
}

auto cpu::rst() -> void
{
    rst_impl();
}

auto cpu::call() -> void
{
    call_impl();
}

auto cpu::jr() -> void
{
    jmp_cond_impl();
}

auto cpu::scf() -> void
{
    m_flags.flags.c = true;
    m_flags.flags.h = false;
    m_flags.flags.n = false;
}

auto cpu::ccf() -> void
{
    m_flags.flags.c = !m_flags.flags.c;
    m_flags.flags.h = false;
    m_flags.flags.n = false;
}

auto cpu::pop() -> void
{
    write_word(pop_word());
}

auto cpu::push() -> void
{
    push_word(in_word());
}

auto cpu::daa() -> void
{
    daa_impl();
}

auto cpu::add_16() -> void
{
    add_16_impl();
}

auto cpu::add_sp_imm() -> void
{
    add_sp_imm_impl();
}

auto cpu::add() -> void
{
    add_impl(false);
}

auto cpu::adc() -> void
{
    add_impl(true);
}

auto cpu::sub() -> void
{
    sub_impl(false);
}

auto cpu::sbc() -> void
{
    sub_impl(true);
}

auto cpu::cp() -> void
{
    cp_impl();
}

auto cpu::_xor() -> void
{
    xor_impl();
}

auto cpu::_or() -> void
{
    or_impl();
}

auto cpu::_and() -> void
{
    and_impl();
}

auto cpu::dec() -> void
{
    dec_impl();
}

auto cpu::inc() -> void
{
    inc_impl();
}

auto cpu::rrc() -> void
{
    rr_impl(false, false, true);
}

auto cpu::rr() -> void
{
    rr_impl(true, false, false);
}

auto cpu::rlc() -> void
{
    rl_impl(false, true);
}

auto cpu::rl() -> void
{
    rl_impl(true);
}

auto cpu::sla() -> void
{
    sla_impl();
}

auto cpu::sra() -> void
{
    sra_impl();
}

auto cpu::srl() -> void
{
    srl_impl();
}

auto cpu::swap() -> void
{
    swap_impl(false);
}

auto cpu::cpl() -> void
{
    cpl_impl();
}

auto cpu::bit() -> void
{
    bit_impl();
}

auto cpu::res() -> void
{
    res_bit_impl();
}

auto cpu::set() -> void
{
    set_bit_impl();
}

#ifdef NOISY
auto cpu::trace_line() const -> std::string
{
    auto& bus{ m_system->m_bus };
    const auto pcmem1{ bus.read(pc) };
    const auto pcmem2{ bus.read(static_cast<u16>(pc + 1)) };
    const auto pcmem3{ bus.read(static_cast<u16>(pc + 2)) };
    const auto pcmem4{ bus.read(static_cast<u16>(pc + 3)) };

    return std::format("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{:04X} PC:{:04X} "
                       "PCMEM:{:02X},{:02X},{:02X},{:02X}",
                       a, flags_as_byte(), b, c, d, e, h, l, sp, pc, pcmem1, pcmem2, pcmem3, pcmem4);
}
#endif

} // namespace cx::gbc
