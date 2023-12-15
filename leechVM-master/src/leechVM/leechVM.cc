#include "leechVM/leechVM.hh"

namespace leech {

void LeechVM::run() {
  Executor exec(leechFile_.get());
  exec.execute();
}

void LeechVM::generateLeechFile(std::istream &in, bool isFromBinary) {
  if (isFromBinary) {
    leechFile_ = std::make_shared<LeechFile>(LeechFile::deserialize(in));
  } else {
    std::stringstream out;
    yy::Driver driver{in, out};
    if (!driver.parse()) {
      throw std::runtime_error(out.str());
    }
    leechFile_ = driver.getLeechFile();
  }
}

void LeechVM::dumpBinary(std::ostream &out) { leechFile_->serialize(out); }
} // namespace leech
