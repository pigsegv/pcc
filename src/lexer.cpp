#include "lexer.hpp"
#include "arena.hpp"
#include "dynamic-array.hpp"
#include "error.hpp"

#include <cstring>
#include <cctype>
#include <utility>

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
                                     class arena *strings,
                                     const char **end) {
  if (q != '\'' && q != '"') {
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

        if (end != nullptr) *end = start + 1;

        return sv;

      } else if (*start == '\n') {
        if (end != nullptr) *end = start + 1;
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
                                 class arena *strings, const char **end) {
  struct string_view sv = { .view = nullptr };
  struct dynamic_array<char> scratch_da(scratch);

  while (*start) {
    if (*start != '_' && !std::isalpha(*start) && !std::isdigit(*start)) {
      if (scratch_da.count == 0) {
        if (end != nullptr) *end = start;
        return sv;
      }

      char *tmp = strings->alloc<char>(scratch_da.count);
      for (uint64_t i = 0; i < scratch_da.count; i++) {
        tmp[i] = scratch_da[i];
      }

      sv.view = tmp;
      sv.len = scratch_da.count;

      if (end != nullptr) *end = start;

      return sv;
    }

    scratch_da.append(*start);

    start++;
  }

  return sv;
}

static void get_number(struct token *token, const char *start, 
                       class arena *scratch, class arena *strings,
                       const char **end) {
  char *int_end, *float_end;

  uint64_t int_rep = std::strtoull(start, &int_end, 0);
  long double float_rep = std::strtold(start, &float_end);
  (void)float_rep;

  if (float_end > int_end) {
    token->type = FLOATLIT;

    {
      char *tmp = strings->alloc<char>((ptrdiff_t)(float_end - start));
      std::memcpy(tmp, start, (ptrdiff_t)(float_end - start));
      token->number.float_lit.view = tmp;
      token->number.float_lit.len = (ptrdiff_t)(float_end - start);
    }
    
    token->number.suff = get_id(float_end, scratch, strings, end);

    return;
  } else {
    token->type = INTLIT;

    token->number.intlit = int_rep;
    token->number.suff = get_id(int_end, scratch, strings, end);

    return;
  }
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
            tok.type = CHARLIT; 
            tok.charlit = '/';
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
            tok.type = CHARLIT;
            tok.charlit = '*';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '+': {
        switch (*(tmp + 1)) {
          case '+': 
            tok.type = PLUSPLUS;
            m_cursor = tmp + 2;
            break;

          case '=': 
            tok.type = PLUSEQ;
            m_cursor = tmp + 2;
            break;

          default:
            tok.type = CHARLIT;
            tok.charlit = '+';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '-': {
        switch(*(tmp + 1)) {
          case '-': 
            tok.type = MINUSMINUS;
            m_cursor = tmp + 2;
            break;

          case '=': 
            tok.type = MINUSEQ;
            m_cursor = tmp + 2;
            break;

          case '>': 
            tok.type = ARROW;
            m_cursor = tmp + 2;
            break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '-';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '=': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = EQ;
            m_cursor = tmp + 2;
            break;


          default: 
            tok.type = CHARLIT;
            tok.charlit = '=';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '!': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = NOTEQ;
            m_cursor = tmp + 2;
            break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '!';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '<': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = LESSEQ;
            m_cursor = tmp + 2;
            break;

          case '<': {
            if (*(tmp + 2) == '=') {
              tok.type = SHLEQ;
              m_cursor = tmp + 3;
            } else {
              tok.type = SHL;
              m_cursor = tmp + 2;
            }

          } break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '<';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '>': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = GREATEREQ;
            m_cursor = tmp + 2;
            break;

          case '>': {
            if (*(tmp + 2) == '=') {
              tok.type = SHREQ;
              m_cursor = tmp + 3;
            } else {
              tok.type = SHR;
              m_cursor = tmp + 2;
            }

          } break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '>';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '%': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = MODEQ;
            m_cursor = tmp + 2;
            break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '%';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '&': {
        switch(*(tmp + 1)) {
          case '&': 
            tok.type = ANDAND;
            m_cursor = tmp + 2;
            break;

          case '=': 
            tok.type = ANDEQ;
            m_cursor = tmp + 2;
            break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '&';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '|': {
        switch(*(tmp + 1)) {
          case '|': 
            tok.type = OROR;
            m_cursor = tmp + 2;
            break;

          case '=': 
            tok.type = OREQ;
            m_cursor = tmp + 2;
            break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '|';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '^': {
        switch(*(tmp + 1)) {
          case '=': 
            tok.type = XOREQ;
            m_cursor = tmp + 2;
            break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '^';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '.': {
        switch(*(tmp + 1)) {
          case '.': {
            if (*(tmp + 2) == '.') {
              tok.type = ELIPSES;
              m_cursor = tmp + 3;
            } 

          } break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '.';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case '#': {
        switch(*(tmp + 1)) {
          case '#': 
            tok.type = POUNDPOUND;
            m_cursor = tmp + 2;
            break;

          default: 
            tok.type = CHARLIT;
            tok.charlit = '#';
            m_cursor = tmp + 1;
        }

        return tok;
      }

      case ';': case '(': case ')': 
      case '{': case '}': case '[': case ']': 
      case '\\': case ',': case '?': case ':':
      case '\n': case '~':
        tok.type = CHARLIT;
        tok.charlit = *tmp;
        m_cursor = tmp + 1;
        return tok;

      case '"': case '\'': {
        if (*tmp == '"') tok.type = DQSTRING;
        else             tok.type = SQSTRING;

        tok.str = get_string(tmp + 1, *tmp, &m_scratch, &m_strings, &m_cursor);
        if (tok.str.view == nullptr) {
          report_error(m_filepath, m_src, tok.location, 
                       "Expected closing '\"'");
          return {
            .type = PARSE_ERROR,
            .location = tok.location,
          };
        }

        return tok;
      }
    }

    if (*tmp == '_' || std::isalpha(*tmp)) {
      tok.type = ID;
      tok.str = get_id(tmp, &m_scratch, &m_strings, &m_cursor);
      return tok;
    }

    if (std::isdigit(*tmp)) {
      get_number(&tok, tmp, &m_scratch, &m_strings, &m_cursor);
      return tok;
    }

    tmp++;
  }
  m_cursor = tmp;

  tok.type = END_OF_FILE;
  return tok;
}

class lexer &lexer::operator =(class lexer &that) {
  if (&that == this) {
    return *this;
  }

  delete[] m_src;
  delete[] m_filepath;
  m_scratch.reset();
  m_strings.reset();

  char *tmp;

  tmp = new char[std::strlen(that.m_filepath) + 1];
  std::strcpy(tmp, that.m_filepath);
  m_filepath = tmp;

  tmp = new char[std::strlen(that.m_src) + 1];
  std::strcpy(tmp, that.m_src);
  m_src = that.m_src;

  m_cursor = m_src; 

  return *this;
}

class lexer &lexer::operator =(class lexer &&that) {
  m_scratch.reset();
  m_strings.reset();

  std::swap(m_filepath, that.m_filepath);
  std::swap(m_src, that.m_src);
  m_cursor = m_src;


  return *this;
}

lexer::lexer(const char *src, const char *filepath) : m_scratch(), m_strings() {
  size_t len = std::strlen(src);

  char *tmp = new char[len + 1];
  std::memcpy(tmp, src, len);
  tmp[len] = 0;

  m_filepath = new char[std::strlen(filepath) + 1];
  std::strcpy(m_filepath, filepath);

  m_src = tmp;
  m_cursor = m_src;
}

lexer::~lexer(void) {
  delete[] m_src;
  delete[] m_filepath;
  m_src = nullptr;
}

}
