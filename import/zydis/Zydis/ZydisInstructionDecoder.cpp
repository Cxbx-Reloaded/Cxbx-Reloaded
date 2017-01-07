// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/***************************************************************************************************

  Zyan Disassembler Engine
  Version 1.0

  Remarks         : Freeware, Copyright must be included

  Original Author : Florian Bernd
  Modifications   : Joel Höner

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

***************************************************************************************************/

#include "ZydisInstructionDecoder.hpp"
#include <cstring>

namespace Zydis
{

/* InstructionDecoder ====================================================================== */

bool InstructionDecoder::decodeRegisterOperand(InstructionInfo& info, 
    OperandInfo& operand, RegisterClass registerClass, uint8_t registerId, 
    DefinedOperandSize operandSize) const
{
    Register reg = Register::NONE;
    uint16_t size = getEffectiveOperandSize(info, operandSize);
    switch (registerClass)
    {
    case RegisterClass::GENERAL_PURPOSE: 
        switch (size) 
        {
        case 64:
            reg = static_cast<Register>(
                static_cast<uint16_t>(Register::RAX) + registerId);
            break;
        case 32:
            reg = static_cast<Register>(
                static_cast<uint16_t>(Register::EAX) + registerId);
            break;
        case 16:
            reg = static_cast<Register>(
                static_cast<uint16_t>(Register::AX) + registerId);
            break;
        case 8:
            // TODO: Only REX? Or VEX too?
            if (m_disassemblerMode == DisassemblerMode::M64BIT && (info.flags & IF_PREFIX_REX)) 
            {
                if (registerId >= 4)
                {
                    reg = static_cast<Register>(
                        static_cast<uint16_t>(Register::SPL) + (registerId - 4));
                } else
                {
                    reg = static_cast<Register>(
                        static_cast<uint16_t>(Register::AL) + registerId);
                }
            } else 
            {
                reg = static_cast<Register>(
                    static_cast<uint16_t>(Register::AL) + registerId);
            }
            break;
        case 0:
            // TODO: Error?
            reg = Register::NONE;
            break;
        default:
            assert(0);
        }
        break;
    case RegisterClass::MMX: 
        reg = static_cast<Register>(
            static_cast<uint16_t>(Register::MM0) + (registerId & 0x07));
        break;
    case RegisterClass::CONTROL: 
        reg = static_cast<Register>(static_cast<uint16_t>(Register::CR0) + registerId);
        break;
    case RegisterClass::DEBUG: 
        reg = static_cast<Register>(static_cast<uint16_t>(Register::DR0) + registerId);
        break;
    case RegisterClass::SEGMENT: 
        if ((registerId & 7) > 5) 
        {
            info.flags |= IF_ERROR_OPERAND;
            return false;
        }
        reg = static_cast<Register>(
            static_cast<uint16_t>(Register::ES) + (registerId & 0x07));
        break;
    case RegisterClass::XMM:  
        reg = static_cast<Register>(registerId + static_cast<uint16_t>(
            ((size == 256) ? Register::YMM0 : Register::XMM0)));
        break;
    default: 
        assert(0);
    }
    operand.type = OperandType::REGISTER;
    operand.base = static_cast<Register>(reg);
    operand.size = size;
    return true;
}

bool InstructionDecoder::decodeRegisterMemoryOperand(InstructionInfo& info, 
    OperandInfo& operand, RegisterClass registerClass, DefinedOperandSize operandSize)
{
    if (!decodeModrm(info))
    {
        return false;
    }
    assert(info.flags & IF_MODRM);
    // Decode register operand
    if (info.modrm_mod == 3)
    {
        return decodeRegisterOperand(info, operand, registerClass, info.modrm_rm_ext, 
            operandSize);
    }
    // Decode memory operand
    uint8_t offset = 0;
    operand.type = OperandType::MEMORY;
    operand.size = getEffectiveOperandSize(info, operandSize);
    switch (info.address_mode)
    {
    case 16:
        {
            static const Register bases[] = { 
                Register::BX, Register::BX, Register::BP, Register::BP, 
                Register::SI, Register::DI, Register::BP, Register::BX };
            static const Register indices[] = { 
                Register::SI, Register::DI, Register::SI, Register::DI,
                Register::NONE, Register::NONE, Register::NONE, 
                Register::NONE };
            operand.base = static_cast<Register>(bases[info.modrm_rm_ext & 0x07]);
            operand.index = static_cast<Register>(indices[info.modrm_rm_ext & 0x07]);
            operand.scale = 0;
            if (info.modrm_mod == 0 && info.modrm_rm_ext == 6) {
                offset = 16;
                operand.base = Register::NONE;
            } else if (info.modrm_mod == 1) {
                offset = 8;
            } else if (info.modrm_mod == 2) {
                offset = 16;
            }
        }
        break;
    case 32:
        operand.base = static_cast<Register>(
            static_cast<uint16_t>(Register::EAX) + info.modrm_rm_ext);
        switch (info.modrm_mod)
        {
        case 0:
            if (info.modrm_rm_ext == 5)
            {
                operand.base = Register::NONE;
                offset = 32;
            }
            break;
        case 1:
            offset = 8;
            break;
        case 2:
            offset = 32;
            break;
        default:
            assert(0);
        }
        if ((info.modrm_rm_ext & 0x07) == 4)
        {
            if (!decodeSIB(info))
            {
                return false;
            }
            operand.base = 
                static_cast<Register>(static_cast<uint16_t>(Register::EAX) + 
                info.sib_base_ext);
            operand.index = 
                static_cast<Register>(static_cast<uint16_t>(Register::EAX) + 
                info.sib_index_ext);
            operand.scale = (1 << info.sib_scale) & ~1;
            if (operand.index == Register::ESP)  
            {
                operand.index = Register::NONE;
                operand.scale = 0;
            } 
            if (operand.base == Register::EBP)
            {
                if (info.modrm_mod == 0)
                {
                    operand.base = Register::NONE;
                } 
                if (info.modrm_mod == 1)
                {
                    offset = 8;
                } else
                {
                    offset = 32;
                }
            }
        } else
        {
            operand.index = Register::NONE;
            operand.scale = 0;    
        }
        break;
    case 64:
        operand.base = static_cast<Register>(
            static_cast<uint16_t>(Register::RAX) + info.modrm_rm_ext);
        switch (info.modrm_mod)
        {
        case 0:
            if ((info.modrm_rm_ext & 0x07) == 5)
            {
                info.flags |= IF_RELATIVE;
                operand.base = Register::RIP;
                offset = 32;
            }
            break;
        case 1:
            offset = 8;
            break;
        case 2:
            offset = 32;
            break;
        default:
            assert(0);
        }
        if ((info.modrm_rm_ext & 0x07) == 4)
        {
            if (!decodeSIB(info))
            {
                return false;
            }
            operand.base = 
                static_cast<Register>(static_cast<uint16_t>(Register::RAX) + 
                info.sib_base_ext);
            operand.index = 
                static_cast<Register>(static_cast<uint16_t>(Register::RAX) + 
                info.sib_index_ext);
            if (operand.index == Register::RSP) 
            {
                operand.index = Register::NONE;
                operand.scale = 0;
            } else
            {
                operand.scale = (1 << info.sib_scale) & ~1;
            }
            if ((operand.base == Register::RBP) || (operand.base == Register::R13))
            {
                if (info.modrm_mod == 0)
                {
                    operand.base = Register::NONE;
                }
                if (info.modrm_mod == 1)
                {
                    offset = 8;    
                } else
                {
                    offset = 32;    
                }
            }
        } else
        {
            operand.index = Register::NONE;
            operand.scale = 0;
        }
        break;
    }
    if (offset)
    {
        if (!decodeDisplacement(info, operand, offset))
        {
            return false;
        }
    } else
    {
        operand.offset = 0;
    }
    return true;
}

bool InstructionDecoder::decodeImmediate(InstructionInfo& info, OperandInfo& operand, 
    DefinedOperandSize operandSize)
{
    operand.type = OperandType::IMMEDIATE;
    operand.size = getEffectiveOperandSize(info, operandSize);
    switch (operand.size) 
    {
        case 8: 
            operand.lval.ubyte = inputNext(info); 
            break;
        case 16: 
            operand.lval.uword = inputNext<uint16_t>(info); 
            break;
        case 32: 
            operand.lval.udword = inputNext<uint32_t>(info); 
            break;
        case 64: 
            operand.lval.uqword = inputNext<uint64_t>(info); 
            break;
        default: 
            // TODO: Maybe return false instead of assert
            assert(0);
    }
    if (!operand.lval.uqword && (info.flags & IF_ERROR_MASK))
    {
        return false;
    }
    return true;
}

bool InstructionDecoder::decodeDisplacement(InstructionInfo& info, 
    OperandInfo& operand, uint8_t size)
{
    switch (size)
    {
    case 8:
        operand.offset = 8;
        operand.lval.ubyte = inputNext(info);
        break;
    case 16:
        operand.offset = 16;
        operand.lval.uword = inputNext<uint16_t>(info);
        break;
    case 32:
        operand.offset = 32;
        operand.lval.udword = inputNext<uint32_t>(info);
        break;
    case 64:
        operand.offset = 64;
        operand.lval.uqword = inputNext<uint64_t>(info);
        break;
    default:
        // TODO: Maybe return false instead of assert
        assert(0);
    }
    if (!operand.lval.uqword && (info.flags & IF_ERROR_MASK))
    {
        return false;
    }
    return true;
}

bool InstructionDecoder::decodeModrm(InstructionInfo& info)
{
    if (!(info.flags & IF_MODRM))
    {
        info.modrm = inputNext(info);
        if (!info.modrm && (info.flags & IF_ERROR_MASK))
        {
            return false;
        }
        info.flags |= IF_MODRM;
        info.modrm_mod = (info.modrm >> 6) & 0x03;
        info.modrm_reg = (info.modrm >> 3) & 0x07;
        info.modrm_rm  = (info.modrm >> 0) & 0x07;
    }
    // The @c decodeModrm method might get called multiple times during the opcode- and the
    // operand decoding, but the effective REX/VEX fields are not initialized before the end of  
    // the opcode decoding process. As the extended values are only used for the operand decoding,
    // we should have no problems.
    info.modrm_reg_ext = (info.eff_rexvex_r << 3) | info.modrm_reg;
    info.modrm_rm_ext  = (info.eff_rexvex_b << 3) | info.modrm_rm;
    return true;
}

bool InstructionDecoder::decodeSIB(InstructionInfo& info)
{
    assert(info.flags & IF_MODRM);
    assert((info.modrm_rm & 0x7) == 4);
    if (!(info.flags & IF_SIB))
    {
        info.sib = inputNext(info);
        if (!info.sib && (info.flags & IF_ERROR_MASK))
        {
            return false;
        }
        info.flags |= IF_SIB;
        info.sib_scale  = (info.sib >> 6) & 0x03;
        info.sib_index  = (info.sib >> 3) & 0x07;
        info.sib_base   = (info.sib >> 0) & 0x07;
        // The @c decodeSib method is only called during the operand decoding, so updating the
        // extended values at this point should be safe.
        info.sib_index_ext = (info.eff_rexvex_x << 3) | info.sib_index;
        info.sib_base_ext  = (info.eff_rexvex_b << 3) | info.sib_base;
    }
    return true;
}

bool InstructionDecoder::decodeVex(InstructionInfo& info)
{
    if (!(info.flags & IF_PREFIX_VEX))
    {
        info.vex_op = inputCurrent();
        switch (info.vex_op)
        {
        case 0xC4:
            info.vex_b1 = inputNext(info);
            if (!info.vex_b1 || (info.flags & IF_ERROR_MASK))
            {
                return false;
            }
            info.vex_b2 = inputNext(info);
            if (!info.vex_b2 || (info.flags & IF_ERROR_MASK))
            {
                return false;
            }
            info.vex_r      = (info.vex_b1 >> 7) & 0x01;
            info.vex_x      = (info.vex_b1 >> 6) & 0x01;
            info.vex_b      = (info.vex_b1 >> 5) & 0x01;
            info.vex_m_mmmm = (info.vex_b1 >> 0) & 0x1F;
            info.vex_w      = (info.vex_b2 >> 7) & 0x01;
            info.vex_vvvv   = (info.vex_b2 >> 3) & 0x0F;
            info.vex_l      = (info.vex_b2 >> 2) & 0x01;
            info.vex_pp     = (info.vex_b2 >> 0) & 0x03;
            break;
        case 0xC5:
            info.vex_b1 = inputNext(info);
            if (!info.vex_b1 || (info.flags & IF_ERROR_MASK))
            {
                return false;
            }
            info.vex_r      = (info.vex_b1 >> 7) & 0x01;
            info.vex_x      = 1;
            info.vex_b      = 1;
            info.vex_m_mmmm = 1;
            info.vex_w      = 0;
            info.vex_vvvv   = (info.vex_b1 >> 3) & 0x0F;
            info.vex_l      = (info.vex_b1 >> 2) & 0x01;
            info.vex_pp     = (info.vex_b1 >> 0) & 0x03;
            break;
        default:
            assert(0);
        }
        if (info.vex_m_mmmm > 3)
        {
            // TODO: Add proper error flag
            info.flags |= IF_ERROR_MASK;
            return false;
        }
        info.flags |= IF_PREFIX_VEX;
    }
    return true;
}

uint16_t InstructionDecoder::getEffectiveOperandSize(const InstructionInfo& info, 
    DefinedOperandSize operandSize) const
{
    switch (operandSize)
    {
    case DefinedOperandSize::NA: 
        return 0;
    case DefinedOperandSize::Z: 
        return (info.operand_mode == 16) ? 16 : 32;
    case DefinedOperandSize::V: 
        return info.operand_mode;
    case DefinedOperandSize::Y: 
        return (info.operand_mode == 16) ? 32 : info.operand_mode;
    case DefinedOperandSize::X: 
        assert(info.vex_op != 0);
        return (info.eff_vex_l) ? 
            getEffectiveOperandSize(info, DefinedOperandSize::QQ) : 
            getEffectiveOperandSize(info, DefinedOperandSize::DQ);
    case DefinedOperandSize::RDQ: 
        return (m_disassemblerMode == DisassemblerMode::M64BIT) ? 64 : 32;
    default: 
        return Internal::GetSimpleOperandSize(operandSize);
    }
}

bool InstructionDecoder::decodeOperands(InstructionInfo& info)
{
    assert(info.instrDefinition);
    // Always try to decode the first operand
    if (!decodeOperand(info, info.operand[0], info.instrDefinition->operand[0].type, 
        info.instrDefinition->operand[0].size))
    {
        return false;
    }
    // Decode other operands on demand
    for (unsigned int i = 1; i < 4; ++i)
    {
        if (info.operand[i - 1].type != OperandType::NONE)
        {
            if (!decodeOperand(info, info.operand[i], info.instrDefinition->operand[i].type, 
                info.instrDefinition->operand[i].size))
            {
                return false;
            }
        }    
    }
    // Update operand access modes
    for (unsigned int i = 0; i < 4; ++i)
    {
        if (info.operand[i].type != OperandType::NONE)
        {
            info.operand[i].access_mode = OperandAccessMode::READ;
            if (i == 0)
            {
                if (info.instrDefinition->flags & IDF_OPERAND1_WRITE)
                {
                    info.operand[0].access_mode = OperandAccessMode::WRITE;
                } else if (info.instrDefinition->flags & IDF_OPERAND1_READWRITE)
                {
                    info.operand[0].access_mode = OperandAccessMode::READWRITE;
                }
            } else if (i == 1)
            {
                if (info.instrDefinition->flags & IDF_OPERAND2_WRITE)
                {
                    info.operand[1].access_mode = OperandAccessMode::WRITE;
                } else if (info.instrDefinition->flags & IDF_OPERAND2_READWRITE)
                {
                    info.operand[1].access_mode = OperandAccessMode::READWRITE;
                }    
            }
        }
    }
    return true;
}

bool InstructionDecoder::decodeOperand(InstructionInfo& info, OperandInfo& operand, 
    DefinedOperandType operandType, DefinedOperandSize operandSize)
{
    using namespace Internal;
    operand.type = OperandType::NONE;
    switch (operandType)
    {
    case DefinedOperandType::NONE:
        break;
    case DefinedOperandType::A: 
        operand.type = OperandType::POINTER;
        if (info.operand_mode == 16)
        {
            operand.size = 32;
            operand.lval.ptr.off = inputNext<uint16_t>(info);
            operand.lval.ptr.seg = inputNext<uint16_t>(info);
        } else {
            operand.size = 48;
            operand.lval.ptr.off = inputNext<uint32_t>(info);
            operand.lval.ptr.seg = inputNext<uint16_t>(info);
        }
        if ((!operand.lval.ptr.off || !operand.lval.ptr.seg) && (info.flags & IF_ERROR_MASK))
        {
            return false;
        }
        break;
    case DefinedOperandType::C: 
        if (!decodeModrm(info))
        {
            return false;
        }
        return decodeRegisterOperand(info, operand, RegisterClass::CONTROL, info.modrm_reg_ext, 
            operandSize);
    case DefinedOperandType::D: 
        if (!decodeModrm(info))
        {
            return false;
        }
        return decodeRegisterOperand(info, operand, RegisterClass::DEBUG, info.modrm_reg_ext, 
            operandSize);
    case DefinedOperandType::F: 
        // TODO: FAR flag
    case DefinedOperandType::M: 
        // ModR/M byte may refer only to memory
        if (info.modrm_mod == 3)
        {
            info.flags |= IF_ERROR_OPERAND;
            return false;
        }
    case DefinedOperandType::E: 
        return decodeRegisterMemoryOperand(info, operand, RegisterClass::GENERAL_PURPOSE, 
            operandSize);
    case DefinedOperandType::G: 
        if (!decodeModrm(info))
        {
            return false;
        }
        return decodeRegisterOperand(info, operand, RegisterClass::GENERAL_PURPOSE, 
            info.modrm_reg_ext, operandSize);
    case DefinedOperandType::H: 
        assert(info.vex_op != 0);
        return decodeRegisterOperand(info, operand, RegisterClass::XMM, (0xF & ~info.vex_vvvv), 
            operandSize);
    case DefinedOperandType::sI:
        operand.signed_lval = true;
    case DefinedOperandType::I: 
        return decodeImmediate(info, operand, operandSize);
    case DefinedOperandType::I1: 
        operand.type = OperandType::CONSTANT;
        operand.lval.udword = 1;
        break;
    case DefinedOperandType::J: 
        if (!decodeImmediate(info, operand, operandSize))
        {
            return false;
        }
        operand.type = OperandType::REL_IMMEDIATE;
        operand.signed_lval = true;
        info.flags |= IF_RELATIVE;
        break;
    case DefinedOperandType::L: 
        {
            assert(info.vex_op != 0);
            uint8_t imm = inputNext(info);
            if (!imm && (info.flags & IF_ERROR_MASK))
            {
                return false;
            }
            uint8_t mask = (m_disassemblerMode == DisassemblerMode::M64BIT) ? 0xF : 0x7;
            return decodeRegisterOperand(info, operand, RegisterClass::XMM, mask & (imm >> 4), 
                operandSize);
        }
    case DefinedOperandType::MR: 
        return decodeRegisterMemoryOperand(info, operand, RegisterClass::GENERAL_PURPOSE, 
            info.modrm_mod == 3 ? 
            GetComplexOperandRegSize(operandSize) : 
            GetComplexOperandMemSize(operandSize));
    case DefinedOperandType::MU: 
        return decodeRegisterMemoryOperand(info, operand, RegisterClass::XMM, 
            info.modrm_mod == 3 ? 
            GetComplexOperandRegSize(operandSize) : 
            GetComplexOperandMemSize(operandSize));
    case DefinedOperandType::N: 
        // ModR/M byte may refer only to a register
        if (info.modrm_mod != 3)
        {
            info.flags |= IF_ERROR_OPERAND;
            return false;
        }
    case DefinedOperandType::Q: 
        return decodeRegisterMemoryOperand(info, operand, RegisterClass::MMX, operandSize);
    case DefinedOperandType::O: 
        operand.type = OperandType::MEMORY;
        operand.base = Register::NONE;
        operand.index = Register::NONE;
        operand.scale = 0;
        operand.size = getEffectiveOperandSize(info, operandSize);
        return decodeDisplacement(info, operand, info.address_mode);
    case DefinedOperandType::P: 
        if (!decodeModrm(info))
        {
            return false;
        }
        return decodeRegisterOperand(info, operand, RegisterClass::MMX, info.modrm_reg_ext, 
            operandSize);
    case DefinedOperandType::R: 
        // ModR/M byte may refer only to memory
        if (info.modrm_mod != 3)
        {
            info.flags |= IF_ERROR_OPERAND;
            return false;
        }
        return decodeRegisterMemoryOperand(info, operand, RegisterClass::GENERAL_PURPOSE, 
            operandSize);
    case DefinedOperandType::S: 
        if (!decodeModrm(info))
        {
            return false;
        }
        return decodeRegisterOperand(info, operand, RegisterClass::SEGMENT, info.modrm_reg_ext, 
            operandSize);
    case DefinedOperandType::U: 
        // ModR/M byte may refer only to memory
        if (info.modrm_mod != 3)
        {
            info.flags |= IF_ERROR_OPERAND;
            return false;
        }
     case DefinedOperandType::W: 
        return decodeRegisterMemoryOperand(info, operand, RegisterClass::XMM, operandSize);
    case DefinedOperandType::V: 
        if (!decodeModrm(info))
        {
            return false;
        }
        return decodeRegisterOperand(info, operand, RegisterClass::XMM, info.modrm_reg_ext, 
            operandSize);
    case DefinedOperandType::R0: 
    case DefinedOperandType::R1: 
    case DefinedOperandType::R2: 
    case DefinedOperandType::R3: 
    case DefinedOperandType::R4: 
    case DefinedOperandType::R5: 
    case DefinedOperandType::R6: 
    case DefinedOperandType::R7: 
        return decodeRegisterOperand(info, operand, RegisterClass::GENERAL_PURPOSE, 
            ((info.eff_rexvex_b << 3) | (static_cast<uint16_t>(operandType) - 
            static_cast<uint16_t>(DefinedOperandType::R0))), operandSize);
    case DefinedOperandType::AL: 
    case DefinedOperandType::AX: 
    case DefinedOperandType::EAX: 
    case DefinedOperandType::RAX: 
        return decodeRegisterOperand(info, operand, RegisterClass::GENERAL_PURPOSE, 0, 
            operandSize);
    case DefinedOperandType::CL: 
    case DefinedOperandType::CX: 
    case DefinedOperandType::ECX: 
    case DefinedOperandType::RCX: 
        return decodeRegisterOperand(info, operand, RegisterClass::GENERAL_PURPOSE, 1, 
            operandSize);
    case DefinedOperandType::DL: 
    case DefinedOperandType::DX: 
    case DefinedOperandType::EDX: 
    case DefinedOperandType::RDX: 
        return decodeRegisterOperand(info, operand, RegisterClass::GENERAL_PURPOSE, 2, 
            operandSize);
    case DefinedOperandType::ES: 
    case DefinedOperandType::CS: 
    case DefinedOperandType::SS: 
    case DefinedOperandType::DS: 
    case DefinedOperandType::FS: 
    case DefinedOperandType::GS: 
        if (m_disassemblerMode == DisassemblerMode::M64BIT)
        {
            if ((operandType != DefinedOperandType::FS) && 
                (operandType != DefinedOperandType::GS))
            {
                info.flags |= IF_ERROR_OPERAND;
                return false;
            }
        }
        operand.type = OperandType::REGISTER;
        operand.base = static_cast<Register>((static_cast<uint16_t>(operandType) - 
            static_cast<uint16_t>(DefinedOperandType::ES)) + 
            static_cast<uint16_t>(Register::ES));
        operand.size = 16;
        break;
    case DefinedOperandType::ST0: 
    case DefinedOperandType::ST1: 
    case DefinedOperandType::ST2: 
    case DefinedOperandType::ST3: 
    case DefinedOperandType::ST4: 
    case DefinedOperandType::ST5: 
    case DefinedOperandType::ST6: 
    case DefinedOperandType::ST7: 
        operand.type = OperandType::REGISTER;
        operand.base = static_cast<Register>((static_cast<uint16_t>(operandType) - 
            static_cast<uint16_t>(DefinedOperandType::ST0)) + 
            static_cast<uint16_t>(Register::ST0));
        operand.size = 80;
        break;
    default: 
        assert(0);
    }
    return true;
}

void InstructionDecoder::resolveOperandAndAddressMode(InstructionInfo& info) const
{
    assert(info.instrDefinition);
    switch (m_disassemblerMode)
    {
    case DisassemblerMode::M16BIT:
        info.operand_mode = (info.flags & IF_PREFIX_OPERAND_SIZE) ? 32 : 16;
        info.address_mode = (info.flags & IF_PREFIX_ADDRESS_SIZE) ? 32 : 16;
        break;
    case DisassemblerMode::M32BIT:
        info.operand_mode = (info.flags & IF_PREFIX_OPERAND_SIZE) ? 16 : 32;
        info.address_mode = (info.flags & IF_PREFIX_ADDRESS_SIZE) ? 16 : 32;
        break;
    case DisassemblerMode::M64BIT:
        if (info.eff_rexvex_w)
        {
            info.operand_mode = 64;
        } else if ((info.flags & IF_PREFIX_OPERAND_SIZE))
        {
            info.operand_mode = 16;
        } else
        {
            info.operand_mode = (info.instrDefinition->flags & IDF_DEFAULT_64) ? 64 : 32;
        }
        info.address_mode = (info.flags & IF_PREFIX_ADDRESS_SIZE) ? 32 : 64;
        break;
    default: 
        assert(0);
    }
}

void InstructionDecoder::calculateEffectiveRexVexValues(InstructionInfo& info) const
{
    assert(info.instrDefinition);
    uint8_t rex = info.rex;
    if (info.flags & IF_PREFIX_VEX)
    {
        switch (info.vex_op)
        {
        case 0xC4:
            rex = ((~(info.vex_b1 >> 5) & 0x07) | ((info.vex_b2 >> 4) & 0x08));
            break;
        case 0xC5:
            rex = (~(info.vex_b1 >> 5)) & 4;
            break;
        default:
            assert(0);
        }    
    }
    rex &= (info.instrDefinition->flags & 0x000F);
    info.eff_rexvex_w = (rex >> 3) & 0x01;
    info.eff_rexvex_r = (rex >> 2) & 0x01;
    info.eff_rexvex_x = (rex >> 1) & 0x01;
    info.eff_rexvex_b = (rex >> 0) & 0x01;
    info.eff_vex_l    = info.vex_l && (info.instrDefinition->flags & IDF_ACCEPTS_VEXL);
}

bool InstructionDecoder::decodePrefixes(InstructionInfo& info)
{
    bool done = false;
    do
    {
        switch (inputPeek(info))
        {
        case 0xF0:
            info.flags |= IF_PREFIX_LOCK;
            break;
        case 0xF2:
            // REPNZ and REPZ are mutally exclusive. The one that comes later has precedence.
            info.flags |= IF_PREFIX_REP;
            info.flags &= ~IF_PREFIX_REPNE;
            break;
        case 0xF3:
            // REPNZ and REPZ are mutally exclusive. The one that comes later has precedence.
            info.flags |= IF_PREFIX_REP;
            info.flags &= ~IF_PREFIX_REPNE;
            break;
        case 0x2E: 
            info.flags |= IF_PREFIX_SEGMENT;
            info.segment = Register::CS;
            break;
        case 0x36:
            info.flags |= IF_PREFIX_SEGMENT;
            info.segment = Register::SS;
            break;
        case 0x3E: 
            info.flags |= IF_PREFIX_SEGMENT;
            info.segment = Register::DS;
            break;
        case 0x26: 
            info.flags |= IF_PREFIX_SEGMENT;
            info.segment = Register::ES;
            break;
        case 0x64:
            info.flags |= IF_PREFIX_SEGMENT;
            info.segment = Register::FS;
            break;
        case 0x65: 
            info.flags |= IF_PREFIX_SEGMENT;
            info.segment = Register::GS;
            break;
        case 0x66:
            info.flags |= IF_PREFIX_OPERAND_SIZE;
            break;
        case 0x67:
            info.flags |= IF_PREFIX_ADDRESS_SIZE;
            break;
        default:
            if ((m_disassemblerMode == DisassemblerMode::M64BIT) && 
                (inputCurrent() & 0xF0) == 0x40)
            {
                info.flags |= IF_PREFIX_REX;
                info.rex = inputCurrent(); 
            } else
            {
                done = true;
            }
            break;
        }
        // Increase the input offset, if a prefix was found
        if (!done)
        {
            if (!inputNext(info) && (info.flags & IF_ERROR_MASK))
            {
                return false;
            }
        }
    } while (!done);
    // TODO: Check for multiple prefixes of the same group
    // Parse REX Prefix
    if (info.flags & IF_PREFIX_REX)
    {
        info.rex_w = (info.rex >> 3) & 0x01;
        info.rex_r = (info.rex >> 2) & 0x01;
        info.rex_x = (info.rex >> 1) & 0x01;
        info.rex_b = (info.rex >> 0) & 0x01;
    }
    return true;
}

bool InstructionDecoder::decodeOpcode(InstructionInfo& info)
{
    using namespace Internal;
    // Read first opcode byte
    if (!inputNext(info) && (info.flags & IF_ERROR_MASK))
    {
        return false;
    }
    // Update instruction info
    info.opcode[0] = inputCurrent();
    info.opcode_length = 1;
    // Iterate through opcode tree
    OpcodeTreeNode node = GetOpcodeTreeChild(GetOpcodeTreeRoot(), inputCurrent());
    OpcodeTreeNodeType nodeType;
    do
    {
        uint16_t index = 0;
        nodeType = GetOpcodeNodeType(node);
        switch (nodeType)
        {
        case OpcodeTreeNodeType::INSTRUCTION_DEFINITION: 
            {
                // Check for invalid instruction
                if (GetOpcodeNodeValue(node) == 0)
                {
                    info.flags |= IF_ERROR_INVALID;
                    return false;
                }
                // Get instruction definition
                const InstructionDefinition *instrDefinition = 
                    GetInstructionDefinition(node);
                // Check for invalid 64 bit instruction
                if ((m_disassemblerMode == DisassemblerMode::M64BIT) && 
                    (instrDefinition->flags & IDF_INVALID_64))
                {
                    info.flags |= IF_ERROR_INVALID_64;
                    return false;
                }
                // Update instruction info
                info.instrDefinition = instrDefinition;
                info.mnemonic = instrDefinition->mnemonic;
                // Update effective REX/VEX values
                calculateEffectiveRexVexValues(info);
                // Resolve operand and address mode
                resolveOperandAndAddressMode(info);
                // Decode operands
                if (!decodeOperands(info))
                {
                    return false;
                }
            }  
            return true;
        case OpcodeTreeNodeType::TABLE: 
            // Read next opcode byte
            if (!inputNext(info) && (info.flags & IF_ERROR_MASK))
            {
                return false;
            }
            // Update instruction info
            assert((info.opcode_length > 0) && (info.opcode_length < 3));
            info.opcode[info.opcode_length] = inputCurrent();
            info.opcode_length++;
            // Set child node index for next iteration
            index = inputCurrent();
            break;
        case OpcodeTreeNodeType::MODRM_MOD: 
            // Decode modrm byte
            if (!decodeModrm(info))
            {
                return false;
            }
            index = (info.modrm_mod == 0x3) ? 1 : 0;
            break;
        case OpcodeTreeNodeType::MODRM_REG: 
            // Decode modrm byte
            if (!decodeModrm(info))
            {
                return false;
            }
            index = info.modrm_reg;
            break;
        case OpcodeTreeNodeType::MODRM_RM: 
            // Decode modrm byte
            if (!decodeModrm(info))
            {
                return false;
            }
            index = info.modrm_rm;
            break;
        case OpcodeTreeNodeType::MANDATORY: 
            // Check if there are any prefixes present
            if (info.flags & IF_PREFIX_REP)
            {
                index = 1; // F2
            } else if (info.flags & IF_PREFIX_REPNE)
            {
                index = 2; // F3
            } else if (info.flags & IF_PREFIX_OPERAND_SIZE)
            {
                index = 3; // 66
            }
            if (GetOpcodeTreeChild(node, index) == 0)
            {
                index = 0;
            }
            if (index && (GetOpcodeTreeChild(node, index) != 0))
            {
                // Remove REP and REPNE prefix
                info.flags &= ~IF_PREFIX_REP;
                info.flags &= ~IF_PREFIX_REPNE;
                // Remove OPERAND_SIZE prefix, if it was used as mandatory prefix for the 
                // instruction
                if (index == 3)
                {
                    info.flags &= ~IF_PREFIX_OPERAND_SIZE;
                }
            }
            break;
        case OpcodeTreeNodeType::X87: 
            // Decode modrm byte
            if (!decodeModrm(info))
            {
                return false;
            }
            index = info.modrm - 0xC0;
            break;
        case OpcodeTreeNodeType::ADDRESS_SIZE: 
            switch (m_disassemblerMode)
            {
            case DisassemblerMode::M16BIT:
                index = (info.flags & IF_PREFIX_ADDRESS_SIZE) ? 1 : 0;
                break;
            case DisassemblerMode::M32BIT:
                index = (info.flags & IF_PREFIX_ADDRESS_SIZE) ? 0 : 1;
                break;
            case DisassemblerMode::M64BIT:
                index = (info.flags & IF_PREFIX_ADDRESS_SIZE) ? 1 : 2;
                break;
            default:
                assert(0);
            }
            break;
        case OpcodeTreeNodeType::OPERAND_SIZE: 
            switch (m_disassemblerMode)
            {
            case DisassemblerMode::M16BIT:
                index = (info.flags & IF_PREFIX_OPERAND_SIZE) ? 1 : 0;
                break;
            case DisassemblerMode::M32BIT:
                index = (info.flags & IF_PREFIX_OPERAND_SIZE) ? 0 : 1;
                break;
            case DisassemblerMode::M64BIT:
                index = (info.rex_w) ? 2 : ((info.flags & IF_PREFIX_OPERAND_SIZE) ? 0 : 1);
                break;
            default:
                assert(0);
            }
            break;
        case OpcodeTreeNodeType::MODE: 
            index = (m_disassemblerMode != DisassemblerMode::M64BIT) ? 0 : 1;
            break;
        case OpcodeTreeNodeType::VENDOR:
            switch (m_preferredVendor)
            {
            case InstructionSetVendor::ANY: 
                index = (GetOpcodeTreeChild(node, 0) != 0) ? 0 : 1;
                break;
            case InstructionSetVendor::INTEL: 
                index = 1;
                break;
            case InstructionSetVendor::AMD: 
                index = 0;
                break;
            default: 
                assert(0);
            }
            break;
        case OpcodeTreeNodeType::AMD3DNOW: 
            {     
                // As all 3dnow instructions got the same operands and flag definitions, we just
                // decode a random instruction and determine the specific opcode later.
                assert(GetOpcodeTreeChild(node, 0x0C) != 0);
                const InstructionDefinition *instrDefinition =
                    GetInstructionDefinition(GetOpcodeTreeChild(node, 0x0C));
                // Update instruction info
                info.instrDefinition = instrDefinition;
                info.mnemonic = instrDefinition->mnemonic;
                // Update effective REX/VEX values
                calculateEffectiveRexVexValues(info);
                // Resolve operand and address mode
                resolveOperandAndAddressMode(info);
                // Decode operands
                if (!decodeOperands(info))
                {
                    return false;
                }
                // Read the actual 3dnow opcode
                info.opcode[2] = inputNext(info);
                if (!info.opcode[2] && (info.flags & IF_ERROR_MASK))
                {
                    return false;
                }
                // Update instruction info
                instrDefinition = 
                    GetInstructionDefinition(GetOpcodeTreeChild(node, info.opcode[2]));
                if (!instrDefinition || 
                    (instrDefinition->mnemonic == InstructionMnemonic::INVALID))
                {
                    info.flags |= IF_ERROR_INVALID;
                    return false;
                }
                info.instrDefinition = instrDefinition;
                info.mnemonic = instrDefinition->mnemonic; 
                // Update operand access modes
                for (unsigned int i = 0; i < 4; ++i)
                {
                    if (info.operand[i].type != OperandType::NONE)
                    {
                        info.operand[i].access_mode = OperandAccessMode::READ;
                    }    
                }
                if (info.operand[0].type != OperandType::NONE)
                {
                    if (info.instrDefinition->flags & IDF_OPERAND1_WRITE)
                    {
                        info.operand[0].access_mode = OperandAccessMode::WRITE;
                    } else if (info.instrDefinition->flags & IDF_OPERAND1_READWRITE)
                    {
                        info.operand[0].access_mode = OperandAccessMode::READWRITE;
                    }
                }
                if (info.operand[1].type != OperandType::NONE)
                {
                    if (info.instrDefinition->flags & IDF_OPERAND2_WRITE)
                    {
                        info.operand[1].access_mode = OperandAccessMode::WRITE;
                    } else if (info.instrDefinition->flags & IDF_OPERAND2_READWRITE)
                    {
                        info.operand[1].access_mode = OperandAccessMode::READWRITE;
                    }
                }
                // Terminate loop
                return true;
            }
        case OpcodeTreeNodeType::VEX: 
            if ((m_disassemblerMode == DisassemblerMode::M64BIT) ||
                (((inputCurrent() >> 6) & 0x03) == 0x03))
            {
                // Decode vex prefix
                if (!decodeVex(info))
                {
                    return false;
                }
                // Update instruction info (error cases are checked by the @c decodeVex method)
                switch (info.vex_m_mmmm)
                {
                case 1:
                    info.opcode_length = 1;
                    info.opcode[0] = 0x0F;
                    break;
                case 2:
                    info.opcode_length = 2;
                    info.opcode[0] = 0x0F;
                    info.opcode[1] = 0x38;
                    break;
                case 3:
                    info.opcode_length = 2;
                    info.opcode[0] = 0x0F;
                    info.opcode[1] = 0x3A;
                    break;
                }
                // Set child node index for next iteration
                index = info.vex_m_mmmm + (info.vex_pp << 2);
            } else
            {
                index = 0;
            }
            break;
        case OpcodeTreeNodeType::VEXW: 
            assert(info.flags & IF_PREFIX_VEX);
            index = info.vex_w;
            break;
        case OpcodeTreeNodeType::VEXL: 
            assert(info.flags & IF_PREFIX_VEX);
            index = info.vex_l;
            break;
        default: 
            assert(0);
        }
        node = GetOpcodeTreeChild(node, index);
    } while (nodeType != OpcodeTreeNodeType::INSTRUCTION_DEFINITION);
    return false;
}

InstructionDecoder::InstructionDecoder()
    : m_input(nullptr)
    , m_disassemblerMode(DisassemblerMode::M32BIT)
    , m_preferredVendor(InstructionSetVendor::ANY) 
    , m_instructionPointer(0)
{

}    

InstructionDecoder::InstructionDecoder(BaseInput *input, 
    DisassemblerMode disassemblerMode, InstructionSetVendor preferredVendor,
    uint64_t instructionPointer)
    : m_input(input)
    , m_disassemblerMode(disassemblerMode)
    , m_preferredVendor(preferredVendor) 
    , m_instructionPointer(instructionPointer)
{

}

bool InstructionDecoder::decodeInstruction(InstructionInfo& info)
{
    // Clear instruction info
    memset(&info, 0, sizeof(info));
    // Set disassembler mode flags
    switch (m_disassemblerMode)
    {
    case DisassemblerMode::M16BIT: 
        info.flags |= IF_DISASSEMBLER_MODE_16;
        break;
    case DisassemblerMode::M32BIT: 
        info.flags |= IF_DISASSEMBLER_MODE_32;
        break;
    case DisassemblerMode::M64BIT: 
        info.flags |= IF_DISASSEMBLER_MODE_64;
        break;
    default: 
        assert(0);
    }
    // Set instruction address
    info.instrAddress = m_instructionPointer;
    // Decode
    if (!decodePrefixes(info) || !decodeOpcode(info))
    {
        goto DecodeError;
    }
    // SWAPGS is only valid in 64 bit mode
    if ((info.mnemonic == InstructionMnemonic::SWAPGS) && 
        (m_disassemblerMode != DisassemblerMode::M64BIT))
    {
        info.flags &= IF_ERROR_INVALID;
        goto DecodeError;
    }
    // Handle aliases
    if (info.mnemonic == InstructionMnemonic::XCHG)
    {
        if ((info.operand[0].type == OperandType::REGISTER && 
            info.operand[0].base == Register::AX &&
            info.operand[1].type == OperandType::REGISTER && 
            info.operand[1].base == Register::AX) || 
            (info.operand[0].type == OperandType::REGISTER && 
            info.operand[0].base == Register::EAX &&
            info.operand[1].type == OperandType::REGISTER && 
            info.operand[1].base == Register::EAX))
        {
            info.mnemonic = InstructionMnemonic::NOP;
            info.operand[0].type = OperandType::NONE;    
            info.operand[1].type = OperandType::NONE; 
            info.operand[0].access_mode = OperandAccessMode::NA;
            info.operand[1].access_mode = OperandAccessMode::NA;
        }
    }
    if ((info.mnemonic == InstructionMnemonic::NOP) && (info.flags & IF_PREFIX_REP))
    {
        info.mnemonic = InstructionMnemonic::PAUSE;
        info.flags &= ~IF_PREFIX_REP;
    }
    // Increment instruction pointer
    m_instructionPointer += info.length;
    // Set instruction pointer
    info.instrPointer = m_instructionPointer;
    return true;
DecodeError:
    // Increment instruction pointer. 
    m_instructionPointer += 1;
    // Backup all error flags, the instruction length and the instruction address
    uint32_t flags = info.flags & (IF_ERROR_MASK | 0x00000007);
    uint8_t length = info.length;
    uint8_t firstByte = info.data[0];
    uint64_t instrAddress = info.instrAddress;
    // Clear instruction info
    memset(&info, 0, sizeof(info));
    // Restore saved values
    info.flags = flags;
    info.length = length;
    info.data[0] = firstByte;
    info.instrAddress = instrAddress;
    info.instrDefinition = Internal::GetInstructionDefinition(0);
    // Decrement the input position, if more than one byte was read from the input data 
    // source while decoding the invalid instruction. 
    if (info.length != 1)
    {
        m_input->setPosition(m_input->getPosition() - info.length + 1);
        info.length = 1;
    }
    // Return with error, if the end of the input source was reached while decoding the 
    // invalid instruction
    if (info.flags & IF_ERROR_END_OF_INPUT)
    {
        info.length = 0;
        return false;
    }
    return true;
}

/* ============================================================================================== */

}
