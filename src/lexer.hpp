#pragma once

#ifndef PCC_LEXER_H
#define PCC_LEXER_H

#include "tokenize.hpp"
#include "arena.hpp"
#include "string_view.hpp"

#include <string>
#include <stdint.h>
#include <optional>
#include <vector>
#include <unordered_map>

class lexer {
public:
  lexer(void) = delete;
  lexer(const char *src, const char *filepath, class arena *strings);
  ~lexer(void);

  class lexer &operator =(class lexer &) = delete;
  class lexer &operator =(class lexer &&) = delete;

  /*
   * All string views returned through the token are 
   * guaranteed to be null-terminated
   */
  struct token get_tok(void);
  struct token get_tok_and_expect(int type, ...);

  struct token peek(void); // TODO

  const char *get_filepath(void) { return m_filepath; }
  const char *get_src(void) { return m_src; }

private:
  std::optional<struct token> m_peeked;

  char *m_filepath;

  const char *m_src;
  const char *m_cursor;

  class arena *m_strings;

  sv_map m_strings_map;
};

#endif // PCC_LEXER_H
