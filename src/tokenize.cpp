#include "tokenize.hpp"
#include "string_view.hpp"
#include "arena.hpp"
#include "error.hpp"

#include <assert.h>
#include <vector>
#include <cstring>
#include <cctype>
#include <string_view>

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

struct string_view get_string(sv_map &strings_map, const char *start, char q,
                              class arena *strings, const char **end) {
  if (q != '\'' && q != '"') {
    assert(0 && "Unreachable");
  }

  bool escape = false;
  struct string_view sv = { };

  std::vector<char> scratch;

  while (*start) {
    if (escape) {
      if (*start == '\n') {
        start++;
        continue;
      }

      scratch.push_back(resolve_esc(*start));
      escape = false;

    } else {
      if (*start == '\\') {
        escape = true;

      } else if (*start == q) {
        if (end != nullptr) *end = start + 1;

        auto &&key = std::string_view(scratch.begin(), scratch.end());
        if (auto search = strings_map.find(key); 
              search != strings_map.end()) {
          return search->second;
        }

        char *tmp = strings->alloc<char>(scratch.size() + 1);
        sv.len = scratch.size();
        for (uint64_t i = 0; i < scratch.size(); i++) {
          tmp[i] = scratch[i];
        }

        tmp[scratch.size()] = 0;

        sv.view = tmp;
        
        strings_map.insert({sv.view, sv});

        return sv;

      } else if (*start == '\n') {
        if (end != nullptr) *end = start + 1;
        return { nullptr, 0 }; // Parse error

      } else {
        scratch.push_back(*start);
      }
    }

    start++;
  }

  return { nullptr, 0 }; // Parse error
}

const char *skip_comment(const char *start, char style) {
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

struct string_view get_id(sv_map &strings_map, const char *start, 
                          class arena *strings, const char **end) {
  struct string_view sv = { .view = nullptr };
  std::vector<char> scratch;

  while (*start) {
    if (*start != '_' && !std::isalpha(*start) && !std::isdigit(*start)) {
      if (scratch.size() == 0) {
        if (end != nullptr) *end = start;
        return sv;
      }
      if (end != nullptr) *end = start;

      auto &&key = std::string_view(scratch.begin(), scratch.end());
      if (auto search = strings_map.find(key); 
            search != strings_map.end()) {
        return search->second;
      }

      char *tmp = strings->alloc<char>(scratch.size() + 1);
      for (uint64_t i = 0; i < scratch.size(); i++) {
        tmp[i] = scratch[i];
      }
      
      tmp[scratch.size()] = 0;

      sv.view = tmp;
      sv.len = scratch.size();

      strings_map.insert({sv.view, sv});

      return sv;
    }

    scratch.push_back(*start);

    start++;
  }

  return sv;
}

void get_number(sv_map &strings_map, struct token *token, const char *start,
                class arena *strings, const char **end) {
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
    
    token->number.suff = get_id(strings_map, float_end, strings, end);

    return;
  } else {
    token->type = INTLIT;

    token->number.intlit = int_rep;
    token->number.suff = get_id(strings_map, int_end, strings, end);

    return;
  }
}
