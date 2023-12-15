#include "executor/executor.hh"

namespace leech {
StackFrame::StackFrame(const FuncMeta *pmeta) : pmeta_(pmeta) {
  if (pmeta_ == nullptr)
    throw std::invalid_argument("Creating stack frame w/ Null func meta");

  for (auto &name : pmeta_->names)
    vars_.emplace(name, nullptr);
}

void StackFrame::push(pLeechObj obj) {
  if (nullptr == obj)
    throw std::invalid_argument("Trying to push nullptr into stackframe");

  dataStack_.push(obj);
}

State::State(LeechFile *pfile) : pFile(pfile) {
  if (nullptr == pFile)
    throw std::invalid_argument("Trying to execute null leech file");
  auto &meta = pFile->meta;
  auto *mainFrame = &meta.funcs.at(std::string(kMainFuncName));
  pc = mainFrame->addr;
  funcStack.emplace(mainFrame);
}

void Executor::execute() {
  auto &fStack = state_.funcStack;
  while (fStack.size() != 0) {
    auto &curInst = state_.getInst(state_.pc);

    state_.nextPC.reset();

    curInst.execute(state_);

    state_.pc = state_.nextPC.value_or(state_.pc + 1);
  }
}

} // namespace leech
