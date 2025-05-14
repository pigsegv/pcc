#include "lexer.hpp"
#include "arena.hpp"
#include "dynamic-array.hpp"

#include <cstring>
#include <cctype>

namespace pcc {
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

static struct string_view get_string(const char *start, char q, 
                                     class arena *scratch, 
                                     class arena *strings) {
  if (q != '\'' || q != '"') {
    assert(0 && "Unreachable");
  }

  bool escape = false;
  struct string_view sv = { };

  struct dynamic_array<char> scratch_da(scratch);

  while (*start) {
    if (escape) {
      if (*start == '\n') {
        start++;
        continue;
      }

      scratch_da.append(resolve_esc(*start));
      escape = false;

    } else {
      if (*start == '\\') {
        escape = true;

      } else if (*start == q) {
        char *tmp = strings->alloc<char>(scratch_da.count);
        sv.len = scratch_da.count;
        for (uint64_t i = 0; i < scratch_da.count; i++) {
          tmp[i] = scratch_da[i];
        }

        sv.view = tmp;

        return sv;

      } else if (*start == '\n') {
          return { nullptr, 0 }; // Parse error

      } else {
        scratch_da.append(*start);
      }
    }

    start++;
  }

  return { nullptr, 0 }; // Parse error
}

/* 
 * style -> '/' or '*'
 * where, '/' -> c++ style
 *        '*' -> c style
 */
static const char *skip_comment(const char *start, char style) {
  while (*start) {
    if (style == '/') {
      if (*start == '\n') {
        return start + 1;
      }

      if (*start == '\\' && *(start + 1) == '\n') {
        start += 2;
        continue;
      }

    } else if (style == '*') {
      if (*start == '*' && *(start + 1) == '/') {
        return start + 2;
      }

    } else {
      assert(0 && "Unreachable");
    }

    start++;
  }

  return start;
}

static struct string_view get_id(const char *start, class arena *scratch,
                                 class arena *strings) {
  struct string_view sv;
  return sv;
}

struct token lexer::get_tok(void) {
  struct token tok;

  const char *tmp = m_cursor;
  while (*tmp) {
    tok.location = tmp;

    switch (*tmp) {
      case '/': {
        switch (*(tmp + 1)) {
          case '/': case '*':
            tmp = skip_comment(tmp + 2, *(tmp + 1));
            continue;

          case '=': 
            tok.type = DIVEQ; 
            m_cursor = tmp + 2;
            break;
            

          default: 
            tok.type = OPERATOR; 
            tok.op = '/';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '*': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = MULEQ;
            m_cursor = tmp + 2;
            break;

          default: 
            tok.type = OPERATOR;
            tok.op = '*';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '+': {
        switch (*(tmp + 1)) {
          case '+': 
            tok.type = PLUSPLUS;
            m_cursor += 2;
            break;

          case '=': 
            tok.type = PLUSEQ;
            m_cursor += 2;
            break;

          default:
            tok.type = OPERATOR;
            tok.op = '+';
            m_cursor += 1;
        }

        return tok;
      }

      case '-': {
        switch(*(tmp + 1)) {
          case '-': 
            tok.type = MINUSMINUS;
            m_cursor += 2;
            break;

          case '=': 
            tok.type = MINUSEQ;
            m_cursor += 2;
            break;

          case '>': 
            tok.type = ARROW;
            m_cursor += 2;
            break;

          default: 
            tok.type = OPERATOR;
            tok.op = '-';
            m_cursor += 1;
        }

        return tok;
      }

      case '=': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = EQ;
            m_cursor += 2;
            break;


          default: 
            tok.type = OPERATOR;
            tok.op = '=';
            m_cursor += 1;
        }

        return tok;
      }

      case '!': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = NOTEQ;
            m_cursor += 2;
            break;

          default: 
            tok.type = OPERATOR;
            tok.op = '!';
            m_cursor += 1;
        }

        return tok;
      }

      case '<': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = LESSEQ;
            m_cursor += 2;
            break;

          case '<': {
            if (*(tmp + 2) == '=') {
              tok.type = SHLEQ;
              m_cursor += 3;
            } else {
              tok.type = SHL;
              m_cursor += 2;
            }

          } break;

          default: 
            tok.type = OPERATOR;
            tok.op = '<';
            m_cursor += 1;
        }

        return tok;
      }

      case '>': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = GREATEREQ;
            m_cursor += 2;
            break;

          case '>': {
            if (*(tmp + 2) == '=') {
              tok.type = SHREQ;
              m_cursor += 3;
            } else {
              tok.type = SHR;
              m_cursor += 2;
            }

          } break;

          default: 
            tok.type = OPERATOR;
            tok.op = '>';
            m_cursor += 1;
        }

        return tok;
      }

      case '%': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = MODEQ;
            m_cursor += 2;
            break;

          default: 
            tok.type = OPERATOR;
            tok.op = '%';
            m_cursor += 1;
        }

        return tok;
      }

      case '&': {
        switch(*(tmp + 1)) {
          case '&': 
            tok.type = ANDAND;
            m_cursor += 2;
            break;

          case '=': 
            tok.type = ANDEQ;
            m_cursor += 2;
            break;

          default: 
            tok.type = OPERATOR;
            tok.op = '&';
            m_cursor += 1;
        }

        return tok;
      }

      case '|': {
        switch(*(tmp + 1)) {
          case '|': 
            tok.type = OROR;
            m_cursor += 2;
            break;

          case '=': 
            tok.type = OREQ;
            m_cursor += 2;
            break;

          default: 
            tok.type = OPERATOR;
            tok.op = '|';
            m_cursor += 1;
        }

        return tok;
      }

      case '^': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = XOREQ;
            m_cursor += 2;
            break;

          default: 
            tok.type = OPERATOR;
            tok.op = '^';
            m_cursor += 1;
        }

        return tok;
      }

      case '.': {
        switch(*(tmp + 1)) {
          case '.': {
            if (*(tmp + 2) == '.') {
              tok.type = ELIPSES;
              m_cursor += 3;
            } 

          } break;

          default: 
            tok.type = OPERATOR;
            tok.op = '.';
            m_cursor += 1;
        }

        return tok;
      }

      case '"': case '\'': {
        if (*tmp == '"') tok.type = DQSTRING;
        else             tok.type = SQSTRING;

        tok.str = get_string(tmp + 1, *tmp, &m_scratch, &m_strings);
        m_cursor += tok.str.len + 2;

        return tok;
      }
    }

    if (*tmp == '_' || std::isalpha(*tmp)) {
      tok.str = get_id(tmp, &m_scratch, &m_strings);
    }

    tmp++;
  }

  tok.type = PARSE_ERROR;
  return tok;
}

lexer::lexer(const char *src) : m_scratch(), m_strings() {
  size_t len = std::strlen(src);

  char *tmp = new char[len + 1];
  std::memcpy(tmp, src, len);
  tmp[len] = 0;

  m_src = tmp;
  m_cursor = m_src;
}

lexer::~lexer(void) {
  delete[] m_src;
  m_src = NULL;
}

};
