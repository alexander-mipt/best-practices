#include <iostream>
#include <unordered_map>
#include <vector>
#include "../pipeline/pipeline.h"
#include "gtest/gtest.h"


namespace pipeline::tests {

    class InstructionsTest : public ::testing::Test {
    public:
        InstructionsTest() {};

        virtual ~InstructionsTest() {};
    };

    TEST_F(InstructionsTest, AddiTest) {
        const std::vector<pipeline::word_> instructions = {
            0x00850513,     // addi a0, a0, 8
            0x00000013,     // addi x0, x0, 0 == nop
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x0000006f      // jr 0
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;
    }

    TEST_F(InstructionsTest, AddTest) {
        const std::vector<pipeline::word_> instructions = {
                0x00c58533,     // add  a0, a1, a2
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;
    }

    TEST_F(InstructionsTest, RightShiftTest) {
        const std::vector<pipeline::word_> instructions = {
                0x00402503,     // lw   a0, 4
                0x00300593,     // addi a1, x0, 3
                0x40b55533,     // sra  a0, a0, a1
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00402603,     // lw  a2, 4
                0x00b65633,     // srl a2, a2, a1
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };
        const pipeline::byte_ data_addr = 4;
        const pipeline::word_ magic_number = 0x8000ffff;
        std::unordered_map<pipeline::word_, pipeline::word_> data = {
                {data_addr, magic_number}
        };

        const pipeline::byte_ reg_addr0 = 10;
        const pipeline::word_ shift_res0 = 0xf0001fff;
        const pipeline::byte_ reg_addr2 = 12;
        const pipeline::word_ shift_res2 = 0x10001fff;

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr, data};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;

        ASSERT_EQ(cpu.getRegister(reg_addr0), shift_res0);
        ASSERT_EQ(cpu.getRegister(reg_addr2), shift_res2);
    }

    TEST_F(InstructionsTest, SltiTest) {
        const std::vector<pipeline::word_> instructions = {
                0x00300593,     // addi a1, x0, 3
                0x0045a513,     // slti a0, a1, 4
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };

        const pipeline::byte_ reg_addr0 = 10;
        const pipeline::byte_ reg_addr1 = 11;
        const pipeline::word_ a1_value = 3;
        const pipeline::word_ cmp_res = 1;

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;

        ASSERT_EQ(cpu.getRegister(reg_addr1), a1_value);
        ASSERT_EQ(cpu.getRegister(reg_addr0), cmp_res);
    }

    TEST_F(InstructionsTest, AuipcTest) {
        const std::vector<pipeline::word_> instructions = {
                0x00000517,     // auipc a0, 0
                0x00000013,     // addi  x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;

        const pipeline::byte_ reg_addr = 10;
        ASSERT_EQ(cpu.getRegister(reg_addr), start_inst_addr);
    }

    TEST_F(InstructionsTest, LwTest) {
        const std::vector<pipeline::word_> instructions = {
                0x00402503,     // lw   a0, 4
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };
        const pipeline::word_ magic_number = 42;
        std::unordered_map<pipeline::word_, pipeline::word_> data = {
                {4, magic_number}
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr, data};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;

        ASSERT_EQ(cpu.getRegister(10), magic_number);
    }

    TEST_F(InstructionsTest, LbTest) {
        const std::vector<pipeline::word_> instructions = {
                0x00400503,     // lb   a0, 4
                0x00404583,     // lbu  a1, 4
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };
        const pipeline::word_ magic_number = 0xffffffff;
        std::unordered_map<pipeline::word_, pipeline::word_> data = {
                {4, magic_number}
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr, data};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;

        ASSERT_EQ(cpu.getRegister(10), magic_number);
        const pipeline::word_ zero_extended = 0xff;
        ASSERT_EQ(cpu.getRegister(11), zero_extended);
    }

    TEST_F(InstructionsTest, SwTest) {
        const std::vector<pipeline::word_> instructions = {
                0x02a50513,     // addi a0, a0, 42
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00a02223,     // sw a0, x0, 4
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };

        const pipeline::byte_ reg_addr = 10;
        const pipeline::word_ magic_number = 42;
        const pipeline::word_ data_addr = 4;

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;

        ASSERT_EQ(cpu.getRegister(reg_addr), magic_number);
        ASSERT_EQ(cpu.getDataWord(data_addr), magic_number);
    }

    TEST_F(InstructionsTest, SbTest) {
        const std::vector<pipeline::word_> instructions = {
                0x10150513,     // addi a0, a0, 257
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00a00223,     // sb a0, x0, 4
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };

        const pipeline::byte_ reg_addr = 10;
        const pipeline::word_ magic_number = 257;
        const pipeline::word_ cut_magic_number = 1;
        const pipeline::word_ data_addr = 4;

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        cpu.setProgramCounter(start_inst_addr);
        cpu.run();
        std::cout << "Taken " << std::dec << cpu.getTicksCounter() << " ticks to run the program" << std::endl;

        ASSERT_EQ(cpu.getRegister(reg_addr), magic_number);
        ASSERT_EQ(cpu.getDataWord(data_addr), cut_magic_number);
    }
}
