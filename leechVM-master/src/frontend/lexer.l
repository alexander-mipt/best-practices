%option c++
%option noyywrap debug
%option yylineno
%option yyclass="Lexer"
%{
#include "parser.hh"
#include "frontend/lLexer.hh"
#define YY_USER_ACTION updCurLoc();
%}

WS [ \t\n\v]+
INTEGER [0]|[1-9][0-9]*
NAME [_]*[a-zA-Z][a-zA-Z0-9_]*

%%
".func"     return yy::parser::token_type::FUNC_DECL;
".cpool"    return yy::parser::token_type::CPOLL_DECL;
".names"    return yy::parser::token_type::NAMES_DECL;
".code"     return yy::parser::token_type::CODE_DECL;
".label"    return yy::parser::token_type::LABEL;
":"         return yy::parser::token_type::COLON;
","         return yy::parser::token_type::COMMA;
"("         return yy::parser::token_type::LRB;
")"         return yy::parser::token_type::RRB;
{INTEGER}   return yy::parser::token_type::INTEGER;
{NAME}      return yy::parser::token_type::IDENTIFIER;
{WS}
%%
