%require "3.5"
%language "c++"
%skeleton "lalr1.cc"

%define parse.trace
%define parse.lac full
%locations
%define api.value.type variant
%define parse.error verbose

%param {Driver* driver}

%code requires {
    #include <string>
    #include <memory>
    namespace yy { class Driver; }

    #include "common/common.hh"
    #include "common/opcodes.hh"
    #include "leechobj/leechobj.hh"
}

%code {
    #include "frontend/frontend.hh"

namespace yy
  { parser::token_type yylex(parser::semantic_type* yylval, parser::location_type* yylloc, Driver* driver); }
}

%token <std::string> IDENTIFIER
%token <int> INTEGER
%token FUNC_DECL              ".func"
       CPOLL_DECL             ".cpool"
       NAMES_DECL             ".names"
       CODE_DECL              ".code"
       LABEL                  ".label"
       COLON                  ":"
       LRB                    "("
       RRB                    ")"
       COMMA                  ","

%nterm<leech::Instruction>      instruction;
%nterm<leech::pLeechObj>        leechObj
%nterm<leech::pLeechObj>        primitiveTy
%nterm<leech::pLeechObj>        tupple
%nterm<std::string>             nameEntry

%%

program:            funcList                                  {};

funcList:           funcList func                             {};
                  | func                                      {};

func:               funcHeader
                        cpollBlock namesBlock codeBlock       {};

funcHeader:         FUNC_DECL IDENTIFIER INTEGER              {
                                                                driver->currentFunc_ = $2;
                                                                driver->leechFile_->meta.funcs[$2].addr = driver->globalInstrCount_;
                                                                driver->leechFile_->meta.funcs[$2].argNum = $3;
                                                                driver->instrCount_ = 0;
                                                                driver->labels_.clear();
                                                                driver->forwardBranches_.clear();
                                                              }

cpollBlock:         CPOLL_DECL constants                      {};
                  | /* empty */                               {};

constants:          constants leechObjEntry                   {};
                  | leechObjEntry                             {};

leechObjEntry:      INTEGER COLON leechObj                    {
                                                                auto&& currentFunc = driver->currentFunc_;
                                                                driver->leechFile_->meta.funcs[currentFunc].cstPool.emplace_back($3);
                                                              };

leechObj:           primitiveTy                               { $$ = $1; };
                  | tupple                                    { $$ = $1; };

primitiveTy:        IDENTIFIER                                { $$ = std::make_shared<leech::StringObj>($1); };
                  | INTEGER                                   { $$ = std::make_shared<leech::NumberObj<std::int64_t>>($1); };

tupple:             LRB tuppleArgs RRB                        {
                                                                auto&& args = driver->tupleArgs_;
                                                                $$ = std::make_shared<leech::TupleObj>(args.begin(), args.end());
                                                                args.clear();
                                                              };
tuppleArgs:         tuppleArgs COMMA primitiveTy              { driver->tupleArgs_.emplace_back($3); };
                  | primitiveTy                               { driver->tupleArgs_.emplace_back($1); };

namesBlock:         NAMES_DECL names                          {};
                  | /* empty */                               {};

names:              names nameEntry                           {
                                                                auto&& currentFunc = driver->currentFunc_;
                                                                driver->leechFile_->meta.funcs[currentFunc].names.emplace_back($2);
                                                              };
                  | nameEntry                                 {
                                                                auto&& currentFunc = driver->currentFunc_;
                                                                driver->leechFile_->meta.funcs[currentFunc].names.emplace_back($1);
                                                              };

nameEntry:          INTEGER COLON IDENTIFIER                  { $$ = $3; };

codeBlock:          CODE_DECL code                            {};
                  | /* empty */                               {};

code:               code codeEntry                            {};
                  | codeEntry                                 {};

codeEntry:          LABEL IDENTIFIER                          {
                                                                auto&& it = driver->forwardBranches_.find($2);
                                                                if (it != driver->forwardBranches_.end()) {
                                                                  driver->leechFile_->code[it->second].setArg(driver->instrCount_);
                                                                }
                                                                driver->labels_[$2] = driver->instrCount_;
                                                              };
                  | instruction                               {
                                                                driver->leechFile_->code.push_back($1);
                                                                ++driver->instrCount_;
                                                                ++driver->globalInstrCount_;
                                                              };

instruction:        IDENTIFIER                                {
                                                                // TODO: process invalid opcode
                                                                auto opcode = leech::OpcodeConv::fromName($1).value();
                                                                $$ = leech::Instruction(opcode);
                                                              };
                  | IDENTIFIER INTEGER                        {
                                                                // TODO: process invalid opcode
                                                                auto opcode = leech::OpcodeConv::fromName($1).value();
                                                                $$ = leech::Instruction(opcode, $2);
                                                              };
                  | IDENTIFIER COLON IDENTIFIER               {
                                                                // TODO: process invalid opcode
                                                                auto opcode = leech::OpcodeConv::fromName($1).value();
                                                                auto&& it = driver->labels_.find($3);
                                                                if (it != driver->labels_.end()) {
                                                                  $$ = leech::Instruction(opcode, it->second);
                                                                } else {
                                                                  driver->forwardBranches_[$3] = driver->globalInstrCount_;
                                                                  $$ = leech::Instruction(opcode);
                                                                }
                                                              };
%%

namespace yy {
  void parser::error (const parser::location_type& location, const std::string& string)
  {
    std::cerr << string << " in (line.column): "<< location << std::endl;
  }

  parser::token_type yylex(parser::semantic_type* yylval, parser::location_type* yylloc, Driver* driver)
  {
    return driver->yylex(yylval, yylloc);
  }
}
