#include "decls.hpp"
#include "context.hpp"

#include <cstring>
#include <cstdlib>
#include <vector>
#include <assert.h>
#include <cstdlib>


static struct type_spec *parse_var(struct context *ctx,
                                  struct ast_node *curr) {
  struct type_spec *type; 

  struct token tok = ctx->lexer->get_tok();
  if (auto s = primitives.find(TO_STD_SV(tok.str));
      s != primitives.end()) {
    
    struct type_spec tmp = {
      .type = TYPE_NONE,
    };

    for (; s != primitives.end(); 
         tok = ctx->lexer->get_tok(), 
         s = primitives.find(TO_STD_SV(tok.str))) {
      switch (s->second) {
        case SIGNED_FLAG:
          tmp.type = TO_SIGNED(tmp.type);
          break;
        case UNSIGNED_FLAG:
          tmp.type = TO_UNSIGNED(tmp.type);
          break;
        case LONG_FLAG:
          tmp.type = TO_LONG(tmp.type);
          break;
        case SHORT_FLAG:
          tmp.type = TO_SHORT(tmp.type);
          break;

        case TYPE_NONE:
          report_error(ctx->filepath, ctx->src, tok.location,
                       "Unexpected token \"%.*s\"\n", 
                       (int)tok.str.len, tok.str.view);
          std::exit(EXIT_FAILURE);
          
        default:
          tmp.type = MERGE_TYPE(tmp.type, s->second);
      }
    }

    switch (tmp.type) {
      case TYPE_CHAR: case TYPE_INT: case TYPE_FLOAT: case TYPE_DOUBLE:
      case TYPE_VOID: case TYPE_SCHAR: case TYPE_SINT: case TYPE_UCHAR:
      case TYPE_UINT: case TYPE_SHORT: case TYPE_LONG: case TYPE_LONGLONG:
      case TYPE_LONGDOUBLE: case TYPE_SSHORT: case TYPE_SLONG:
      case TYPE_SLONGLONG: case TYPE_USHORT: case TYPE_ULONG:
      case TYPE_ULONGLONG:
        break;

      default:
        report_error(ctx->filepath, ctx->src, tok.location,
                     "Declaration has an invalid type: %d\n", tok.type);
        std::exit(EXIT_FAILURE);
    }
    type = new (ctx->arena->alloc(sizeof(*type))) (struct type_spec) {
      .type = tmp.type,
    };
  } else {
    type = find_type_in_scope(&tok.str, &ctx->scopes);
  }

  printf("%d\n", type->type);
  return type;
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
    assert(0 && "TODO: Handle non-primitive types");

  } else if (std::strcmp(tok.str.view, "typedef") == 0) {
    assert(0 && "TODO: Handle typedefs");
  }

  /* 
   * Possible declarations ->
   *   variable
   *   function
   */
  struct type_spec *t = parse_var(ctx, curr);
}
