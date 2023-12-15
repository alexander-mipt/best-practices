#ifndef __INCLUDE_EXECUTOR_EXECUTOR_HH__
#define __INCLUDE_EXECUTOR_EXECUTOR_HH__

#include <functional>
#include <optional>
#include <stack>
#include <string_view>

#include "leechfile/leechfile.hh"

namespace leech {

constexpr std::string_view kMainFuncName = "main";

class StackFrame final {
  const FuncMeta *pmeta_ = nullptr;
  std::stack<pLeechObj> dataStack_{};
  // TODO: Store pc in frame
  std::uint64_t retAddr_ = {};
  std::unordered_map<std::string, pLeechObj> vars_{};

public:
  explicit StackFrame(const FuncMeta *pmeta);

  StackFrame(const StackFrame &) = delete;
  StackFrame &operator=(const StackFrame &) = delete;
  StackFrame(StackFrame &&) = default;
  StackFrame &operator=(StackFrame &&) = default;

  template <class T, typename... Args> void emplace(Args &&...args) {
    dataStack_.emplace(new T(std::forward<Args>(args)...));
  }

  [[nodiscard]] auto getRet() const { return retAddr_; }
  template <class T> void setRet(T val) { retAddr_ = val; }

  template <class InpIt> void fillArgs(InpIt beg, InpIt end) {
    for (auto &name : pmeta_->names) {
      if (beg == end)
        break;
      setVar(name, *beg++);
    }
  }

  void setVar(std::string_view name, pLeechObj obj) {
    setVar(std::string(name), obj);
  }

  [[nodiscard]] auto stackSize() const { return dataStack_.size(); }

  void setVar(const std::string &name, pLeechObj obj) { vars_.at(name) = obj; }

  [[nodiscard]] auto getVar(const std::string &name) const {
    return vars_.at(name);
  }

  [[nodiscard]] auto getVar(std::string_view name) const {
    return getVar(std::string(name));
  }

  void push(pLeechObj obj);

  [[nodiscard]] auto getConst(ArgType idx) const {
    return pmeta_->cstPool.at(idx);
  }

  [[nodiscard]] std::string_view getName(ArgType idx) const {
    return pmeta_->names.at(idx);
  }

  [[nodiscard]] auto top() const {
    if (!dataStack_.size())
      throw std::runtime_error("Trying to top from empty stack!");
    return dataStack_.top();
  }

  [[nodiscard]] auto popGetTos() {
    auto tos = top()->clone();
    pop();
    return tos;
  }

  void pop() {
    if (!dataStack_.size())
      throw std::runtime_error("Trying to pop from empty stack!");
    dataStack_.pop();
  }
};

using FuncStack = std::stack<StackFrame>;

struct State final {
  FuncStack funcStack{};
  LeechFile *pFile{};
  std::uint64_t pc{};
  std::optional<std::uint64_t> nextPC{};

  State() = default;
  explicit State(LeechFile *pfile);

  State(const State &) = default;
  State &operator=(const State &) = default;

  [[nodiscard]] const auto &getInst(std::uint64_t idx) const {
    return pFile->code.at(idx);
  }

  auto &getCurFrame() { return funcStack.top(); }
};

class Executor final {
  State state_{};

public:
  explicit Executor(LeechFile *leechFile) : state_(leechFile) {}

  void execute();
};

} // namespace leech

#endif // __INCLUDE_EXECUTOR_EXECUTOR_HH__
