#pragma once

#ifndef PCC_TOKENIZE_HPP
#define PCC_TOKENIZE_HPP

#include "string_view.hpp"
#include "arena.hpp"

#include <stdint.h>
#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>

enum tok_type {
  /*
   * Enums by Sean Barrett.
   * https://github.com/nothings/stb/blob/master/stb_c_lexer.h
   */
  END_OF_FILE,
  PARSE_ERROR,

  NONE,

  INTLIT,
  FLOATLIT,

  ID,

  DQSTRING,
  SQSTRING,

  CHARLIT, // single character operators
  EQ,
  NOTEQ,
  LESSEQ,
  GREATEREQ,
  ANDAND,
  OROR,
  SHL,
  SHR,
  PLUSPLUS,
  MINUSMINUS,
  PLUSEQ,
  MINUSEQ,
  MULEQ,
  DIVEQ,
  MODEQ,
  ANDEQ,
  OREQ,
  XOREQ,
  ARROW,
  EQARROW,
  SHLEQ, 
  SHREQ,
  POUNDPOUND,
  ELIPSES,
};

struct number {
  union {
    uint64_t intlit;
    struct string_view float_lit;
  };

  struct string_view suff;
};

struct token {
  enum tok_type type;
  const char *location;
  union {
    struct string_view str;
    struct number number;
    char charlit;
  };
};

struct string_view get_string(sv_map &strings_map, const char *start, char q,
                              class arena *strings, const char **end);

struct string_view get_id(sv_map &strings_map, const char *start, 
                          class arena *strings, const char **end);

void get_number(sv_map &strings_map, struct token *token, const char *start,
                class arena *strings, const char **end);

/* 
 * style -> '/' or '*'
 * where, '/' -> c++ style
 *        '*' -> c style
 */
const char *skip_comment(const char *start, char style);
char resolve_esc(char c);

#endif // PCC_TOKENIZE_HPP
