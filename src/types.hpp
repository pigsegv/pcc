#pragma once

#ifndef PCC_TYPES_H
#define PCC_TYPES_H

enum types {
  TYPE_PTR,
  TYPE_ARRAY,

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


#endif // PCC_TYPES_H
