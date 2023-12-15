#ifndef JIT_AOT_COMPILERS_COURSE_CONCEPTS_H_
#define JIT_AOT_COMPILERS_COURSE_CONCEPTS_H_

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "instructions/Types.h"


namespace ir {
template <typename T>
concept Numeric = std::is_arithmetic<T>::value;

template <typename BaseType, typename... SubType>
concept InstructionType = (std::is_base_of_v<BaseType, SubType> && ...);

template <typename TargetType, typename... InputsTypes>
concept IsSameType = (std::is_same_v<std::remove_cv_t<TargetType>, std::remove_cv_t<InputsTypes>> && ...);

template <typename FunctionType, typename ArgumentType, typename ReturnType>
concept UnaryFunctionType = requires (FunctionType func, ArgumentType arg) {
    { func(arg) } -> std::same_as<ReturnType>;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_CONCEPTS_H_
