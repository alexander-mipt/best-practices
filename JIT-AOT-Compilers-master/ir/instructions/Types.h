#ifndef JIT_AOT_COMPILERS_COURSE_TYPES_H_
#define JIT_AOT_COMPILERS_COURSE_TYPES_H_

#include <array>
#include <cstdint>
#include "helpers.h"
#include <limits>
#include "macros.h"
#include <type_traits>


namespace ir {
class TypeId {
public:
    using TypeIdType = uint64_t;

    TypeId(TypeIdType id) : id(id) {}
    DEFAULT_COPY_SEMANTIC(TypeId);
    DEFAULT_MOVE_CTOR(TypeId);
    virtual DEFAULT_DTOR(TypeId);

    operator TypeIdType() const {
        return id;
    }

private:
    TypeIdType id;
};

#define TYPE_LIST(DEF)  \
    DEF(VOID)           \
    DEF(I8)             \
    DEF(I16)            \
    DEF(I32)            \
    DEF(I64)            \
    DEF(U8)             \
    DEF(U16)            \
    DEF(U32)            \
    DEF(U64)            \
    DEF(REF)

enum class OperandType {
#define TYPE_DEF(name, ...) name,
    TYPE_LIST(TYPE_DEF)
#undef TYPE_DEF
    INVALID,
    NUM_TYPES = INVALID
};

constexpr inline const char *getTypeName(OperandType type) {
    std::array<const char *, static_cast<size_t>(OperandType::NUM_TYPES)> names{
#define TYPE_NAME(name, ...) #name,
    TYPE_LIST(TYPE_NAME)
#undef TYPE_NAME
    };
    return names[static_cast<size_t>(type)];
}

constexpr std::array<uint64_t, static_cast<size_t>(OperandType::NUM_TYPES)> maxValues{
    0,
    std::numeric_limits<int8_t>::max(),
    std::numeric_limits<int16_t>::max(),
    std::numeric_limits<int32_t>::max(),
    std::numeric_limits<int64_t>::max(),
    std::numeric_limits<uint8_t>::max(),
    std::numeric_limits<uint16_t>::max(),
    std::numeric_limits<uint32_t>::max(),
    std::numeric_limits<uint64_t>::max(),
    0
};

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <typename T>
concept ValidOpType = IsAnyOf<T, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>;

constexpr inline size_t GetTypeBitSize(OperandType type) {
    switch (type) {
    case OperandType::I8:
    case OperandType::U8:
        return 8;
    case OperandType::I16:
    case OperandType::U16:
        return 16;
    case OperandType::I32:
    case OperandType::U32:
        return 32;
    case OperandType::I64:
    case OperandType::U64:
        return 64;
    default:
        UNREACHABLE("");
        return 0;
    }
}

constexpr inline int64_t ToSigned(uint64_t value, OperandType type) {
    switch (type) {
    case OperandType::U8:
        return static_cast<int64_t>(static_cast<int8_t>(value));
    case OperandType::U16:
        return static_cast<int64_t>(static_cast<int16_t>(value));
    case OperandType::U32:
        return static_cast<int64_t>(static_cast<int32_t>(value));
    case OperandType::U64:
        return static_cast<int64_t>(static_cast<int64_t>(value));
    default:
        return static_cast<int64_t>(value);
    }
}

constexpr inline uint64_t GetMaxValue(OperandType type) {
    ASSERT(type != OperandType::INVALID);
    return maxValues[static_cast<size_t>(type)];
}

constexpr inline bool IsIntegerType(OperandType type) {
    auto t = utils::to_underlying(type);
    return utils::to_underlying(OperandType::I8) <= t && t <= utils::to_underlying(OperandType::U64);
}
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_TYPES_H_
