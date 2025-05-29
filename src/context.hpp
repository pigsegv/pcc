#pragma once

#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include "string_view.hpp"
#include "tokenize.hpp"
#include "arena.hpp"
#include "lexer.hpp"


#include <vector>

#define TO_MAP(str)                     \
  std::pair(                            \
    str,                                \
    TO_SV(str)                          \
  )

using sv_vec = std::vector<struct string_view>;
using ident_map = cstr_umap<struct string_view>;

struct context {
  context(const char *filepath, const char *src, class lexer *lexer,
          class arena *arena) 
    : filepath(filepath), src(src), lexer(lexer), arena(arena) { }
  
  uint64_t id_counter = 0;

  const char *filepath;
  const char *src;

  class lexer *lexer;
  class arena *arena;
};

#endif // PCC_CONTEXT_H
