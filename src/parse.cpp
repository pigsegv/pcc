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

static void parse_block(struct context *ctx,
                        struct ast_node *curr) {
  ctx->scopes.push_back({ });

  auto last_arena_save = ctx->arena->save();
  curr->child = new (ctx->arena->alloc(sizeof(*curr->child))) 
                  (struct ast_node) {
                    .type = NODE_TYPE_NONE,
                    .id = ctx->id_counter++,
                    .parent = curr,
                  };

  struct ast_node *last_child = curr->child;

  struct type_spec t = { .type = TYPE_VARIADIC };
  ctx->scopes.back().types.insert({"lol", &t});
  for (;;) {
    struct token tok = ctx->lexer->peek();

    switch (tok.type) {
      case ID: {
        if (keywords.contains(TO_STD_SV(tok.str)) || 
            find_type_in_scope(&tok.str, &ctx->scopes) != nullptr) {
          parse_decl(ctx, last_child);
        } else {
          ctx->lexer->get_tok();
        }
      } break;
      
      case END_OF_FILE:
        std::exit(0);
      default:
        ctx->lexer->get_tok();
    }

    last_arena_save = ctx->arena->save();
    last_child->siblings.next = 
      new (ctx->arena->alloc(sizeof(*last_child->siblings.next))) 
        (struct ast_node) {
          .type = NODE_TYPE_NONE,
          .id = ctx->id_counter++,
          .parent = curr,
          
          .siblings = {
            .prev = last_child,
          },
        };
    last_child = last_child->siblings.next;
  }

  last_child->siblings.prev->siblings.next = nullptr;

  assert(!ctx->scopes.empty());
  ctx->scopes.pop_back();
}


struct ast_node parse(class lexer *lexer, class arena *arena) {
  struct context ctx(lexer->get_filepath(), lexer->get_src(), lexer, arena);
  struct ast_node root = {
    .type = NODE_TYPE_BLOCK,
    .id = ctx.id_counter++,
    
    .block = new (arena->alloc(sizeof(*root.block))) (struct block)(),

    .parent = nullptr,

    .siblings = {
      nullptr,
      nullptr,
    },

    .child = nullptr,
  };

  root.block->type = BLOCK_GLOBAL;

  parse_block(&ctx, &root);

  return root;
}
