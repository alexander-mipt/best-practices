#ifndef JIT_AOT_COMPILERS_COURSE_INPUT_H_
#define JIT_AOT_COMPILERS_COURSE_INPUT_H_

namespace ir {
class InstructionBase;

class Input final {
public:
    Input() : instr(nullptr) {}
    Input(InstructionBase *instr) : instr(instr) {}
    DEFAULT_COPY_SEMANTIC(Input);
    DEFAULT_MOVE_SEMANTIC(Input);
    DEFAULT_DTOR(Input);

    InstructionBase *GetInstruction() {
        return instr;
    }
    const InstructionBase *GetInstruction() const {
        return instr;
    }

    void SetInstruction(InstructionBase *newInstr) {
        instr = newInstr;
    }

    InstructionBase *operator->() {
        return instr;
    }
    const InstructionBase *operator->() const {
        return instr;
    }

private:
    InstructionBase *instr;
};

inline bool operator ==(const Input& lhs, const Input &rhs) {
    return lhs.GetInstruction() == rhs.GetInstruction();
}

inline bool operator ==(const InstructionBase *lhs, const Input &rhs) {
    return lhs == rhs.GetInstruction();
}

inline bool operator ==(const Input &lhs, const InstructionBase *rhs) {
    return lhs.GetInstruction() == rhs;
}
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_INPUT_H_
