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

static void print_op(enum operators op) {
  switch (op) {
    case OP_UNARY_PLUS: case OP_PLUS:
      printf("+");
      return;
    case OP_UNARY_MINUS: case OP_MINUS:
      printf("-");
      return;

    case OP_MUL: case OP_DEREF:
      printf("*");
      return;
    case OP_DIV:
      printf("/");
      return;
    case OP_MOD:
      printf("%%");
      return;

    case OP_OR: 
      printf("|");
      return;
    case OP_AND: case OP_REF:
      printf("&");
      return;
    case OP_XOR: 
      printf("^");
      return;
    case OP_NEGATE: 
      printf("~");
      return;
    case OP_NOT:
      printf("!");
      return;

    case OP_SUBSCRIPT:
      printf("[]");
      return;

    case OP_FUNCALL:
      printf("Funcall");
      return;

    case OP_CAST:
      printf("Cast");
      return;

    case OP_COMMA:
      printf(",");
      return;

    case OP_SIZEOF:
      printf("sizeof");
      return;


    case OP_LESS:
      printf("<");
      return;
    case OP_GREATER:
      printf(">");
      return;
    case OP_EQ:
      printf("==");
      return;
    case OP_NOTEQ:
      printf("!=");
      return;
    case OP_LESSEQ:
      printf("<=");
      return;
    case OP_GREATEREQ:
      printf(">=");
      return;


    case OP_ANDAND:
      printf("&&");
      return;
    case OP_OROR:
      printf("||");
      return;

    case OP_SHL:
      printf("<<");
      return;
    case OP_SHR:
      printf(">>");
      return;


    case OP_PREINC: case OP_POSTINC:
      printf("++");
      return;
    case OP_PREDEC: case OP_POSTDEC:
      printf("--");
      return;

    case OP_ASSIGN:
      printf("=");
      return;
    case OP_PLUSEQ:
      printf("+=");
      return;
    case OP_MINUSEQ:
      printf("-=");
      return;
    case OP_MULEQ:
      printf("*=");
      return;
    case OP_DIVEQ:
      printf("/=");
      return;
    case OP_MODEQ:
      printf("%%=");
      return;
    case OP_ANDEQ:
      printf("&=");
      return;
    case OP_OREQ:
      printf("|=");
      return;
    case OP_XOREQ:
      printf("^=");
      return;
    case OP_SHLEQ:
      printf("<<=");
      return;
    case OP_SHREQ:
      printf(">>=");
      return;

    case OP_ARROW:
      printf("->");
      return;
    case OP_DOT:
      printf(".");
      return;

    case OP_TERNARY:
      printf("Tern");
      return;

    default:
      assert(0);
  }
}

static void print_expr(struct expr *e) {
  switch (e->type) {
    case EXPR_SIMPLE_ID:
      printf("%s", e->identifier.view);
      return;

    case EXPR_SIMPLE_LIT: {
      switch (e->value->type) {
        case TYPE_SHORT: 
          printf("%d", e->value->lit_short);
          break;
        case TYPE_INT: 
          printf("%d", e->value->lit_int);
          break;
        case TYPE_LONG:
          printf("%ld", e->value->lit_long);
          break;
        case TYPE_USHORT: 
          printf("%u", e->value->lit_ushort);
          break;
        case TYPE_UINT: 
          printf("%u", e->value->lit_uint);
          break;
        case TYPE_ULONG:
          printf("%lu", e->value->lit_ulong);
          break;
        case TYPE_FLOAT:
          printf("%f", e->value->lit_float);
          break;
        case TYPE_DOUBLE:
          printf("%lf", e->value->lit_double);
          break;
        case TYPE_LONGDOUBLE:
          printf("%LF", e->value->lit_longdouble);
          break;
        case TYPE_STR:
          printf("\"%s\"", e->value->lit_str.view);
          break;
        case TYPE_CHAR:
          printf("%c", e->value->lit_char);
          break;
        case TYPE_SCHAR:
          printf("%d", e->value->lit_schar);
          break;
        case TYPE_UCHAR:
          printf("%d", e->value->lit_uchar);
          break;

        default:
          assert(0);
      }

    } return;

    case EXPR_UNARY: {
      printf("(");
      if (e->op == OP_SUBSCRIPT) {
        printf("[");
        print_expr(e->unary.ctx);
        printf("]");
      } else {
        print_op(e->op);
      }
      printf(" ");
      print_expr(e->unary.operand);
      printf(")");

    } return;

    case EXPR_BINARY:
      if (e->op == OP_CONCAT) {
        print_expr(e->binary.left);
        print_expr(e->binary.right);
      } else {
        printf("(");
        print_op(e->op);
        printf(" ");
        print_expr(e->binary.left);
        printf(" ");
        print_expr(e->binary.right);
        printf(")");
      }
      return;

    case EXPR_TERNARY:
      printf("(");
      print_op(e->op);
      printf(" ");
      print_expr(e->ternary.condition);
      printf(" ");
      print_expr(e->ternary.succ);
      printf(" ");
      print_expr(e->ternary.fail);
      printf(")");
      return;

    default:
      assert(0);
  }
}

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
        printf("%sarray ", ind.c_str());
        print_expr(type->array.size);
        printf("\n");
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
      case TYPE_DOUBLE:
        printf("%sdouble\n", ind.c_str());
        break;

      case TYPE_VOID:
        printf("%svoid\n", ind.c_str());
        break;

      case TYPE_SCHAR:
        printf("%sschar\n", ind.c_str());
        break;

      case TYPE_UCHAR:
        printf("%suchar\n", ind.c_str());
        break;

      case TYPE_SHORT:
        printf("%sshort\n", ind.c_str());
        break;
      case TYPE_LONG:
        printf("%slong\n", ind.c_str());
        break;
      case TYPE_LONGDOUBLE:
        printf("%slongdouble\n", ind.c_str());
        break;

      case TYPE_USHORT:
        printf("%sushort\n", ind.c_str());
        break;
      case TYPE_ULONG:
        printf("%sulong\n", ind.c_str());
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
