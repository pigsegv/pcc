#pragma once

#ifndef PCC_EXPR_H
#define PCC_EXPR_H

#include "types.hpp"
#include "literals.hpp"

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

enum expr_types {
  EXPR_NONE,

  EXPR_SIMPLE_ID,
  EXPR_SIMPLE_LIT,

  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_TERNARY,
};

struct simple_expr {

};

struct expr {
  enum expr_types type;

  bool is_lvalue;

  union {
    struct {
      enum operators op;
      struct expr *operand;

    } unary;

    struct {
      enum operators op;
      struct expr *left;
      struct expr *right;

    } binary;

    struct {
      struct expr *condition;
      struct expr *succ;
      struct expr *fail;

    } ternary;

    struct literal *value;
    struct type_spec *cast_type;
    struct string_view identifier;
  };
};

struct expr *parse_expr(struct context *ctx);

#endif // PCC_EXPR_H
