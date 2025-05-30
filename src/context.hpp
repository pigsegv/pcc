#pragma once

#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include "string_view.hpp"
#include "tokenize.hpp"
#include "arena.hpp"
#include "lexer.hpp"


#include <vector>

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
