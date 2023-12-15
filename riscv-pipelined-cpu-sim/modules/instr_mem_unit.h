#ifndef RISC_V_SIMULATOR_INSTR_MEM_UNIT_H
#define RISC_V_SIMULATOR_INSTR_MEM_UNIT_H

#include <algorithm>
#include <array>
#include <vector>
#include "common.h"


namespace modules {

    class InstrMemUnit {
    public:
        explicit InstrMemUnit(const std::vector<word_>& instructions, word_ start_address) : address(0) {
            if (start_address % sizeof(word_)) {
                throw AlignmentException("start_address: " +\
                    std::to_string(start_address) +\
                    " % " + std::to_string(sizeof(word_)) + " != 0");
            }
            std::copy(instructions.cbegin(), instructions.cend(),
                      memory.begin() + start_address / sizeof(word_));
            start_of_section = start_address;
            end_of_section = start_address + instructions.size() * sizeof(word_);
        }

        virtual ~InstrMemUnit() noexcept = default;

        [[nodiscard]] word_ getData() {
            return *reinterpret_cast<word_ *>(reinterpret_cast<byte_ *>(memory.data()) + address);
        }

        [[nodiscard]] bool isPcOutOfSection(word_ pc) const {
            return (pc < start_of_section || pc >= end_of_section);
        }

#ifdef DEBUG
        virtual void debug() {
            std::cout << "InstrMemUnit: read_data=" << getData() << "; address=" << address << std::endl;
        }
#endif

        word_ address = 0;

    private:
        static constexpr size_t capacity = 1024;
        std::array<word_, capacity> memory = {0};
        word_ start_of_section = 0;
        word_ end_of_section = 0;
    };
}

#endif //RISC_V_SIMULATOR_INSTR_MEM_UNIT_H
