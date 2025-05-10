#pragma once

#ifndef PCC_LEXER_H
#define PCC_LEXER_H

#include <string>
#include <cstdint>

namespace pcc::lexer {

enum tok_type {
  /*
   * Enums by Sean Barrett.
   * https://github.com/nothings/stb/blob/master/stb_c_lexer.h
   */
   PARSE_ERROR,
   NONE,
   INTLIT,
   FLOATLIT,
   ID,
   DQSTRING,
   SQSTRING,
   CHARLIT,
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
};

struct string_view {
  const char *view;
  uint64_t len;
};

struct token {
  enum tok_type type;
  union {
    struct string_view str;
    uint64_t intlit; 
    char charlit;
  };
};

class lexer {
public:
  lexer(const char *src);
  ~lexer();

  struct token get_tok(const char **end);

private:
  const char *m_src;
};

};

#endif // PCC_LEXER_H
