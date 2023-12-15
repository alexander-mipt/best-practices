#ifndef RISC_V_SIMULATOR_REGISTER_H
#define RISC_V_SIMULATOR_REGISTER_H

#include "common.h"


namespace modules {

    template <typename T>
    class Register {
    public:
        virtual ~Register() noexcept = default;

        virtual void tick() {
            if (enable_flag) {
                current = next;
            }
        }

        [[nodiscard]] T getCurrent() const {
            return current;
        }

        virtual void clear() {
            current = 0;
            next = 0;
        }

        virtual void debug() const {
            std::cout << "Register: enable_flag=" << enable_flag << "; next=" << next;
            std::cout << "; current=" << current << std::endl;
        }

        bool enable_flag = true;
        T next = 0;

    private:
        T current = 0;
    };
}


#endif //RISC_V_SIMULATOR_REGISTER_H
