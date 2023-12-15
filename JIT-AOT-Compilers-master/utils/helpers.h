#ifndef JIT_AOT_COMPILERS_COURSE_HELPERS_H_
#define JIT_AOT_COMPILERS_COURSE_HELPERS_H_

#include <type_traits>


namespace utils {
template <typename E>
constexpr inline auto to_underlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

template <typename E>
constexpr inline auto underlying_logic_or(E e) noexcept {
    return to_underlying(e);
}

template <typename E, typename... Eout>
constexpr inline auto underlying_logic_or(E e, Eout... out) noexcept
requires (std::is_same_v<std::remove_cv_t<E>, std::remove_cv_t<Eout>> && ...) {
    return underlying_logic_or(e) | underlying_logic_or(out...);
}

struct expand_t {
template <typename... T> expand_t(T...) {}
};

template <typename T, typename = void>
struct has_set_flag {
    static constexpr bool value = false;
};

template<typename T>
struct has_set_flag<T, std::void_t<decltype(T::SET_FLAG)>> {
    static constexpr bool value = true;
};

template <typename T>
constexpr bool has_set_flag_v = has_set_flag<T>::value;
}   // namespace utils

#endif // JIT_AOT_COMPILERS_COURSE_HELPERS_H_
