#ifndef RISC_V_SIMULATOR_ALU_H
#define RISC_V_SIMULATOR_ALU_H

#include <exception>
#include <iostream>
#include "common.h"


namespace modules {

    enum class ALUControl : byte_ {
        NOP,
        ADD,
        SUB,
        SLL,
        SLT,
        SLTU,
        XOR,
        SRL,
        SRA,
        OR,
        AND
    };

    inline std::ostream& operator<<(std::ostream& os, const ALUControl& alu_control) {
        switch (alu_control) {
            case ALUControl::NOP:
                os << "NOP";
                break;
            case ALUControl::ADD:
                os << "ADD";
                break;
            case ALUControl::SUB:
                os << "SUB";
                break;
            case ALUControl::SLL:
                os << "SLL";
                break;
            case ALUControl::SLT:
                os << "SLT";
                break;
            case ALUControl::SLTU:
                os << "SLTU";
                break;
            case ALUControl::XOR:
                os << "XOR";
                break;
            case ALUControl::SRL:
                os << "SRL";
                break;
            case ALUControl::SRA:
                os << "SRA";
                break;
            case ALUControl::OR:
                os << "OR";
                break;
            case ALUControl::AND:
                os << "AND";
                break;
        }
        return os;
    }

    struct ALU {
        constexpr inline word_ operator()(ALUControl control_signal, word_ src_a, word_ src_b) {
            switch (control_signal) {
                case ALUControl::NOP:
                    return 0;
                case ALUControl::ADD:
                    return src_a + src_b;
                case ALUControl::SUB:
                    return src_a - src_b;
                case ALUControl::AND:
                    return src_a & src_b;
                case ALUControl::OR:
                    return src_a | src_b;
                case ALUControl::XOR:
                    return src_a ^ src_b;
                case ALUControl::SLL:
                    // for shifts only the least-significant 5 bits are used
                    src_b &= 0x0000001f;
                    return src_a << src_b;
                case ALUControl::SRL:
                    src_b &= 0x0000001f;
                    return src_a >> src_b;
                case ALUControl::SRA:
                    src_b &= 0x0000001f;
                    return (int32_t)src_a >> src_b;
                case ALUControl::SLT:
                    return ((int32_t)src_a < (int32_t)src_b);
                case ALUControl::SLTU:
                    return (src_a < src_b);
                default:
                    throw std::logic_error("unknown control signal: " +\
                        std::to_string(static_cast<byte_>(control_signal)));
            }
        }
    };
}

#endif //RISC_V_SIMULATOR_ALU_H
