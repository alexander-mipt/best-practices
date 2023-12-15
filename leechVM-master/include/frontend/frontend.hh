#ifndef __INCLUDE_FRONTEND_FRONTEND_HH__
#define __INCLUDE_FRONTEND_FRONTEND_HH__

#include <array>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "lLexer.hh"
#include "leechfile/leechfile.hh"

namespace yy {

class Driver final {
public:
  ~Driver() = default;
  Driver(std::istream &in, std::ostream &out);

  parser::token_type yylex(parser::semantic_type *yylval,
                           parser::location_type *yylloc);
  bool parse();
  [[nodiscard]] auto getLeechFile() const { return leechFile_; }

  friend parser;

private:
  static inline constexpr size_t numTokens = 10;

  std::unique_ptr<Lexer> lexer_{};
  std::shared_ptr<leech::LeechFile> leechFile_{
      std::make_shared<leech::LeechFile>()};
  std::unordered_map<std::string, leech::FuncAddr> labels_{};
  std::unordered_map<std::string, leech::FuncAddr> forwardBranches_{};
  leech::Tuple tupleArgs_{};
  std::string currentFunc_{};
  std::size_t instrCount_ = 0;
  std::size_t globalInstrCount_ = 0;
};

} // namespace yy

#endif // __INCLUDE_FRONTEND_FRONTEND_HH__
