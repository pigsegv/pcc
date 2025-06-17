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
  OP_MOD,

  OP_OR,
  OP_AND,
  OP_XOR,
  OP_NEGATE,

  OP_NOT,

  OP_REF,
  OP_DEREF,
  OP_SUBSCRIPT,
  OP_OPEN_SQR,
  OP_CLOSE_SQR,

  OP_FUNCALL,
  OP_OPEN_PAREN,
  OP_CLOSE_PAREN,

  OP_COMMA,

  OP_CAST,

  OP_SIZEOF,

  OP_LESS,
  OP_GREATER,
  OP_EQ,
  OP_NOTEQ,
  OP_LESSEQ,
  OP_GREATEREQ,


  OP_ANDAND,
  OP_OROR,

  OP_SHL,
  OP_SHR,

  OP_PREINC,
  OP_POSTINC,
  OP_PREDEC,
  OP_POSTDEC,

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
  OP_TERN,    // ?
  OP_ARY,     // :
};


enum expr_types {
  EXPR_NONE,

  EXPR_SIMPLE_ID,
  EXPR_SIMPLE_LIT,

  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_TERNARY,

  EXPR_OP, // Just an operator, no operands
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

    struct {
      struct expr **args;
      uint64_t args_count;

    } func;

    struct literal *value;
    struct type_spec *cast_type;
    struct string_view identifier;
  };
};

struct expr *parse_expr(struct context *ctx, const char *term);
struct literal *fold_expr(const struct expr *expr);

#endif // PCC_EXPR_H
