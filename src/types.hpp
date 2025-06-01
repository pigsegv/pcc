#pragma once

#ifndef PCC_TYPES_H
#define PCC_TYPES_H

#include "string_view.hpp"
#include "error.hpp"

#include <utility>
#include <stdint.h>

#define SIGNED_FLAG   0x00'01'00'00
#define UNSIGNED_FLAG 0x00'02'00'00
#define LONG_FLAG     0x00'04'00'00
#define SHORT_FLAG    0x00'08'00'00
#define LONGLONG_FLAG 0x00'10'00'00 // Not directly used

#define TO_SIGNED(type)                                          \
  ((enum types)(((type) & SIGNED_FLAG || (type) & UNSIGNED_FLAG) \
    ? TYPE_INVALID                                               \
    : (type) + SIGNED_FLAG))                         

#define TO_UNSIGNED(type)                                        \
  ((enum types)(((type) & SIGNED_FLAG || (type) & UNSIGNED_FLAG) \
    ? TYPE_INVALID                                               \
    : (type) + UNSIGNED_FLAG))                         

#define TO_LONG(type)                                            \
  ((enum types)(((type) & SHORT_FLAG ||                          \
                 (type) & LONGLONG_FLAG)                         \
    ? TYPE_INVALID                                               \
    : (type) + LONG_FLAG))                         

#define TO_SHORT(type)                                           \
  ((enum types)(((type) & LONG_FLAG || (type) & SHORT_FLAG ||    \
                 (type) & LONGLONG_FLAG)                         \
    ? TYPE_INVALID                                               \
    : (type) + SHORT_FLAG))                         

#define MERGE_TYPE(flags, type)                                  \
  ((enum types)(((flags) & 0x0000ffff)                           \
    ? TYPE_INVALID                                               \
    : (flags) | (type)))

enum types : uint32_t {
  TYPE_NONE = 0,

  TYPE_PTR,
  TYPE_ARRAY,
  TYPE_STR, // This is for string literals

  TYPE_RECORD,
  TYPE_ENUM,

  TYPE_FUNC,

  TYPE_CHAR,
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_DOUBLE,

  TYPE_VOID,
  TYPE_VARIADIC,

  TYPE_SCHAR = TYPE_CHAR + SIGNED_FLAG,
  TYPE_SINT  = TYPE_INT + SIGNED_FLAG,

  TYPE_UCHAR = TYPE_CHAR + UNSIGNED_FLAG,
  TYPE_UINT  = TYPE_INT + UNSIGNED_FLAG,

  TYPE_SHORT      = TYPE_INT + SHORT_FLAG,
  TYPE_LONG       = TYPE_INT + LONG_FLAG,
  TYPE_LONGLONG   = TYPE_INT + LONGLONG_FLAG,
  TYPE_LONGDOUBLE = TYPE_DOUBLE + LONG_FLAG,

  TYPE_SSHORT      = TYPE_SINT + SHORT_FLAG,
  TYPE_SLONG       = TYPE_SINT + LONG_FLAG,
  TYPE_SLONGLONG   = TYPE_SINT + LONGLONG_FLAG,

  TYPE_USHORT      = TYPE_UINT + SHORT_FLAG,
  TYPE_ULONG       = TYPE_UINT + LONG_FLAG,
  TYPE_ULONGLONG   = TYPE_UINT + LONGLONG_FLAG,

  TYPE_INVALID = 0xffff0000, // Guaranteed not to be valid, hopefully
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
  struct string_view *arg_names;
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
static const cstr_umap<uint32_t> primitives = {
  TO_MAP("int", TYPE_INT),
  TO_MAP("char", TYPE_CHAR),

  TO_MAP("long", LONG_FLAG),
  TO_MAP("short", SHORT_FLAG),

  TO_MAP("signed", SIGNED_FLAG),
  TO_MAP("unsigned", UNSIGNED_FLAG),

  TO_MAP("struct", TYPE_NONE),
  TO_MAP("union", TYPE_NONE),
  TO_MAP("enum", TYPE_NONE),

  TO_MAP("const", TYPE_NONE),
  TO_MAP("volatile", TYPE_NONE),

  TO_MAP("auto", TYPE_NONE),
  TO_MAP("static", TYPE_NONE),
  TO_MAP("register", TYPE_NONE),
  TO_MAP("extern", TYPE_NONE),

  TO_MAP("float", TYPE_FLOAT),
  TO_MAP("double", TYPE_DOUBLE),

  TO_MAP("void", TYPE_VOID),
};

#endif // PCC_TYPES_H
