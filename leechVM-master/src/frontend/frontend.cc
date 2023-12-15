#include "frontend/frontend.hh"

namespace yy {

Driver::Driver(std::istream &in, std::ostream &out)
    : lexer_{std::make_unique<Lexer>(in, out)} {}

bool Driver::parse() {
  parser parser(this);
  bool res = parser.parse();
  return !res;
}

parser::token_type Driver::yylex(parser::semantic_type *yylval,
                                 parser::location_type *yylloc) {
  parser::token_type token = static_cast<parser::token_type>(lexer_->yylex());
  if (token == yy::parser::token_type::IDENTIFIER) {
    std::string name(lexer_->YYText());
    parser::semantic_type tmp;
    tmp.as<std::string>() = name;
    yylval->swap<std::string>(tmp);
  } else if (token == yy::parser::token_type::INTEGER) {
    yylval->as<int>() = std::atoi(lexer_->YYText());
  }

  *yylloc = lexer_->getCurLocation();
  return token;
}

} // namespace yy
