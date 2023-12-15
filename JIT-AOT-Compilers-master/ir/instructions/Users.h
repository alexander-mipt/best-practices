#ifndef JIT_AOT_COMPILERS_COURSE_USERS_H_
#define JIT_AOT_COMPILERS_COURSE_USERS_H_

#include "AllocatorUtils.h"
#include "logger.h"
#include <span>


namespace ir {
class InstructionBase;

class Users {
public:
    explicit Users(std::pmr::memory_resource *memResource)
        : users(memResource)
    {
        ASSERT(memResource);
    }
    Users(std::span<InstructionBase *> instrs, std::pmr::memory_resource *memResource)
        : users(instrs.begin(), instrs.end(), memResource)
    {
        ASSERT(memResource);
    }
    template <typename AllocatorT>
    Users(const std::vector<InstructionBase *, AllocatorT> &instrs,
          std::pmr::memory_resource *memResource)
        : users(instrs.begin(), instrs.end(), memResource)
    {
        ASSERT(memResource);
    }
    NO_COPY_SEMANTIC(Users);
    NO_MOVE_SEMANTIC(Users);
    virtual DEFAULT_DTOR(Users);

    size_t UsersCount() const {
        return users.size();
    }

    const std::pmr::vector<InstructionBase *> &GetUsers() const {
        return users;
    }
    auto GetUsers() {
        return std::span(users);
    }

    void ReserveUsers(size_t usersCount) {
        users.reserve(usersCount);
    }
    void AddUser(InstructionBase *instr) {
        ASSERT(instr);
        users.push_back(instr);
    }
    void AddUsers(std::span<InstructionBase *> instrs) {
        users.reserve(users.size() + instrs.size());
        for (auto &&it : instrs) {
            users.push_back(it);
        }
    }

    void RemoveUser(InstructionBase *instr) {
        auto iter = std::find(users.begin(), users.end(), instr);
        ASSERT(iter != users.end());
        *iter = users.back();
        users.pop_back();
    }

    void ReplaceUser(InstructionBase *oldInstr, InstructionBase *newInstr) {
        auto iter = std::find(users.begin(), users.end(), oldInstr);
        ASSERT(iter != users.end());
        *iter = newInstr;
    }

    void SetNewUsers(std::pmr::vector<InstructionBase *> &&newUsers) {
        users = std::move(newUsers);
    }
    void SetNewUsers(const std::pmr::vector<InstructionBase *> &newUsers) {
        users = newUsers;
    }

protected:
    std::pmr::vector<InstructionBase *> users;
};
}   // namespace ir

#endif  // JIT_AOT_COMPILERS_COURSE_USERS_H_
