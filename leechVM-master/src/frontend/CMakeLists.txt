set(GRAMMAR_GEN ${CMAKE_CURRENT_BINARY_DIR})

bison_target(parser parser.y ${GRAMMAR_GEN}/parser.cc
             DEFINES_FILE ${GRAMMAR_GEN}/parser.hh)

message(STATUS "bison files generated in: ${BISON_parser_OUTPUTS}")

flex_target(lexer lexer.l ${GRAMMAR_GEN}/lexer.cc)
message(STATUS "flex files generated in: ${FLEX_lexer_OUTPUTS}")

add_flex_bison_dependency(lexer parser)

add_library(lex ${FLEX_lexer_OUTPUTS})
add_library(par ${BISON_parser_OUTPUTS} ${CMAKE_SOURCE_DIR}/src/leechobj/leechobj.cc)
target_include_directories(
  par SYSTEM
  PUBLIC ${GRAMMAR_GEN}
  PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(
    lex SYSTEM
    PUBLIC ${GRAMMAR_GEN}
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(par PRIVATE cxx_std_20)
target_compile_features(lex PRIVATE cxx_std_20)

add_library(frontend frontend.cc lLexer.cc)
target_link_libraries(frontend PUBLIC lex par)
