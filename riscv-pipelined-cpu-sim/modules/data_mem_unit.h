#ifndef RISC_V_SIMULATOR_DATA_MEM_UNIT_H
#define RISC_V_SIMULATOR_DATA_MEM_UNIT_H

#include <algorithm>
#include <array>
#include <iostream>
#include <unordered_map>
#include "address_exceptions.h"
#include "common.h"


namespace modules {

    enum class StoreMode : byte_ {
        Byte = 0,
        Halfword = 1,
        Word = 2
    };

    inline std::ostream& operator<<(std::ostream& os, const StoreMode& store_mode) {
        switch (store_mode) {
            case StoreMode::Byte:
                os << "Byte";
                break;
            case StoreMode::Halfword:
                os << "Halfword";
                break;
            case StoreMode::Word:
                os << "Word";
                break;
        }
        return os;
    }

    enum class LoadExtensionMode : byte_ {
        Byte = 0,
        Halfword = 1,
        Word = 2,
        ByteU = 4,
        HalfwordU = 5
    };

    inline std::ostream& operator<<(std::ostream& os, const LoadExtensionMode& load_mode) {
        switch (load_mode) {
            case LoadExtensionMode::Byte:
                os << "Byte";
                break;
            case LoadExtensionMode::Halfword:
                os << "Halfword";
                break;
            case LoadExtensionMode::Word:
                os << "Word";
                break;
            case LoadExtensionMode::ByteU:
                os << "ByteU";
                break;
            case LoadExtensionMode::HalfwordU:
                os << "HalfwordU";
                break;
        }
        return os;
    }

    class DataMemUnit {
    public:
        DataMemUnit() = default;

        explicit DataMemUnit(const std::unordered_map<word_, word_>& data) : address(0) {
            for (const auto& [key, value] : data) {
                *reinterpret_cast<word_ *>(reinterpret_cast<byte_ *>(memory.data()) + key) = value;
            }
        }

        virtual ~DataMemUnit() noexcept = default;

        virtual void tick() {
            if (write_enable) {
                if (address > capacity * sizeof(word_)) {
                    throw InvalidAddressException("data write overflow - address=" +\
                                                  std::to_string(address));
                }
                // SH and SB commands can write 16 and 8-bit values
                if (address % sizeof(byte_)) {
                    throw AlignmentException("data write invalid alignment - address="+\
                                             std::to_string(address));
                }
                storeValue();
#ifdef DEBUG
                std::cout << "======================== writing value " << write_data << " on address ";
                std::cout << std::hex << static_cast<word_>(address) << std::endl;
#endif
                // read_data modification is useless, as we don't read after write
            } else {
                read_data = getData();
#ifdef DEBUG
                std::cout << "======================== reading value " << read_data << " on address ";
                std::cout << std::hex << static_cast<word_>(address) << std::endl;
#endif
            }
        }

        [[nodiscard]] word_ getData() {
            if (address > capacity * sizeof(word_)) {
                throw InvalidAddressException("data read overflow - address=" + std::to_string(address));
            }
            // LH and LB commands can read 16 and 8-bit values
            if (address % sizeof(byte_)) {
                throw AlignmentException("data write invalid alignment - address="+\
                    std::to_string(address));
            }
            return loadValue();
        }

#ifdef DEBUG
        virtual void debug() const {
            std::cout << "DataMemUnit: read_data=" << read_data << "; write_enable=";
            std::cout << write_enable << "; address=" << address << "; store_mode=";
            std::cout << static_cast<StoreMode>(store_mode) << "; load_mode=";
            std::cout << static_cast<LoadExtensionMode>(store_mode) << std::endl;
        }
#endif

        byte_ store_mode = 0;
        bool write_enable = false;
        word_ address = 0;
        word_ write_data = 0;

    private:

        void storeValue() {
            auto store_mode_enum = static_cast<StoreMode>(store_mode);
            switch (store_mode_enum) {
                case StoreMode::Byte:
                    *(reinterpret_cast<byte_ *>(memory.data()) + address) = static_cast<byte_>(write_data);
                    break;
                case StoreMode::Halfword:
                    *reinterpret_cast<byte2_ *>(reinterpret_cast<byte_ *>(memory.data()) + address) = static_cast<byte2_>(write_data);
                    break;
                case StoreMode::Word:
                    *reinterpret_cast<word_ *>(reinterpret_cast<byte_ *>(memory.data()) + address) = write_data;
                    break;
            }
        }

        word_ loadValue() {
            auto load_mode = static_cast<LoadExtensionMode>(store_mode);
            switch (load_mode) {
                case LoadExtensionMode::Byte:
                    return signExtendFromByte(*(reinterpret_cast<byte_ *>(memory.data()) + address));
                case LoadExtensionMode::Halfword:
                    return signExtendFromByte2(*reinterpret_cast<byte2_ *>(reinterpret_cast<byte_ *>(memory.data()) + address));
                case LoadExtensionMode::Word:
                    return *reinterpret_cast<word_ *>(reinterpret_cast<byte_ *>(memory.data()) + address);
                case LoadExtensionMode::ByteU:
                    return static_cast<word_>(*(reinterpret_cast<byte_ *>(memory.data()) + address));
                case LoadExtensionMode::HalfwordU:
                    return static_cast<word_>(*reinterpret_cast<byte2_ *>(reinterpret_cast<byte_ *>(memory.data()) + address));
            }
            throw std::logic_error("invalid funct3: " + std::to_string(store_mode));
        }

        static constexpr size_t capacity = 4096;
        std::array<word_, capacity> memory = {0};
        word_ read_data = 0;
    };
}

#endif //RISC_V_SIMULATOR_DATA_MEM_UNIT_H
