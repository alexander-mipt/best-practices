#ifndef __INCLUDE_FRONTEND_LLEXER_HH__
#define __INCLUDE_FRONTEND_LLEXER_HH__

#include <cctype>
#include <iostream>

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif /* yyFlexLexer */

#include "parser.hh"

class Lexer final : public yyFlexLexer {
private:
  yy::location curLocation{};
  int lastNumOfLine{};

public:
  Lexer() = default;

  Lexer(const Lexer &flx) = delete;
  Lexer &operator=(const Lexer &) = delete;

  Lexer(Lexer &&flx) = delete;
  Lexer &operator=(Lexer &&) = delete;

  Lexer(std::istream &in, std::ostream &out) : yyFlexLexer{in, out} {}

  yy::location getCurLocation() const;
  int getLastLine() const;

  static bool isEmptyLine(const char *str);

  void updCurLoc();

  int yylex() override;

  ~Lexer() override = default;
};

#endif // __INCLUDE_FRONTEND_LLEXER_HH__
