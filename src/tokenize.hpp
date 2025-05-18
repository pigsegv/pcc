#pragma once

#ifndef PCC_TOKENIZE_HPP
#define PCC_TOKENIZE_HPP

#include <stdint.h>

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

struct string_view {
  const char *view;
  uint64_t len;
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

struct string_view get_string(const char *start, char q,
                              class arena *scratch, class arena *strings, 
                              const char **end);
struct string_view get_id(const char *start, class arena *scratch,
                          class arena *strings, const char **end);
void get_number(struct token *token, const char *start, 
                class arena *scratch, class arena *strings,
                const char **end);
const char *skip_comment(const char *start, char style);
char resolve_esc(char c);

#endif // PCC_TOKENIZE_HPP
