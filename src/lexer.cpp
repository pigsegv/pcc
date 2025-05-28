#include "lexer.hpp" 
#include "tokenize.hpp"
#include "error.hpp"

#include <cstdarg>
#include <cstring>
#include <cctype>
#include <utility>
#include <optional>

struct token lexer::get_tok(void) {
  struct token tok;

  if (m_peeked.has_value()) {
    tok = m_peeked.value();
    m_peeked = std::nullopt;
    return tok;
  }

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

        tok.str = get_string(m_strings_map, tmp + 1, *tmp, m_strings, &m_cursor);
        if (tok.str.view == nullptr) {
          report_error(m_filepath, m_src, tok.location, 
                       "Expected closing '" FORMAT_ERROR("\"") "'\n");
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
      tok.str = get_id(m_strings_map, tmp, m_strings, &m_cursor);
      return tok;
    }

    if (std::isdigit(*tmp)) {
      get_number(m_strings_map, &tok, tmp, m_strings, &m_cursor);
      return tok;
    }

    tmp++;
  }
  m_cursor = tmp;

  tok.type = END_OF_FILE;
  return tok;
}


struct token lexer::get_tok_and_expect(int type, ...) {
  std::va_list args;
  va_start(args, type);

  struct token tok = get_tok();
  if (tok.type != type) {
    report_error(m_filepath, m_src, tok.location,
                 "Expected %d, but got %d\n", type, tok.type);
    tok.type = PARSE_ERROR;
    goto Exit;
  }

  if (tok.type == CHARLIT) {
    int c = va_arg(args, int);
    if (tok.charlit != c) {
      report_error(m_filepath, m_src, tok.location,
                   "Expected '%c', but got '%c'\n", c, tok.charlit);
      tok.type = PARSE_ERROR;
      goto Exit;
    }
  }

Exit:
  va_end(args);
  return tok;
}

struct token lexer::peek(void) {
  if (m_peeked.has_value()) {
    return m_peeked.value();
  }

  m_peeked = get_tok();
  return m_peeked.value();
}

lexer::lexer(const char *src, const char *filepath, class arena *strings) {
  size_t len = std::strlen(src);

  char *tmp = new char[len + 1];
  std::memcpy(tmp, src, len);
  tmp[len] = 0;

  m_filepath = new char[std::strlen(filepath) + 1];
  std::strcpy(m_filepath, filepath);

  m_src = tmp;
  m_cursor = m_src;

  m_peeked = std::nullopt;

  m_strings = strings;
}

lexer::~lexer(void) {
  delete[] m_src;
  delete[] m_filepath;
  m_src = nullptr;
}
