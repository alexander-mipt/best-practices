#ifndef RISC_V_SIMULATOR_REGFILE_H
#define RISC_V_SIMULATOR_REGFILE_H

#include <array>
#include <exception>


namespace modules {

    class RegFile {
    public:
        virtual ~RegFile() noexcept = default;

        virtual void tick() {
            if (write_enable3 && address3 != 0) {
#ifdef DEBUG
                std::cout << "======================== writing into reg ";
                std::cout << std::hex << static_cast<word_>(address3);
                std::cout << " value " << write_data3 << std::endl;
#endif
                regs.at(address3) = write_data3;
            }
            read_data1 = regs.at(address1);
            read_data2 = regs.at(address2);
        }

        [[nodiscard]] word_ getReadData1() const {
            return read_data1;
        }

        [[nodiscard]] word_ getReadData2() const {
            return read_data2;
        }

        void setNewSignals(byte_ addr1, byte_ addr2, byte_ addr3, bool we3, word_ wd3) {
            address1 = addr1;
            address2 = addr2;
            address3 = addr3;
            write_enable3 = we3;
            write_data3 = wd3;
        }

#ifdef DEBUG
        [[nodiscard]] word_ getRegDirectly(word_ addr) const {
            return regs.at(addr);
        }

        virtual void debug() const {
            std::cout << "RegFile: read_data1=" << read_data1 << "; read_data2=" << read_data2;
            std::cout << "; write_enable3=" << write_enable3 << "; write_data3=" << write_data3;
            std::cout << "; address1=" << static_cast<word_>(address1) << "; address2=";
            std::cout << static_cast<word_>(address2) << "; address3=" << static_cast<word_>(address3) << std::endl;
        }

        virtual void printRegisters() const {
            std::cout << "Current registers file state:" << std::endl;
            for (size_t i = 0; i < number_of_regs; ++i) {
                std::cout << "x" << i << " == " << regs[i] << std::endl;
            }
            std::cout << std::endl;
        }
#endif

        byte_ address1 = 0;
        byte_ address2 = 0;
        byte_ address3 = 0;
        bool write_enable3 = false;
        word_ write_data3 = 0;

    private:
        static constexpr size_t number_of_regs = 32;
        std::array<word_, number_of_regs> regs = {0};
        word_ read_data1 = 0;
        word_ read_data2 = 0;
    };
}

#endif //RISC_V_SIMULATOR_REGFILE_H
