#ifndef RISC_V_SIMULATOR_INSTRUCTION_H
#define RISC_V_SIMULATOR_INSTRUCTION_H

#include <iostream>
#include "../modules/common.h"


namespace pipeline::utils {

    using modules::byte_;
    using modules::word_;

    enum class InstructionType : modules::byte_ {
        UNKNOWN,
        RType,
        IType,
        SType,
        BType,
        UType,
        JType
    };

    constexpr inline byte_ getFunct7(word_ instruction) {
        return static_cast<byte_>((instruction >> 25) & 0x0000007f);
    }

    constexpr inline byte_ getRs2(word_ instruction) {
        return static_cast<byte_>((instruction >> 20) & 0x0000001f);
    }

    constexpr inline byte_ getRs1(word_ instruction) {
        return static_cast<byte_>((instruction >> 15) & 0x0000001f);
    }

    constexpr inline byte_ getFunct3(word_ instruction) {
        return static_cast<byte_>((instruction >> 12) & 0x00000007);
    }

    constexpr inline byte_ getRd(word_ instruction) {
        return static_cast<byte_>((instruction >> 7) & 0x0000001f);
    }

    constexpr inline byte_ getOpcode(word_ instruction) {
        return static_cast<byte_>(instruction & 0x0000007f);
    }

    constexpr inline InstructionType getInstructionType(byte_ opcode) {
        switch (opcode) {
            case 0b0110011:
                return InstructionType::RType;
            case 0b0100011:
                return InstructionType::SType;
            case 0b1100011:
                return InstructionType::BType;
            case 0b0000011:
            case 0b0010011:
            case 0b1110011:
            case 0b1100111:
                return InstructionType::IType;
            case 0b0110111:
            case 0b0010111:
                return InstructionType::UType;
            case 0b1101111:
                return InstructionType::JType;
            default:
                return InstructionType::UNKNOWN;
                // throw std::logic_error("invalid opcode: " + std::to_string(opcode));
        }
    }

    constexpr inline bool hasRs1(InstructionType type) {
        return (type != InstructionType::UType && type != InstructionType::JType);
    }

    constexpr inline bool hasRs2(InstructionType type) {
        return (type == InstructionType::RType || type == InstructionType::IType ||
                type == InstructionType::SType || type == InstructionType::BType);
    }

    struct ImmediateExtensionBlock {
        constexpr inline word_ operator ()(word_ instr) {
            auto instr_t = utils::getInstructionType(getOpcode(instr));
            if (instr_t == InstructionType::UNKNOWN) {
                return 0;
            }
            union imm_layout {
                word_ instr;
                struct {
                    word_ pad0 : 20;
                    word_ imm0 : 12;
                } __attribute__((packed)) instr_i_type;
                struct {
                    word_ imm0 : 12;
                    word_ se : 20;
                } __attribute__((packed)) imm_i_type;
                struct {
                    word_ pad0 : 7;
                    word_ imm0 : 5;
                    word_ pad1 : 13;
                    word_ imm1 : 7;
                } __attribute__((packed)) instr_s_type;
                struct {
                    word_ imm0 : 5;
                    word_ imm1 : 7;
                    word_ se : 20;
                } __attribute__((packed)) imm_s_type;
                struct {
                    word_ pad0 : 7;
                    word_ imm2 : 1;
                    word_ imm0 : 4;
                    word_ pad2 : 13;
                    word_ imm1 : 6;
                    word_ sign : 1;
                } __attribute__((packed)) instr_b_type;
                struct {
                    word_ zero : 1;
                    word_ imm0 : 4;
                    word_ imm1 : 6;
                    word_ imm2 : 1;
                    word_ se : 20;
                } __attribute__((packed)) imm_b_type;
                struct {
                    word_ pad : 12;
                    word_ imm : 20;
                } __attribute__((packed)) instr_u_type;
                struct {
                    word_ zeros : 12;
                    word_ imm : 20;
                } __attribute__((packed)) imm_u_type;
                struct {
                    word_ pad0 : 12;
                    word_ imm2 : 8;
                    word_ imm1 : 1;
                    word_ imm0 : 10;
                    word_ sign : 1;
                } __attribute__((packed)) instr_j_type;
                struct {
                    word_ zero : 1;
                    word_ imm0 : 10;
                    word_ imm1 : 1;
                    word_ imm2 : 8;
                    word_ se : 12;
                } __attribute__((packed)) imm_j_type;
            } in{.instr = instr}, out{.instr = 0};
            byte_ sign = (in.instr >> 31) & 1;

            switch (instr_t) {
                case InstructionType::RType:
                    break;
                case InstructionType::IType:
                    out.imm_i_type.imm0 = in.instr_i_type.imm0;
                    if (sign) {
                        out.imm_i_type.se--;
                    }
                    break;
                case InstructionType::SType:
                    out.imm_s_type.imm0 = in.instr_s_type.imm0;
                    out.imm_s_type.imm1 = in.instr_s_type.imm1;
                    if (sign) {
                        out.imm_s_type.se--;
                    }
                    break;
                case InstructionType::BType:
                    out.imm_b_type.imm0 = in.instr_b_type.imm0;
                    out.imm_b_type.imm1 = in.instr_b_type.imm1;
                    out.imm_b_type.imm2 = in.instr_b_type.imm2;
                    if (sign) {
                        out.imm_b_type.se--;
                    }
                    break;
                case InstructionType::JType:
                    out.imm_j_type.imm0 = in.instr_j_type.imm0;
                    out.imm_j_type.imm1 = in.instr_j_type.imm1;
                    out.imm_j_type.imm2 = in.instr_j_type.imm2;
                    if (sign) {
                        out.imm_j_type.se--;
                    }
                    break;
                case InstructionType::UType:
                    out.imm_u_type.imm = in.instr_u_type.imm;
                    break;
                default:
                    throw std::logic_error("invalid instruction type: " +\
                                           std::to_string(static_cast<byte_>(instr_t)));
            }

            return out.instr;
        }
    };
}

#endif //RISC_V_SIMULATOR_INSTRUCTION_H
