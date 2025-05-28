#pragma once

#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include "string_view.hpp"
#include "tokenize.hpp"

#include <vector>

#define TO_MAP(str)                     \
  std::pair(                            \
    str,                                \
    TO_SV(str)                          \
  )

using sv_vec = std::vector<struct string_view>;
using ident_map = cstr_umap<struct string_view>;

struct context {
  context(const char *filepath, const char *src) 
    : filepath(filepath), src(src) { }

  ident_map types = {
    TO_MAP("short"),
    TO_MAP("int"),
    TO_MAP("long"),
    TO_MAP("char"),

    TO_MAP("signed"),
    TO_MAP("unsigned"),

    TO_MAP("float"),
    TO_MAP("double"),

    TO_MAP("void"),
  };

  ident_map ids;
  ident_map tagged_ids; // Structs, unions, enums
  
  uint64_t id_counter = 0;

  const char *filepath;
  const char *src;
};

#endif // PCC_CONTEXT_H
