#pragma once

#ifndef PCC_PARSE_H
#define PCC_PARSE_H

#include "lexer.hpp"
#include "decls.hpp"
#include "types.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "block.hpp"

#include <string>
#include <stdint.h>
#include <utility>

enum node_types {
  NODE_TYPE_NONE,

  NODE_TYPE_PARSE_ERR,
  NODE_TYPE_BLOCK,
  NODE_TYPE_STMT,
  NODE_TYPE_DECL,
};

struct ast_node {
  enum node_types type;
  uint64_t id;

  union {
    struct decl *decl;
    struct block *block;
    struct stmt *stmt;
  };

  struct ast_node *parent;

  struct {
    struct ast_node *prev;
    struct ast_node *next;

  } siblings;

  struct ast_node *child;
};

struct ast_node parse(class lexer *lexer, class arena *arena);

#endif // PCC_PARSE_H
