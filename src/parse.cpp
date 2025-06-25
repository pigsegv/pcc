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

static void parse_block(struct context *ctx,
                        struct ast_node *curr) {
  ctx->scopes.push_back({ });

  auto last_arena_save = ctx->arena->save();
  (void)last_arena_save;
  curr->child = new (ctx->arena->alloc(sizeof(*curr->child))) 
                  (struct ast_node) {
                    .type = NODE_TYPE_NONE,
                    .id = ctx->id_counter++,
                    .parent = curr,
                  };

  struct ast_node *last_child = curr->child;

  for (;;) {
    struct token tok = ctx->lexer->peek();
    struct expr *e;

    switch (tok.type) {
      case ID: {
        if (type_headers.contains(TO_STD_SV(tok.str)) || 
            find_type_in_scope(&tok.str, &ctx->scopes) != nullptr) {
          parse_decl(ctx, last_child);
        } else {
          goto Parse_expr;
          
        }
      } break;

      // case INTLIT: case FLOATLIT: case CHARLIT:
      
      case END_OF_FILE:
        std::exit(0);
      default:
Parse_expr:
        e = parse_expr(ctx, ";");
        if (e == nullptr) {
          EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                      "Expected expression\n");
        }
        ctx->lexer->get_tok_and_expect(CHARLIT, ';');
        print_expr(e);
        printf("\n");
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
