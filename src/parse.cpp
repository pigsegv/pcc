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

struct scope {
  cstr_umap<struct type_spec *> types;
  cstr_umap<struct decl *> identifiers;
  cstr_umap<struct decl *> tagged_types; // struct, enum, union
};

static bool find_type_in_scope(const struct string_view *sv, 
                               const std::vector<struct scope> *scopes) {
  for (auto &scope : *scopes) {
    if (scope.types.contains(TO_STD_SV(*sv))) {
      return true;
    }
  }

  return false;
}

static void parse_decl(struct context *ctx,
                       std::vector<struct scope> *scopes, 
                       struct ast_node *curr) {
  struct token tok = ctx->lexer->get_tok();
  if (strcmp(tok.str, "struct") == 0 || strcmp(tok.str, "union") == 0 || 
      strcmp(tok.str, "enum") == 0) {
    assert(0 && "TODO: Handle non-primitive types");
  }

  if (auto s = primitives.find(TO_STD_SV(tok.str));
      s != primitives.end()) {

  } else {
    assert(0 && "TODO: Handle non-primitive types");
  }
}

static void parse_block(struct context *ctx,
                        std::vector<struct scope> *scopes, 
                        struct ast_node *curr) {

  scopes->push_back({ });

  auto last_arena_save = ctx->arena->save();
  curr->child = new (ctx->arena->alloc(sizeof(*curr->child))) 
                  (struct ast_node) {
                    .type = NODE_TYPE_NONE,
                    .id = ctx->id_counter++,
                    .parent = curr,
                  };

  struct ast_node *last_child = curr->child;

  for (;;) {
    struct token tok = ctx->lexer->peek();

    switch (tok.type) {
      case ID: {
        if (primitives.contains(TO_STD_SV(tok.str)) || 
            find_type_in_scope(&tok.str, scopes)) {
          parse_decl(ctx, scopes, last_child);
        } 
      } break;

      default:
        break;
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

  assert(!scopes->empty());
  scopes->pop_back();
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

  std::vector<struct scope> scopes;
  parse_block(&ctx, &scopes, &root);

  return root;
}
