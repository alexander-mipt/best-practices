#include <vector>
#include "pipeline/pipeline.h"


int main() {
    // Fibonacci number calculation
    const std::vector<pipeline::word_> instructions = {
            0x00020137,
            0x04c000ef,
            0x0ff00613,
            0x000306b7,
            0x00c68223,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00100513,
            0x00100593,
            0x03700693,
            0x00a58633,
            0x00b00533,
            0x00c005b3,
            0xfea69ae3,
            0x00008067
    };

    const pipeline::word_ start_inst_addr = 16;
    pipeline::Pipeline cpu{instructions, start_inst_addr};
    cpu.setProgramCounter(start_inst_addr);
    cpu.run();

    return 0;
}
