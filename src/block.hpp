#pragma once

#ifndef PCC_BLOCK_H
#define PCC_BLOCK_H

#include "expr.hpp"

enum block_types {
  BLOCK_GLOBAL,

  BLOCK_SCOPE,

  BLOCK_FUNC,

  BLOCK_IF,
  BLOCK_ELSE,

  BLOCK_FOR,
  BLOCK_WHILE,
  BLOCK_DOWHILE,

  BLOCK_SWITCH,
};

/*
 * Does not contain a field for the enclosed statements. This is because
 * statements live in the AST, rather than in the leaves.
 */
struct block {
  enum block_types type;
  struct expr *condition;
  struct expr *init;
};

#endif // PCC_BLOCK_H
