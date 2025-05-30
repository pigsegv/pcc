#pragma once

#ifndef PCC_LITERALS_H
#define PCC_LITERALS_H

#include "types.hpp"
#include "string_view.hpp"

#include <stdint.h>

struct literal {
  enum types type;

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

    struct string_view lit_str;

    char               lit_char;
    signed char        lit_schar;
    unsigned char      lit_uchar;

    void              *lit_ptr;

    struct string_view lit_enum;

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
