#include "frontend/lLexer.hh"

yy::location Lexer::getCurLocation() const { return curLocation; }

bool Lexer::isEmptyLine(const char *str) {
  char sym = *str;
  return std::isspace(sym) && std::iscntrl(sym);
}

int Lexer::getLastLine() const { return lastNumOfLine; }

void Lexer::updCurLoc() {
  auto curNumOfLine = lineno();

  auto prevEndColumn = curLocation.end.column;
  curLocation.begin.line = curLocation.end.line = curNumOfLine;

  if (isEmptyLine(yytext))
    curLocation.begin.column = curLocation.end.column = 1;

  else {
    curLocation.begin.column = prevEndColumn;
    curLocation.end.column = curLocation.begin.column + YYLeng();
  }
  lastNumOfLine = curNumOfLine;
}
