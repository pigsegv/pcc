#pragma once

#ifndef PCC_TYPES_H
#define PCC_TYPES_H

#include "string_view.hpp"

#include <utility>

enum types : int {
  TYPE_NONE,

  TYPE_PTR,
  TYPE_ARRAY,
  TYPE_STR, // This is for string literals

  TYPE_RECORD,
  TYPE_ENUM,

  TYPE_FUNC,

  TYPE_CHAR,
  TYPE_SCHAR,
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

  // These are to make sure that both signed and unsigned don't appear
  // in the same declaration.
  TYPE_SSHORT,
  TYPE_SINT,
  TYPE_SLONG,
  TYPE_SLONGLONG,

  TYPE_VARIADIC,
  TYPE_VOID,
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
  struct type_spec *args;
  uint64_t num_args;
  bool variadic;

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

#define TO_MAP(str, type_enum)          \
  std::pair(                            \
    str,                                \
    type_enum                           \
  )

// I should probably give a better name to this
static const cstr_umap<enum types> primitives = {
  TO_MAP("short", TYPE_SHORT),
  TO_MAP("int", TYPE_INT),
  TO_MAP("char", TYPE_CHAR),
  TO_MAP("long", TYPE_NONE),

  TO_MAP("signed", TYPE_NONE),
  TO_MAP("unsigned", TYPE_NONE),

  TO_MAP("struct", TYPE_NONE),
  TO_MAP("union", TYPE_NONE),
  TO_MAP("enum", TYPE_NONE),

  TO_MAP("float", TYPE_FLOAT),
  TO_MAP("double", TYPE_DOUBLE),

  TO_MAP("void", TYPE_VOID),
};

#endif // PCC_TYPES_H
