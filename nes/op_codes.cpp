#include "cpu.h"
#include <iostream>

namespace cx::nes
{

void cpu::setup_op_codes()
{
    // LDA
    m_op_table[0xA9] = [](cpu* c) { c->a = c->load(mode::imm); };
    m_op_table[0xA5] = [](cpu* c) { c->a = c->load(mode::zp); };
    m_op_table[0xB5] = [](cpu* c) { c->a = c->load(mode::zpx); };
    m_op_table[0xA1] = [](cpu* c) { c->a = c->load(mode::indx); };
    m_op_table[0xB1] = [](cpu* c) { c->a = c->load(mode::indy); };
    m_op_table[0xAD] = [](cpu* c) { c->a = c->load(mode::abs); };
    m_op_table[0xBD] = [](cpu* c) { c->a = c->load(mode::absx); };
    m_op_table[0xB9] = [](cpu* c) { c->a = c->load(mode::absy); };

    // LDX
    m_op_table[0xA2] = [](cpu* c) { c->x = c->load(mode::imm); };
    m_op_table[0xA6] = [](cpu* c) { c->x = c->load(mode::zp); };
    m_op_table[0xB6] = [](cpu* c) { c->x = c->load(mode::zpy); };
    m_op_table[0xAE] = [](cpu* c) { c->x = c->load(mode::abs); };
    m_op_table[0xBE] = [](cpu* c) { c->x = c->load(mode::absy); };

    // LDY
    m_op_table[0xA0] = [](cpu* c) { c->y = c->load(mode::imm); };
    m_op_table[0xA4] = [](cpu* c) { c->y = c->load(mode::zp); };
    m_op_table[0xB4] = [](cpu* c) { c->y = c->load(mode::zpx); };
    m_op_table[0xAC] = [](cpu* c) { c->y = c->load(mode::abs); };
    m_op_table[0xBC] = [](cpu* c) { c->y = c->load(mode::absx); };

    // STA
    m_op_table[0x85] = [](cpu* c) { c->store(c->a, mode::zp); };
    m_op_table[0x95] = [](cpu* c) { c->store(c->a, mode::zpx); };
    m_op_table[0x81] = [](cpu* c) { c->store(c->a, mode::indx); };
    m_op_table[0x91] = [](cpu* c) { c->store(c->a, mode::indy); };
    m_op_table[0x8D] = [](cpu* c) { c->store(c->a, mode::abs); };
    m_op_table[0x9D] = [](cpu* c) { c->store(c->a, mode::absx); };
    m_op_table[0x99] = [](cpu* c) { c->store(c->a, mode::absy); };

    // STX
    m_op_table[0x86] = [](cpu* c) { c->store(c->x, mode::zp); };
    m_op_table[0x96] = [](cpu* c) { c->store(c->x, mode::zpy); };
    m_op_table[0x8E] = [](cpu* c) { c->store(c->x, mode::abs); };

    // STY
    m_op_table[0x84] = [](cpu* c) { c->store(c->y, mode::zp); };
    m_op_table[0x94] = [](cpu* c) { c->store(c->y, mode::zpx); };
    m_op_table[0x8C] = [](cpu* c) { c->store(c->y, mode::abs); };

    // Transfers
    m_op_table[0xAA] = [](cpu* c) { c->tax(); };
    m_op_table[0x8A] = [](cpu* c) { c->txa(); };
    m_op_table[0xA8] = [](cpu* c) { c->tay(); };
    m_op_table[0x98] = [](cpu* c) { c->tya(); };
    m_op_table[0xBA] = [](cpu* c) { c->tsx(); };
    m_op_table[0x9A] = [](cpu* c) { c->txs(); };

    // Stack
    m_op_table[0x68] = [](cpu* c) { c->pla(); };
    m_op_table[0x48] = [](cpu* c) { c->pha(); };
    m_op_table[0x28] = [](cpu* c) { c->plp(); };
    m_op_table[0x08] = [](cpu* c) { c->php(); };

    //
    // JUMP/FLAG
    //

    // Branch
    m_op_table[0x10] = [](cpu* c) { c->branch(!c->m_status.flags.negative); };
    m_op_table[0x30] = [](cpu* c) { c->branch(c->m_status.flags.negative); };
    m_op_table[0x50] = [](cpu* c) { c->branch(!c->m_status.flags.overflow); };
    m_op_table[0x70] = [](cpu* c) { c->branch(c->m_status.flags.overflow); };
    m_op_table[0x90] = [](cpu* c) { c->branch(!c->m_status.flags.carry); };
    m_op_table[0xB0] = [](cpu* c) { c->branch(c->m_status.flags.carry); };
    m_op_table[0xD0] = [](cpu* c) { c->branch(!c->m_status.flags.zero); };
    m_op_table[0xF0] = [](cpu* c) { c->branch(c->m_status.flags.zero); };

    // Interrupts
    m_op_table[0x00] = [](cpu* c) { c->brk(); };
    m_op_table[0x20] = [](cpu* c) { c->jsr(); };
    m_op_table[0x40] = [](cpu* c) { c->rti(); };
    m_op_table[0x60] = [](cpu* c) { c->rts(); };

    // Jumps
    m_op_table[0x4C] = [](cpu* c) { c->jump(mode::abs); };
    m_op_table[0x6C] = [](cpu* c) { c->jump(mode::ind); };

    // Flag Stuff
    m_op_table[0xEA] = [](cpu* c) { c->nop(); };
    m_op_table[0x24] = [](cpu* c) { c->test_bit(mode::zp); };
    m_op_table[0x2C] = [](cpu* c) { c->test_bit(mode::abs); };
    m_op_table[0x18] = [](cpu* c) {
        c->nop();
        c->m_status.flags.carry = false;
    };
    m_op_table[0x38] = [](cpu* c) {
        c->nop();
        c->m_status.flags.carry = true;
    };
    m_op_table[0xD8] = [](cpu* c) {
        c->nop();
        c->m_status.flags.decimal = false;
    };
    m_op_table[0xF8] = [](cpu* c) {
        c->nop();
        c->m_status.flags.decimal = true;
    };
    m_op_table[0x58] = [](cpu* c) {
        c->nop();
        c->m_status.flags.interrupt_disable = false;
    };
    m_op_table[0x78] = [](cpu* c) {
        c->nop();
        c->m_status.flags.interrupt_disable = true;
    };
    m_op_table[0xB8] = [](cpu* c) {
        c->nop();
        c->m_status.flags.overflow = false;
    };

    //
    // Logic & Math
    //

    // ORA
    m_op_table[0x09] = [](cpu* c) { c->a = c->_or(c->a, mode::imm); };
    m_op_table[0x05] = [](cpu* c) { c->a = c->_or(c->a, mode::zp); };
    m_op_table[0x15] = [](cpu* c) { c->a = c->_or(c->a, mode::zpx); };
    m_op_table[0x01] = [](cpu* c) { c->a = c->_or(c->a, mode::indx); };
    m_op_table[0x11] = [](cpu* c) { c->a = c->_or(c->a, mode::indy); };
    m_op_table[0x0D] = [](cpu* c) { c->a = c->_or(c->a, mode::abs); };
    m_op_table[0x1D] = [](cpu* c) { c->a = c->_or(c->a, mode::absx); };
    m_op_table[0x19] = [](cpu* c) { c->a = c->_or(c->a, mode::absy); };

    // AND
    m_op_table[0x29] = [](cpu* c) { c->a = c->_and(c->a, mode::imm); };
    m_op_table[0x25] = [](cpu* c) { c->a = c->_and(c->a, mode::zp); };
    m_op_table[0x35] = [](cpu* c) { c->a = c->_and(c->a, mode::zpx); };
    m_op_table[0x21] = [](cpu* c) { c->a = c->_and(c->a, mode::indx); };
    m_op_table[0x31] = [](cpu* c) { c->a = c->_and(c->a, mode::indy); };
    m_op_table[0x2D] = [](cpu* c) { c->a = c->_and(c->a, mode::abs); };
    m_op_table[0x3D] = [](cpu* c) { c->a = c->_and(c->a, mode::absx); };
    m_op_table[0x39] = [](cpu* c) { c->a = c->_and(c->a, mode::absy); };

    // EOR
    m_op_table[0x49] = [](cpu* c) { c->a = c->_xor(c->a, mode::imm); };
    m_op_table[0x45] = [](cpu* c) { c->a = c->_xor(c->a, mode::zp); };
    m_op_table[0x55] = [](cpu* c) { c->a = c->_xor(c->a, mode::zpx); };
    m_op_table[0x41] = [](cpu* c) { c->a = c->_xor(c->a, mode::indx); };
    m_op_table[0x51] = [](cpu* c) { c->a = c->_xor(c->a, mode::indy); };
    m_op_table[0x4D] = [](cpu* c) { c->a = c->_xor(c->a, mode::abs); };
    m_op_table[0x5D] = [](cpu* c) { c->a = c->_xor(c->a, mode::absx); };
    m_op_table[0x59] = [](cpu* c) { c->a = c->_xor(c->a, mode::absy); };

    // ADC
    m_op_table[0x69] = [](cpu* c) { c->a = c->add(c->a, mode::imm); };
    m_op_table[0x65] = [](cpu* c) { c->a = c->add(c->a, mode::zp); };
    m_op_table[0x75] = [](cpu* c) { c->a = c->add(c->a, mode::zpx); };
    m_op_table[0x61] = [](cpu* c) { c->a = c->add(c->a, mode::indx); };
    m_op_table[0x71] = [](cpu* c) { c->a = c->add(c->a, mode::indy); };
    m_op_table[0x6D] = [](cpu* c) { c->a = c->add(c->a, mode::abs); };
    m_op_table[0x7D] = [](cpu* c) { c->a = c->add(c->a, mode::absx); };
    m_op_table[0x79] = [](cpu* c) { c->a = c->add(c->a, mode::absy); };

    // SBC
    m_op_table[0xE9] = [](cpu* c) { c->a = c->sub(c->a, mode::imm); };
    m_op_table[0xE5] = [](cpu* c) { c->a = c->sub(c->a, mode::zp); };
    m_op_table[0xF5] = [](cpu* c) { c->a = c->sub(c->a, mode::zpx); };
    m_op_table[0xE1] = [](cpu* c) { c->a = c->sub(c->a, mode::indx); };
    m_op_table[0xF1] = [](cpu* c) { c->a = c->sub(c->a, mode::indy); };
    m_op_table[0xED] = [](cpu* c) { c->a = c->sub(c->a, mode::abs); };
    m_op_table[0xFD] = [](cpu* c) { c->a = c->sub(c->a, mode::absx); };
    m_op_table[0xF9] = [](cpu* c) { c->a = c->sub(c->a, mode::absy); };

    // CMP
    m_op_table[0xC9] = [](cpu* c) { c->cmp(c->a, mode::imm); };
    m_op_table[0xC5] = [](cpu* c) { c->cmp(c->a, mode::zp); };
    m_op_table[0xD5] = [](cpu* c) { c->cmp(c->a, mode::zpx); };
    m_op_table[0xC1] = [](cpu* c) { c->cmp(c->a, mode::indx); };
    m_op_table[0xD1] = [](cpu* c) { c->cmp(c->a, mode::indy); };
    m_op_table[0xCD] = [](cpu* c) { c->cmp(c->a, mode::abs); };
    m_op_table[0xDD] = [](cpu* c) { c->cmp(c->a, mode::absx); };
    m_op_table[0xD9] = [](cpu* c) { c->cmp(c->a, mode::absy); };

    // CPX
    m_op_table[0xE0] = [](cpu* c) { c->cmp(c->x, mode::imm); };
    m_op_table[0xE4] = [](cpu* c) { c->cmp(c->x, mode::zp); };
    m_op_table[0xEC] = [](cpu* c) { c->cmp(c->x, mode::abs); };

    // CPY
    m_op_table[0xC0] = [](cpu* c) { c->cmp(c->y, mode::imm); };
    m_op_table[0xC4] = [](cpu* c) { c->cmp(c->y, mode::zp); };
    m_op_table[0xCC] = [](cpu* c) { c->cmp(c->y, mode::abs); };

    // DEC
    m_op_table[0xC6] = [](cpu* c) { c->dec_addr(mode::zp); };
    m_op_table[0xD6] = [](cpu* c) { c->dec_addr(mode::zpx); };
    m_op_table[0xCE] = [](cpu* c) { c->dec_addr(mode::abs); };
    m_op_table[0xDE] = [](cpu* c) { c->dec_addr(mode::absx); };

    // DEX, DEY
    m_op_table[0xCA] = [](cpu* c) { c->x = c->dec(c->x); };
    m_op_table[0x88] = [](cpu* c) { c->y = c->dec(c->y); };

    // INC
    m_op_table[0xE6] = [](cpu* c) { c->inc_addr(mode::zp); };
    m_op_table[0xF6] = [](cpu* c) { c->inc_addr(mode::zpx); };
    m_op_table[0xEE] = [](cpu* c) { c->inc_addr(mode::abs); };
    m_op_table[0xFE] = [](cpu* c) { c->inc_addr(mode::absx); };

    // INX, INY
    m_op_table[0xE8] = [](cpu* c) { c->x = c->inc(c->x); };
    m_op_table[0xC8] = [](cpu* c) { c->y = c->inc(c->y); };

    // ASL
    m_op_table[0x0A] = [](cpu* c) { c->a = c->asl(c->a); };
    m_op_table[0x06] = [](cpu* c) { c->asl_addr(mode::zp); };
    m_op_table[0x16] = [](cpu* c) { c->asl_addr(mode::zpx); };
    m_op_table[0x0E] = [](cpu* c) { c->asl_addr(mode::abs); };
    m_op_table[0x1E] = [](cpu* c) { c->asl_addr(mode::absx); };

    // ROL
    m_op_table[0x2A] = [](cpu* c) { c->a = c->rol(c->a); };
    m_op_table[0x26] = [](cpu* c) { c->rol_addr(mode::zp); };
    m_op_table[0x36] = [](cpu* c) { c->rol_addr(mode::zpx); };
    m_op_table[0x2E] = [](cpu* c) { c->rol_addr(mode::abs); };
    m_op_table[0x3E] = [](cpu* c) { c->rol_addr(mode::absx); };

    // LSR
    m_op_table[0x4A] = [](cpu* c) { c->a = c->lsr(c->a); };
    m_op_table[0x46] = [](cpu* c) { c->lsr_addr(mode::zp); };
    m_op_table[0x56] = [](cpu* c) { c->lsr_addr(mode::zpx); };
    m_op_table[0x4E] = [](cpu* c) { c->lsr_addr(mode::abs); };
    m_op_table[0x5E] = [](cpu* c) { c->lsr_addr(mode::absx); };

    // ROR
    m_op_table[0x6A] = [](cpu* c) { c->a = c->ror(c->a); };
    m_op_table[0x66] = [](cpu* c) { c->ror_addr(mode::zp); };
    m_op_table[0x76] = [](cpu* c) { c->ror_addr(mode::zpx); };
    m_op_table[0x6E] = [](cpu* c) { c->ror_addr(mode::abs); };
    m_op_table[0x7E] = [](cpu* c) { c->ror_addr(mode::absx); };

    //
    // Unofficial
    //

    // NOPs
    //m_op_table[0x04] = [](cpu* c) { c->nop(3); };
    //m_op_table[0x44] = [](cpu* c) { c->nop(3); };
    //m_op_table[0x64] = [](cpu* c) { c->nop(3); };

    //m_op_table[0x0C] = [](cpu* c) { c->nop(4); };
}

} // namespace cx::nes
