#pragma once

#ifndef PCC_LEXER_H
#define PCC_LEXER_H

#include "tokenize.hpp"
#include "arena.hpp"

#include <string>
#include <stdint.h>
#include <optional>

class lexer {
public:
  lexer(void) = delete;
  lexer(const char *src, const char *filepath, class arena *scratch,
        class arena *strings);
  ~lexer(void);

  class lexer &operator =(class lexer &) = delete;
  class lexer &operator =(class lexer &&) = delete;

  struct token get_tok(void);
  struct token get_tok_and_expect(enum tok_type type, ...);

  struct token peek(void); // TODO

private:
  std::optional<struct token> m_peeked;

  char *m_filepath;

  const char *m_src;
  const char *m_cursor;

  class arena *m_scratch;
  class arena *m_strings;
};

#endif // PCC_LEXER_H
