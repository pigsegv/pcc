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

static enum types to_long(enum types type) {
  switch (type) {
    case TYPE_NONE:
      return TYPE_INC_LONG;

    case TYPE_INC_SIGNED:
      return TYPE_INC_SLONG;
  case TYPE_INC_UNSIGNED:
      return TYPE_INC_ULONG;

    case TYPE_INC_LONG:
      return TYPE_INC_LONGLONG;
    case TYPE_INC_SLONG:
      return TYPE_INC_SLONGLONG;
    case TYPE_INC_ULONG:
      return TYPE_INC_ULONGLONG;

    case TYPE_INT:
      return TYPE_INC_LONG;
    case TYPE_LONG:
      return TYPE_INC_LONGLONG;
    case TYPE_UINT:
      return TYPE_INC_ULONG;
    case TYPE_ULONG:
      return TYPE_INC_ULONGLONG;

    case TYPE_DOUBLE:
      return TYPE_LONGDOUBLE;

    default:
      return TYPE_NONE;
  }
}

static enum types to_short(enum types type) {
  switch (type) {
    case TYPE_NONE:
      return TYPE_INC_SHORT;

    case TYPE_INC_SIGNED:
      return TYPE_INC_SSHORT;
    case TYPE_INC_UNSIGNED:
      return TYPE_INC_USHORT;

    case TYPE_INT:
      return TYPE_INC_SHORT;
    case TYPE_UINT:
      return TYPE_INC_USHORT;

    default:
      return TYPE_NONE;
  }
}

static enum types to_unsigned(enum types type) {
  switch (type) {
    case TYPE_NONE:
      return TYPE_INC_UNSIGNED;

    case TYPE_INC_SHORT:
      return TYPE_INC_USHORT;
    case TYPE_INC_LONG:
      return TYPE_INC_ULONG;
    case TYPE_INC_LONGLONG:
      return TYPE_INC_ULONGLONG;

    case TYPE_CHAR:
      return TYPE_UCHAR;

    case TYPE_INT:
      return TYPE_INC_UNSIGNED;
    case TYPE_LONG:
      return TYPE_INC_ULONG;
    case TYPE_LONGLONG:
      return TYPE_INC_ULONGLONG;

    default:
      return TYPE_NONE;
  }
}

static enum types to_signed(enum types type) {
  switch (type) {
    case TYPE_NONE:
      return TYPE_INC_SIGNED;

    case TYPE_INC_SHORT:
      return TYPE_INC_SSHORT;
    case TYPE_INC_LONG:
      return TYPE_INC_SLONG;
    case TYPE_INC_LONGLONG:
      return TYPE_INC_SLONGLONG;

    case TYPE_CHAR:
      return TYPE_SCHAR;

    case TYPE_INT:
      return TYPE_INC_SIGNED;
    case TYPE_LONG:
      return TYPE_INC_SLONG;
    case TYPE_LONGLONG:
      return TYPE_INC_SLONGLONG;

    default:
      return TYPE_NONE;
  }
}

static enum types resolve_type(enum types type) {
  switch (type) {
    case TYPE_INC_SIGNED:
      return TYPE_INT;
    case TYPE_INC_UNSIGNED:
      return TYPE_UINT;

    case TYPE_INC_SHORT: case TYPE_INC_SSHORT:
      return TYPE_SHORT;
    case TYPE_INC_LONG: case TYPE_INC_SLONG:
      return TYPE_LONG;
    case TYPE_INC_LONGLONG: case TYPE_INC_SLONGLONG:
      return TYPE_LONGLONG;

    case TYPE_INC_USHORT:
      return TYPE_USHORT;
    case TYPE_INC_ULONG:
      return TYPE_ULONG;
    case TYPE_INC_ULONGLONG:
      return TYPE_ULONGLONG;
    
    default:
      return type;
  }
}

static struct type_spec parse_var(struct context *ctx,
                                  std::vector<struct scope> *scopes, 
                                  struct ast_node *curr) {
  struct type_spec type = {
    .type = TYPE_NONE,
  };

  struct token tok = ctx->lexer->get_tok();
  if (auto s = primitives.find(TO_STD_SV(tok.str));
      s != primitives.end()) {
    bool contains = true;
    while (contains) {

      tok = ctx->lexer->get_tok();
      s = primitives.find(TO_STD_SV(tok.str));
      contains = s != primitives.end();
    }
  }
}

static void parse_decl(struct context *ctx,
                       std::vector<struct scope> *scopes, 
                       struct ast_node *curr) {
  struct token tok = ctx->lexer->peek();

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
  struct type_spec t = parse_var(ctx, scopes, curr);
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
