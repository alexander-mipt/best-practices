#ifndef RISC_V_SIMULATOR_PIPELINE_H
#define RISC_V_SIMULATOR_PIPELINE_H

#include <array>
#include <unordered_map>
#include <vector>
#include "control_unit.h"
#include "hazard_unit.h"
#include "../modules/alu.h"
#include "../modules/cmp.h"
#include "../modules/common.h"
#include "../modules/data_mem_unit.h"
#include "../modules/instr_mem_unit.h"
#include "../modules/register.h"
#include "../modules/regfile.h"

#define LOG_FIELD(obj, field) os << #field << "=" << std::hex << obj.field << ", "

namespace pipeline {

    using modules::byte_;
    using modules::word_;

    struct DecodeState {
        DecodeState(word_ value) : v_de(static_cast<bool>(value)),
                                   pc_de(value),
                                   instr(value) {};

        bool v_de;
        word_ pc_de;
        word_ instr;
    };

    inline std::ostream& operator<<(std::ostream& os, const DecodeState& decode_state) {
        LOG_FIELD(decode_state, v_de);
        LOG_FIELD(decode_state, pc_de);
        LOG_FIELD(decode_state, instr);
        return os;
    }

    struct ExecuteState {
        ExecuteState(word_ value) : funct3(static_cast<byte_>(value)),
                                    is_jalr(static_cast<bool>(value)),
                                    alu_op(static_cast<modules::ALUControl>(value)),
                                    alu_src2(static_cast<bool>(value)),
                                    mem_to_reg(static_cast<bool>(value)),
                                    wb_we(static_cast<bool>(value)),
                                    mem_we(static_cast<bool>(value)),
                                    cmp_control(static_cast<modules::CmpControl>(value)),
                                    brn_cond(static_cast<bool>(value)),
                                    jmp_cond(static_cast<bool>(value)),
                                    v_de(static_cast<bool>(value)),
                                    data1(value),
                                    data2(value),
                                    pc_de(value),
                                    instr(value) {};

        byte_ funct3;
        bool is_jalr;
        modules::ALUControl alu_op;
        bool alu_src2;
        bool mem_to_reg;
        bool wb_we;
        bool mem_we;
        modules::CmpControl cmp_control;
        bool brn_cond;
        bool jmp_cond;
        bool v_de;
        word_ data1;
        word_ data2;
        word_ pc_de;
        word_ instr;
    };

    inline std::ostream& operator<<(std::ostream& os, const ExecuteState& execute_state) {
        os << "funct3=" << std::hex << static_cast<word_>(execute_state.funct3) << ", ";
        LOG_FIELD(execute_state, is_jalr);
        LOG_FIELD(execute_state, alu_op);
        LOG_FIELD(execute_state, alu_src2);
        LOG_FIELD(execute_state, mem_to_reg);
        LOG_FIELD(execute_state, wb_we);
        LOG_FIELD(execute_state, mem_we);
        LOG_FIELD(execute_state, cmp_control);
        LOG_FIELD(execute_state, brn_cond);
        LOG_FIELD(execute_state, jmp_cond);
        LOG_FIELD(execute_state, v_de);
        LOG_FIELD(execute_state, data1);
        LOG_FIELD(execute_state, data2);
        LOG_FIELD(execute_state, pc_de);
        LOG_FIELD(execute_state, instr);
        return os;
    }

    struct MemoryState {
        MemoryState(word_ value) : mem_we(static_cast<bool>(value)),
                                   mem_to_reg(static_cast<bool>(value)),
                                   wb_we(static_cast<bool>(value)),
                                   jmp_cond(static_cast<bool>(value)),
                                   store_mode(static_cast<byte_>(value)),
                                   write_data(value),
                                   alu_res(value),
                                   wb_a(value) {};

        bool mem_we;
        bool mem_to_reg;
        bool wb_we;
        bool jmp_cond;
        byte_ store_mode;
        word_ write_data;
        word_ alu_res;
        word_ wb_a;
    };

    inline std::ostream& operator<<(std::ostream& os, const MemoryState& memory_state) {
        LOG_FIELD(memory_state, mem_we);
        LOG_FIELD(memory_state, mem_to_reg);
        LOG_FIELD(memory_state, wb_we);
        LOG_FIELD(memory_state, jmp_cond);
        os << "store_mode" << "=" << std::hex << static_cast<word_>(memory_state.store_mode) << ", ";
        LOG_FIELD(memory_state, write_data);
        LOG_FIELD(memory_state, alu_res);
        LOG_FIELD(memory_state, wb_a);
        return os;
    }

    struct WriteBackState {
        WriteBackState(word_ value) : wb_we(static_cast<bool>(value)),
                                      wb_d(value),
                                      wb_a(value) {};

        bool wb_we;
        word_ wb_d;
        byte_ wb_a;
    };

    inline std::ostream& operator<<(std::ostream& os, const WriteBackState& wb_state) {
        LOG_FIELD(wb_state, wb_we);
        LOG_FIELD(wb_state, wb_d);
        os << "wb_a" << "=" << std::hex << static_cast<word_>(wb_state.wb_a);
        return os;
    }
#undef LOG_FIELD

    class Pipeline {
    public:
        explicit Pipeline(const std::vector<word_>& instructions,
                          word_ start_instr_address) : instr_mem_unit(instructions, start_instr_address) {
            std::cout << std::hex;
        };

        Pipeline(const std::vector<word_>& instructions,
                 word_ start_instr_address,
                 const std::unordered_map<word_, word_>& data) : instr_mem_unit(instructions, start_instr_address),
                                                                 data_mem_unit(data) {
            std::cout << std::hex;
        };

        virtual ~Pipeline() noexcept = default;

        virtual void setProgramCounter(word_ pc);

        virtual void tick();

        virtual void run();

        size_t getTicksCounter() const {
            return ticks_counter;
        }

#ifdef DEBUG
        [[nodiscard]] virtual word_ getRegister(word_ addr) const {
            return reg_file.getRegDirectly(addr);
        };

        virtual word_ getDataWord(word_ addr) {
            word_ res = 0;
            auto current_addr = data_mem_unit.address;
            data_mem_unit.address = addr;
            res = data_mem_unit.getData();
            data_mem_unit.address = current_addr;
            return res;
        }

        virtual void printRegisters() const {
            reg_file.printRegisters();
        }

        virtual void debug();
#endif

    private:
        virtual void doFetch();

        virtual void doDecode();

        virtual void doExecute();

        virtual void doMemory();

        virtual void doWriteBack();

        virtual void hazardUnitTick();

        void tickStateRegisters();

        void haltPipeline();

        void restartPipeline();

        size_t ticks_counter = 0;
        int last_instructions_counter = 0;
        bool last_instructions_flag = false;

        word_ bp_mem = 0;
        word_ pc_disp = 0;
        bool pc_r = false;

        BypassOptionsEncoding hu_rs1 = BypassOptionsEncoding::REG;
        BypassOptionsEncoding hu_rs2 = BypassOptionsEncoding::REG;

        modules::InstrMemUnit instr_mem_unit;
        modules::DataMemUnit data_mem_unit;
        modules::RegFile reg_file;
        ControlUnit control_unit;
        modules::Register<word_> program_counter;

        modules::Register<DecodeState> decode_register;
        modules::Register<ExecuteState> execute_register;
        modules::Register<MemoryState> memory_register;
        modules::Register<WriteBackState> write_back_register;
    };
}

#endif //RISC_V_SIMULATOR_PIPELINE_H
