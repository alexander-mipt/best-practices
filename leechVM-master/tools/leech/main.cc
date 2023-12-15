#include <filesystem>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include "leechVM/leechVM.hh"
#include "timer/timer.hh"

namespace fs = std::filesystem;

int main(int argc, char **argv) try {
  CLI::App app{"LeechVM"};
  fs::path input{};
  fs::path binaryOutput{};
  bool fromBinary = false;
  app.add_option("input", input, "input file")->required();
  app.add_option("--dump", binaryOutput, "leech -> dinary dump");
  app.add_flag("--bin", fromBinary, "execute from binary");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  std::fstream in(input.c_str());
  if (!in.is_open()) {
    throw std::invalid_argument("can't find input file");
  }

  leech::LeechVM vm;
  vm.generateLeechFile(in, fromBinary);
  if (!binaryOutput.empty()) {
    std::ofstream out(binaryOutput.c_str());
    vm.dumpBinary(out);
  } else {
    timer::Timer timer;
    vm.run();
    auto time = timer.elapsed_mcs();
    std::cout << "Time: " << static_cast<double>(time) * 1e-3 << " ms"
              << std::endl;
  }
} catch (const std::exception &e) {
  std::cerr << e.what() << std::endl;
  return 1;
}
