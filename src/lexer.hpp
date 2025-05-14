#pragma once

#ifndef PCC_LEXER_H
#define PCC_LEXER_H

#include "arena.hpp"

#include <string>
#include <cstdint>

namespace pcc {

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

struct token {
  enum tok_type type;
  const char *location;
  union {
    struct string_view str;
    uint64_t intlit; 
    char charlit;
  };
};

class lexer {
public:
  lexer(const char *src);
  ~lexer(void);

  struct token get_tok(void);

private:
  const char *m_src;
  const char *m_cursor;

  class arena m_scratch;
  class arena m_strings;
};

};

#endif // PCC_LEXER_H
