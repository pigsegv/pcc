#pragma once

#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include "string_view.hpp"

#include <vector>

using sv_vec = std::vector<struct string_view>;

struct context {
  context(const char *filepath, const char *src) 
    : filepath(filepath), src(src) { }

  sv_vec types = {
    TO_SV("short"),
    TO_SV("int"),
    TO_SV("long"),
    TO_SV("char"),

    TO_SV("signed"),
    TO_SV("unsigned"),

    TO_SV("float"),
    TO_SV("double"),

    TO_SV("void"),
  };

  sv_vec ids;
  sv_vec tagged_ids; // Structs, unions, enums
  
  uint64_t id_counter = 0;

  const char *filepath;
  const char *src;
};

#endif // PCC_CONTEXT_H
