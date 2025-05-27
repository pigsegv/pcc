#include "parse.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "types.hpp"
#include "expr.hpp"
#include "block.hpp"
#include "tokenize.hpp"
#include "string_view.hpp"
#include "context.hpp"

#include <cstring>
#include <cstdlib>
#include <vector>
#include <assert.h>

static bool is_type(const struct string_view &sv, const sv_vec &types) {
  for (auto &i : types) {
    if (sv_cmp(&sv, &i) == 0)
      return true;
  }

  return false;
}

static void parse_block(struct context *ctx, class lexer *lexer, 
                        class ast_node *node, class arena *arena) {
  struct ast_node *last_child = nullptr;
  for (;;) {
    struct token tok = lexer->peek();
  }
}

struct ast_node parse(class lexer *lexer, class arena *arena) {
  struct context ctx(lexer->get_filepath(), lexer->get_src());
  struct ast_node root = {
    .type = NODE_TYPE_BLOCK,
    .id = ctx.id_counter++,
    
    .block = arena->alloc<struct block>(sizeof(*root.block)),

    .parent = nullptr,

    .siblings = {
      nullptr,
      nullptr,
    },

    .child = nullptr,
  };

  root.block->type = BLOCK_GLOBAL;

  parse_block(&ctx, lexer, &root, arena);

  return root;
}
