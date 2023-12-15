#ifndef JIT_AOT_COMPILERS_COURSE_ALLOCATOR_UTILS_H_
#define JIT_AOT_COMPILERS_COURSE_ALLOCATOR_UTILS_H_

#include "macros.h"
#include <memory_resource>
#include <type_traits>


namespace utils {
template <typename T>
concept NonVoid = (!std::is_same_v<T, void>);

template <NonVoid T, typename... ArgsT>
[[nodiscard]] inline T *New(std::pmr::polymorphic_allocator<> &allocator, ArgsT&&... args) {
    // TODO: move into class Graph
    auto *mem = allocator.template allocate_object<T>();
    ASSERT(mem);
    allocator.construct(mem, std::forward<ArgsT>(args)...);
    return mem;
}

template <NonVoid T, typename... ArgsT>
[[nodiscard]] inline T *New(std::pmr::memory_resource *resource, ArgsT&&... args) {
    ASSERT(resource);
    auto allocator = std::pmr::polymorphic_allocator(resource);
    return New<T, ArgsT...>(allocator, std::forward<ArgsT>(args)...);
}
}   // namespace utils

#endif // JIT_AOT_COMPILERS_COURSE_ALLOCATOR_UTILS_H_
