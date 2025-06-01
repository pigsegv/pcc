#pragma once

#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include "string_view.hpp"
#include "tokenize.hpp"
#include "arena.hpp"
#include "lexer.hpp"


#include <vector>

struct scope {
  cstr_umap<struct type_spec *> types;
  cstr_umap<struct decl *> identifiers;
  cstr_umap<struct decl *> tagged_types; // struct, enum, union
};

struct context {
  context(const char *filepath, const char *src, class lexer *lexer,
          class arena *arena) 
    : filepath(filepath), src(src), lexer(lexer), arena(arena), scratch() { }
  
  uint64_t id_counter = 0;

  const char *filepath;
  const char *src;

  class lexer *lexer;

  class arena *arena;
  class arena *scratch;

  std::vector<struct scope> scopes;
};

static inline bool find_type_in_scope(const struct string_view *sv, 
                                      const std::vector<struct scope> *scopes) {
  for (auto &scope : *scopes) {
    if (scope.types.contains(TO_STD_SV(*sv))) {
      return true;
    }
  }

  return false;
}

#endif // PCC_CONTEXT_H
