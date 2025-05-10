#include "lexer.hpp"
#include "arena.hpp"

#include <cstring>
#include <cctype>

namespace pcc::lexer {
char resolve_esc(char c) {
  switch (c) {
    case 'n':
      return '\n';
    case 't':
      return '\t';
    case 'r':
      return '\r';
    case 'b':
      return '\b';
    case 'f':
      return '\f';
    case 'a':
      return '\a';
    case 'v':
      return '\v';
    case '\\':
      return '\\';
    case '\'':
      return '\'';
    case '\"':
      return '\"';
    case '?':
      return '\?';
    case '0': 
      return '\0';
    default:
      return c;
  }
}

static struct string_view get_string(char *start) {
  bool escape = false;
  struct string_view sv = { };

  while (*start) {
    if (escape) {

    }
  }
}

struct token lexer::get_tok(const char **end) {
  struct token tok = {
    .type = NONE,
  };

  const char *tmp = m_src;
  while (*tmp) {
  }
  return tok;
}

lexer::lexer(const char *src) {
  size_t len = std::strlen(src);

  char *tmp = new char[len + 1];
  std::memcpy(tmp, src, len);
  tmp[len] = 0;

  m_src = tmp;
}

lexer::~lexer() {
  delete[] m_src;
  m_src = NULL;
}

};
