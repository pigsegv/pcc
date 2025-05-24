#pragma once

#ifndef PCC_EXPR_H
#define PCC_EXPR_H

#include "types.hpp"

enum operators {
  OP_NONE,

  OP_UNARY_PLUS,
  OP_UNARY_MINUS,

  OP_PLUS,
  OP_MINUS,
  OP_MUL,
  OP_DIV,

  OP_OR,
  OP_AND,
  OP_XOR,
  OP_NEGATE,

  OP_NOT,

  OP_REF,
  OP_DEREF,
  OP_SUBSCRIPT,

  OP_FUNCALL,

  OP_COMMA,

  OP_CAST,

  OP_SIZEOF,

  OP_EQ,
  OP_NOTEQ,
  OP_LESSEQ,
  OP_GREATEREQ,

  OP_ANDAND,
  OP_OROR,

  OP_SHL,
  OP_SHR,

  OP_PLUSPLUS,
  OP_MINUSMINUS,

  OP_ASSIGN,

  OP_PLUSEQ,
  OP_MINUSEQ,
  OP_MULEQ,
  OP_DIVEQ,
  OP_MODEQ,
  OP_ANDEQ,
  OP_OREQ,
  OP_XOREQ,

  OP_ARROW,
  OP_DOT,

  OP_SHLEQ, 
  OP_SHREQ,
  
  OP_TERNARY,
};

struct expr {
  bool is_lvalue;
  enum operators op;

  union {
    struct {
      struct expr *left;
      struct expr *right;
    };

    struct {
      struct expr *condition;
      struct expr *succ;
      struct expr *fail;
    };

    void *value; // Don't know what this is going to be yet
  };
};

#endif // PCC_EXPR_H
