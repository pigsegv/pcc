#include "tokenize.hpp"
#include "arena.hpp"
#include "dynamic-array.hpp"
#include "error.hpp"

#include <assert.h>

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

struct string_view get_string(const char *start, char q, 
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

struct string_view get_id(const char *start, class arena *scratch,
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

void get_number(struct token *token, const char *start, 
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
