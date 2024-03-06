#include "cpu.h"
#include <iostream>

namespace cx::nes
{

cpu::cpu(cpu_bus* bus) noexcept : m_bus{ bus }
{
    set_initial_state();
    pc = m_bus->read_word(RESET_VECTOR);
}

void cpu::clock()
{
    check_interrupts();

    const auto op{ m_bus->read(pc) };
    const auto func{ m_op_table[op] };
    if (!func)
    {
        std::cerr << "Op code '0x" << std::hex << (u32)op << "' not supported." << std::endl;
        return;
    }

    func(this);
    m_total_clock += m_clock;
    m_cycles += m_clock;
    m_clock = 0;
}

void cpu::set_nmi()
{
    m_nmi = true;
}

void cpu::set_irq()
{
    m_irq = true;
}

void cpu::set_initial_state()
{
    a = 0;
    y = 0;
    x = 0;
    sp = 0xFD;
    m_status.bits = 0x20;
}

void cpu::check_interrupts()
{
    if (m_nmi)
        nmi();

    if (m_irq)
        irq();
}

void cpu::nmi()
{
    m_nmi = false;
    m_cycles += 8;
    push_word(pc);
    m_status.flags.break_bit = true;
    m_status.flags.interrupt_disable = true;
    push_status();

    pc = m_bus->read_word(NMI_VECTOR);
}

void cpu::irq()
{
    if (!m_status.flags.interrupt_disable)
    {
        m_irq = false;
        m_cycles += 7;

        push_word(pc);
        m_status.flags.break_bit = true;
        m_status.flags.interrupt_disable = true;
        push_status();

        pc = m_bus->read_word(IRQ_VECTOR);
    }
}

void cpu::push(u8 value)
{
    m_bus->write(sp + 0x100, value);
    sp--;
}

void cpu::push_word(u16 value)
{
    push(value >> 8);
    push(value & 0xFF);
}

void cpu::push_status()
{
    push(m_status.bits);
}

auto cpu::pull() -> u8
{
    sp++;
    return m_bus->read(sp + 0x100);
}

auto cpu::pull_word() -> u16
{
    return pull() | (pull() << 8);
}

void cpu::pull_status()
{
    m_status.bits = pull();
}

auto cpu::get_address(mode mode, bool watch_page_boundary) -> u16
{
    u16 addr{ 0 };
    u16 page{ 0 };
    u16 new_page{ 0 };

    u16 argument = pc + 1;

    switch (mode)
    {
    case mode::imm:
        pc += 2;
        return argument;
    case mode::zp:
        pc += 2;
        addr = m_bus->read(argument);
        return addr;
    case mode::zpx:
        pc += 2;
        return (m_bus->read(argument) + x) & 0xFF;
    case mode::zpy:
        pc += 2;
        return (m_bus->read(argument) + y) & 0xFF;
    case mode::abs:
        pc += 3;
        return m_bus->read_word(argument);
    case mode::absx:
        pc += 3;
        addr = m_bus->read_word(argument);
        page = addr >> 8;
        addr += x;
        new_page = addr >> 8;

        if (watch_page_boundary && page != new_page)
            m_clock++;

        return addr;
    case mode::absy:
        pc += 3;
        addr = m_bus->read_word(argument);
        page = addr >> 8;
        addr += y;
        new_page = addr >> 8;

        if (watch_page_boundary && page != new_page)
            m_clock++;

        return addr;
    case mode::indx:
        pc += 2;

        addr = m_bus->read(argument);
        addr += x;
        addr &= 0xFF;
        return m_bus->read_word_wrapped(addr);
    case mode::indy:
        pc += 2;
        addr = m_bus->read(argument);
        addr = m_bus->read_word_wrapped(addr);
        if (watch_page_boundary && ((addr & 0xFF00) != ((addr + y) & 0xFF00)))
            m_clock++;

        return (addr + y) & 0xFFFF;
    case mode::imp:
    case mode::ind:
    case mode::rel:
        throw std::exception("NYI");
    default:
        throw std::exception("Unknown address mode.");
    }
}

auto cpu::read_next(mode mode, bool watch_page_boundary) -> u8
{
    return m_bus->read(get_address(mode, watch_page_boundary));
}

void cpu::write_next(mode mode, u8 value, bool watch_page_boundary)
{
    m_bus->write(get_address(mode, watch_page_boundary), value);
}

void cpu::set_negative_and_zero(u8 value)
{
    m_status.flags.negative = (value & 0x80) == 0x80;
    m_status.flags.zero = value == 0;
}

auto cpu::load(mode mode) -> u8
{
    if (mode == mode::imm)
        m_clock += 2;
    else if (mode == mode::zp)
        m_clock += 3;
    else if (mode == mode::zpx || mode == mode::zpy || mode == mode::abs || mode == mode::absx || mode == mode::absy)
        m_clock += 4;
    else if (mode == mode::indx)
        m_clock += 6;
    else if (mode == mode::indy)
        m_clock += 5;
    else
        throw std::exception("Invalid load");

    auto value{ read_next(mode, true) };
    set_negative_and_zero(value);
    return value;
}

void cpu::store(u8 value, mode mode)
{
    if (mode == mode::zp || mode == mode::zpy)
        m_clock += 3;
    else if (mode == mode::abs || mode == mode::zpx)
        m_clock += 4;
    else if (mode == mode::absx || mode == mode::absy)
        m_clock += 5;
    else if (mode == mode::indx || mode == mode::indy)
        m_clock += 6;
    else
        throw std::exception("Invalid store");

    write_next(mode, value);
}

void cpu::add_arth_clock_time(mode mode)
{
    switch (mode)
    {
    case mode::imm:
        m_clock += 2;
        break;
    case mode::zp:
        m_clock += 3;
        break;
    case mode::zpx:
    case mode::abs:
    case mode::absx:
    case mode::absy:
        m_clock += 4;
        break;
    case mode::indy:
        m_clock += 5;
        break;
    case mode::indx:
        m_clock += 6;
        break;
    default:
        break;
    }
}

void cpu::add_shift_clock_time(mode mode)
{
    switch (mode)
    {
    case mode::imm:
        m_clock += 2;
        break;
    case mode::zp:
        m_clock += 5;
        break;
    case mode::zpx:
    case mode::abs:
        m_clock += 6;
        break;
    case mode::absx:
        m_clock += 7;
        break;
    default:
        break;
    }
}

auto cpu::_or(u8 value, mode mode) -> u8
{
    add_arth_clock_time(mode);

    u8 ret{ static_cast<u8>(value | read_next(mode, true)) };
    set_negative_and_zero(ret);
    return ret;
}

auto cpu::_xor(u8 value, mode mode) -> u8
{
    add_arth_clock_time(mode);

    u8 ret{ static_cast<u8>(value ^ read_next(mode, true)) };
    set_negative_and_zero(ret);
    return ret;
}

auto cpu::_and(u8 value, mode mode) -> u8
{
    add_arth_clock_time(mode);

    u8 ret{ static_cast<u8>(value & read_next(mode, true)) };
    set_negative_and_zero(ret);
    return ret;
}

auto cpu::add_carry(u8 input, u8 operand) -> u8
{
    i32 result = static_cast<i8>(input) + static_cast<i8>(operand) + m_status.flags.carry;

    m_status.flags.overflow = result < -128 || result > 127;
    m_status.flags.carry = (input + operand + (m_status.flags.carry)) > 0xFF;

    auto ret = static_cast<u8>(result);
    set_negative_and_zero(ret);
    return ret;
}

auto cpu::add(u8 value, mode mode) -> u8
{
    add_arth_clock_time(mode);
    return add_carry(value, read_next(mode));
}

auto cpu::sub(u8 value, mode mode) -> u8
{
    add_arth_clock_time(mode);
    return add_carry(value, ~read_next(mode));
}

void cpu::cmp(u8 value, mode mode)
{
    add_arth_clock_time(mode);

    u64 res{ static_cast<u64>(value - read_next(mode)) };
    m_status.flags.negative = (res & 0x80) == 0x80;
    m_status.flags.zero = res == 0;
    m_status.flags.carry = res >= 0;
}

auto cpu::dec(u8 value) -> u8
{
    add_arth_clock_time(mode::imm);

    pc++;
    u8 ret{ static_cast<u8>(value - 1) };
    set_negative_and_zero(ret);
    return ret;
}

void cpu::dec_addr(mode mode)
{
    add_shift_clock_time(mode);

    auto addr{ get_address(mode, false) };
    auto value{ m_bus->read(addr) };
    value--;
    set_negative_and_zero(value);

    m_bus->write(addr, value);
}

void cpu::inc_addr(mode mode)
{
    add_shift_clock_time(mode);

    auto addr{ get_address(mode, false) };
    auto value{ m_bus->read(addr) };
    value++;
    set_negative_and_zero(value);

    m_bus->write(addr, value);
}

} // namespace cx::nes
