#pragma once

#ifndef PCC_DECLS_H
#define PCC_DECLS_H

#include "types.hpp"

enum decl_types {
  DECL_TYPE_FUNC,
  DECL_TYPE_ARG,
  DECL_TYPE_TYPEDEF,
  DECL_TYPE_RECORD,
  DECL_TYPE_FIELD,
  DECL_TYPE_ENUM,
  DECL_TYPE_VAR,
};

enum storage_classes {
  CLASS_NONE = 0,
  
  CLASS_STATIC,
  CLASS_AUTO,
  CLASS_REGISTER,
  CLASS_EXTERN,
};

enum record_types {
  RECORD_TYPE_STRUCT,
  RECORD_TYPE_UNION,
};

enum qualifiers {
  QUAL_NONE = 0,

  QUAL_VOLATILE,
  QUAL_CONST,
};

struct func_decl {
  struct type_spec *ret;
};

struct var_decl {
  struct type_spec *type;
  struct expr *init;
};

struct decl {
  enum decl_types type;
  const char *name;

  enum qualifiers qual;
  enum storage_classes storage;

  union {
    struct func_decl *func_decl;
    struct var_decl  *var_decl;
  };
};

#endif // PCC_DECLS_H
