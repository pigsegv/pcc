#include "decls.hpp"
#include "context.hpp"
#include "lexer.hpp"
#include "expr.hpp"
#include "parse-types.hpp"

#include <cstring>
#include <cstdlib>
#include <vector>
#include <assert.h>
#include <cstdlib>
#include <tuple>
#include <optional>
#include <string>

static void print_types(struct type_spec *type, int indent) {
  std::string ind(indent * 2, ' ');

  for (;;) {
    switch (type->type) {
      case TYPE_PTR:
        if (type->ptr.level > 1) {
          printf("%ux ", type->ptr.level);
        }
        printf("%spointer\n", ind.c_str());
        break;
      case TYPE_ARRAY:
        printf("%sarray\n", ind.c_str());
        break;
      case TYPE_FUNC:
        printf("%sfunction:\n", ind.c_str()); 
        for (uint64_t i = 0; i < type->func.num_args; i++) {
          print_types(type->func.args[i], indent + 1);
        }
        break;
      case TYPE_INT:
        printf("%sint\n", ind.c_str());
        break;
      case TYPE_FLOAT:
        printf("%sfloat\n", ind.c_str());
        break;

      default:
        printf("%stoo lazy to add this\n", ind.c_str());
        break;
    }

    if (type->type != TYPE_PTR && type->type != TYPE_ARRAY &&
        type->type != TYPE_FUNC) {
      return;
    }

    switch (type->type) {
      case TYPE_PTR:
        type = type->ptr.type;
        break;
      case TYPE_ARRAY:
        type = type->array.type;
        break;
      case TYPE_FUNC:
        type = type->func.ret;
        break;

      default:
        assert(0 && "unreachable");
    }
  }
}

static struct type_spec *parse_var_and_func(struct context *ctx,
                                            struct ast_node *curr) {
  (void)curr;

  struct token tok = ctx->lexer->peek();
  auto arena_save = ctx->arena->save();
  auto [type, storage, quals] = parse_base_type(ctx);

  std::optional<struct string_view> ident;
  
  auto tmp = parse_type_expr(ctx, type, &ident);
  ctx->lexer->get_tok_and_expect(CHARLIT, ';');

  (void)tok;
  (void)arena_save;
  
  // if (std::get<1>(tmp)->type == TYPE_FUNC) {
    // ctx->arena->restore(arena_save);
    // ctx->lexer->backtrack(&tok);
// 
    // return nullptr;
  // }

  auto next = std::get<0>(tmp);
  printf("--------------------------------\n");
  print_types(next, 0);

  return std::get<1>(tmp);
}

void parse_decl(struct context *ctx, struct ast_node *curr) {
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
    assert(0 && "TODO: Handle record types");

  } else if (std::strcmp(tok.str.view, "typedef") == 0) {
    assert(0 && "TODO: Handle typedefs");
  }

  /* 
   * Possible declarations ->
   *   variable
   *   function
   */
  struct type_spec *t = parse_var_and_func(ctx, curr);
  (void)t;
}
