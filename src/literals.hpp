#pragma once

#ifndef PCC_LITERALS_H
#define PCC_LITERALS_H

#include "types.hpp"

#include <stdint.h>

enum lit_types {
  LIT_SHORT,
  LIT_INT,
  LIT_LONG,
  LIT_LONG_LONG,

  LIT_USHORT,
  LIT_UINT,
  LIT_ULONG,
  LIT_ULONG_LONG,

  LIT_FLOAT,
  LIT_DOUBLE,
  LIT_LONG_DOUBLE,

  LIT_STR,

  LIT_CHAR,
  LIT_SCHAR,
  LIT_UCHAR,

  LIT_PTR,

  LIT_ENUM,

  // Only used during initialization
  LIT_ARRAY,
  LIT_RECORD,
};

struct literal {
  enum lit_types type;

  union {
    short              lit_short;
    int                lit_int;
    long               lit_long;
    long long          lit_longlong;

    unsigned short     lit_ushort;
    unsigned int       lit_uint;
    unsigned long      lit_ulong;
    unsigned long long lit_ulonglong;

    float              lit_float;
    double             lit_double;
    long double        lit_longdouble;

    const char        *lit_str;

    char               lit_char;
    signed char        lit_schar;
    unsigned char      lit_uchar;

    void              *lit_ptr;

    const char        *lit_enum;

    struct {
      struct literal *ems;
      uint64_t        count;
    } lit_array;

    struct {
      struct literal *fields;
      uint64_t        count; // This is probably redundant 
    } lit_record;
  };
};

#endif // PCC_LITERALS_H
