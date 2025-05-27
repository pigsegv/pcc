#pragma once

#ifndef PCC_STMT_H
#define PCC_STMT_H

#include "expr.hpp"
#include "string_view.hpp"

enum stmt_types {
  STMT_REGULAR,
  STMT_RETURN,
  STMT_CONTINUE,
  STMT_BREAK,
  STMT_LABEL,
  STMT_CASE,
};

struct stmt {
  enum stmt_types type;
  
  union {
    struct string_view *name;
    struct expr *expr;
  };
};

#endif // PCC_STMT_H
