#pragma once

#ifndef PCC_LEXER_H
#define PCC_LEXER_H

#include "tokenize.hpp"
#include "arena.hpp"

#include <string>
#include <stdint.h>

class lexer {
public:
  lexer(const char *src, const char *filepath);
  ~lexer(void);

  class lexer &operator =(class lexer &);
  class lexer &operator =(class lexer &&);

  struct token get_tok(void);

private:
  char *m_filepath;

  const char *m_src;
  const char *m_cursor;

  class arena m_scratch;
  class arena m_strings;
};

#endif // PCC_LEXER_H
