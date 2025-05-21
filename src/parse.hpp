#pragma once

#ifndef PCC_PARSE_H
#define PCC_PARSE_H

#include "lexer.hpp"
#include "dynamic-array.hpp"

#include <string>
#include <stdint.h>
#include <utility>

enum node_types {
  NODE_TYPE_PARSE_ERR,
  NODE_TYPE_BLOCK,
  NODE_TYPE_STMT,
  NODE_TYPE_EXPR,
  NODE_TYPE_DECL,
};

enum decl_types {
  DECL_TYPE_FUNC,
  DECL_TYPE_TYPEDEF,
  DECL_TYPE_RECORD,
  DECL_TYPE_FIELD,
  DECL_TYPE_ENUM,
  DECL_TYPE_VAR,
};

enum types {
  TYPE_PTR,
  TYPE_ARRAY,

  TYPE_RECORD,
  TYPE_ENUM,

  TYPE_CHAR,
  TYPE_UCHAR,
  TYPE_SHORT,
  TYPE_USHORT,
  TYPE_INT,
  TYPE_UINT,
  TYPE_LONG,
  TYPE_ULONG,
  TYPE_LONGLONG,
  TYPE_ULONGLONG,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_LONGDOUBLE,

  TYPE_VARIADIC,
  TYPE_VOID,
};

enum qualifiers {
  QUAL_NONE = 0,

  QUAL_VOLATILE,
  QUAL_CONST,
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

struct pointer {
  struct type_spec *type;
  uint32_t level;
};

struct array {
  struct type_spec *type;
  int64_t size; // -1 if not known
};

struct bitfield {
  enum types base;
  uint32_t width;
};

struct function {
  dynamic_array<struct type_spec *> args;
  struct type_spec *ret;
};

struct type_spec {
  enum types type;

  union {
    struct pointer  ptr;
    struct array    array;
    struct bitfield bf;
    struct function func;
    uint64_t        record_id;
    uint64_t        enum_id;
  };
};

struct field_decl {
  uint64_t offset;
  struct type_spec *type;
};

struct enum_const {
  const char *name;
  int value;
};

struct record_decl {
  enum record_types type;
  dynamic_array<struct decl *> decls;
};

struct decl {
  enum decl_types type;
  const char *name;

  enum qualifiers qual;
  enum storage_classes storage;

  union {
    struct record_decl              *record_decl;
    struct field_decl               *field_decl;
    struct function                 *func_decl;
    struct type_spec                *var_decl;
    struct type_spec                *typedef_decl;
    dynamic_array<struct enum_const> enum_decl;
  };
};

struct ast_node {
  enum node_types type;
  int64_t id;

  union {
    struct decl *decl;
  };

  struct {
    struct ast_node *prev;
    struct ast_node *next;

  } siblings;
  struct ast_node *child;
};

struct ast_node parse(const class lexer *lexer);

#endif // PCC_PARSE_H
