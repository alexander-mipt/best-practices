#ifndef JIT_AOT_COMPILERS_COURSE_VALUE_LOCATION_H_
#define JIT_AOT_COMPILERS_COURSE_VALUE_LOCATION_H_

#include "ArchInfoBase.h"


namespace ir::codegen {
enum class LocationType : uint8_t {
    REGISTER = 0,
    STACK,
    INVALID,
};

constexpr inline const char *GetLocationName(LocationType type) {
    if (type == LocationType::REGISTER) {
        return "Reg";
    } else if (type == LocationType::STACK) {
        return "Slot";
    }
    return "Invalid location";
}

class ValueLocation final {
public:
    ValueLocation() : type(LocationType::INVALID), id(0) {}
    ValueLocation(LocationType type, LocationIdType id) : type(type), id(id) {}
    DEFAULT_COPY_SEMANTIC(ValueLocation);
    DEFAULT_MOVE_SEMANTIC(ValueLocation);
    DEFAULT_DTOR(ValueLocation);

    LocationType GetType() const {
        return type;
    }
    LocationIdType GetId() const {
        return id;
    }

    constexpr bool operator==(const ValueLocation &other) const {
        return type == other.type && id == other.id;
    }

private:
    LocationType type;
    LocationIdType id;
};

inline std::ostream &operator<<(std::ostream &os, const ValueLocation &loc) {
    os << '<' << GetLocationName(loc.GetType()) << ' ' << loc.GetId() << '>';
    return os;
}
}   // namespace ir::codegen

#endif  // JIT_AOT_COMPILERS_COURSE_VALUE_LOCATION_H_
