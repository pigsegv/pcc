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

struct block_idents {
  cstr_umap<struct type_spec *> types;
  cstr_umap<struct decl *> identifiers;
  cstr_umap<struct decl *> tagged_types; // struct, enum, union
};

static bool is_type(const struct string_view &sv, const sv_vec &types) {
  for (auto &i : types) {
    if (sv_cmp(&sv, &i) == 0)
      return true;
  }

  return false;
}

// Pass by value for `idents` is not a mistake.
static void parse_block(struct context *ctx, struct block_idents idents, 
                        struct ast_node *curr) {
  struct ast_node *last_child = nullptr;
  for (;;) {
    struct token tok = ctx->lexer->peek();
  }
}


struct ast_node parse(class lexer *lexer, class arena *arena) {
  struct context ctx(lexer->get_filepath(), lexer->get_src(), lexer, arena);
  struct ast_node root = {
    .type = NODE_TYPE_BLOCK,
    .id = ctx.id_counter++,
    
    .block = new (arena->alloc(sizeof(*root.block))) block(),

    .parent = nullptr,

    .siblings = {
      nullptr,
      nullptr,
    },

    .child = nullptr,
  };

  root.block->type = BLOCK_GLOBAL;

  struct block_idents idents;
  parse_block(&ctx, idents, &root);

  return root;
}
