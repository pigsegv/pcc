#ifndef PCC_LANG_H
#define PCC_LANG_H

#include "string_view.hpp"

#include <unordered_set>
#include <functional>

static const std::unordered_set<const char *, sv_map_hash, std::equal_to<>> 
keywords = {
  "int",
  "char",

  "long",
  "short",

  "signed",
  "unsigned",

  "float",
  "double",

  "void",

  "struct",
  "union",
  "enum",

  "typedef",

  "const",
  "volatile",

  "auto",
  "static",
  "register",
  "extern",

  "switch",
  "break",
  "case",
  "continue",
  "default",

  "do",
  "while",
  "for",

  "if",
  "else",

  "goto",
  "return",

  "sizeof",
};

#endif // PCC_LANG_H
