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

static enum types to_unsigned(enum types type) {
  switch (type) {
    case TYPE_NONE:
      return TYPE_UINT;

    case TYPE_CHAR:
      return TYPE_UCHAR;

    case TYPE_SHORT:
      return TYPE_USHORT;
    case TYPE_INT:
      return TYPE_UINT;
    case TYPE_LONG:
      return TYPE_ULONG;
    case TYPE_LONGLONG:
      return TYPE_ULONGLONG;

    default:
      return TYPE_NONE;
  }
}

static enum types to_signed(enum types type) {
  switch (type) {
    case TYPE_NONE:
      return TYPE_SINT;

    case TYPE_CHAR:
      return TYPE_SCHAR;

    case TYPE_SHORT:
      return TYPE_SSHORT;
    case TYPE_INT:
      return TYPE_SINT;
    case TYPE_LONG:
      return TYPE_SLONG;
    case TYPE_LONGLONG:
      return TYPE_SLONGLONG;

    default:
      return TYPE_NONE;
  }
}

static enum types strip_signed(enum types type) {
  switch (type) {
    case TYPE_SSHORT:
      return TYPE_SHORT;
    case TYPE_SINT:
      return TYPE_INT;
    case TYPE_SLONG:
      return TYPE_LONG;
    case TYPE_SLONGLONG:
      return TYPE_LONGLONG;

    default:
      return type;
  }
}

static void parse_type(struct context *ctx,
                       std::vector<struct scope> *scopes, 
                       struct ast_node *curr, 
                       struct type_spec *type) {

}

static void parse_decl(struct context *ctx,
                       std::vector<struct scope> *scopes, 
                       struct ast_node *curr) {
  struct token tok = ctx->lexer->get_tok();
  struct token peeked = ctx->lexer->peek();

  if (std::strcmp(tok.str.view, "struct") == 0 || 
      std::strcmp(tok.str.view, "union") == 0 || 
      std::strcmp(tok.str.view, "enum") == 0) {
    /* 
     * Possible declarations ->
     *   variable
     *   function
     *   record
     *   enum
     */
    assert(0 && "TODO: Handle non-primitive types");

  } else if (std::strcmp(tok.str.view, "typedef")) {
    assert(0 && "TODO: Handle typedefs");
  }

  /* 
   * Possible declarations ->
   *   variable
   *   function
   */
  struct type_spec t;
  parse_type(ctx, scopes, curr, &t);
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
