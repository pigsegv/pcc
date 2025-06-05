#include "parse-types.hpp"
#include "types.hpp"
#include "string_view.hpp"
#include "context.hpp"

#include <tuple>

std::tuple<struct type_spec *, enum storage_classes, enum qualifiers> 
parse_base_type(struct context *ctx) {
  struct type_spec *type = nullptr; 
  enum storage_classes storage = CLASS_NONE;
  enum qualifiers qual = QUAL_NONE;

  struct token tok = ctx->lexer->peek();
  
  // get types, qualifiers and storage classes
  if (auto s = keywords.find(TO_STD_SV(tok.str));
      s != keywords.end()) {
    
    struct type_spec tmp = {
      .type = TYPE_NONE,
    };

    for (; tok.type == ID && s != keywords.end(); 
         tok = ctx->lexer->peek(), 
         s = tok.type == ID 
           ? keywords.find(TO_STD_SV(tok.str)) : keywords.end()) {
      ctx->lexer->get_tok();
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

        case TYPE_NONE: {
          if (std::strcmp(tok.str.view, "static") == 0) {
            if (storage != CLASS_NONE) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                           "declaration has multiple storage classes\n");
            }

            storage = CLASS_STATIC;

          } else if (std::strcmp(tok.str.view, "auto") == 0) {
            if (storage != CLASS_NONE) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                           "declaration has multiple storage classes\n");
            }
            storage = CLASS_AUTO;

          } else if (std::strcmp(tok.str.view, "register") == 0) {
            if (storage != CLASS_NONE) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                           "declaration has multiple storage classes\n");
            }
            storage = CLASS_REGISTER;

          } else if (std::strcmp(tok.str.view, "extern") == 0) {
            if (storage != CLASS_NONE) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                           "declaration has multiple storage classes\n");
            }
            storage = CLASS_EXTERN;

          }  else if (std::strcmp(tok.str.view, "volatile") == 0) {
            qual = (enum qualifiers)(qual | QUAL_VOLATILE);

          } else if (std::strcmp(tok.str.view, "const") == 0) {
            qual = (enum qualifiers)(qual | QUAL_CONST);

          } else {
            report_error(ctx->filepath, ctx->src, tok.location,
                        "Unexpected token \"%.*s\"\n", 
                        (int)tok.str.len, tok.str.view);
            std::exit(EXIT_FAILURE);
          }
        } break;
          
        default:
          tmp.type = MERGE_TYPE(tmp.type, s->second);
      }
    }

    if (storage == CLASS_NONE) {
      storage = CLASS_AUTO;
    }
  
    if ((tmp.type & 0x0000ffff) == 0) {
      tmp.type = MERGE_TYPE(tmp.type, TYPE_INT);
    }

    switch (tmp.type) {
      case TYPE_CHAR: case TYPE_INT:   case TYPE_FLOAT: case TYPE_DOUBLE:
      case TYPE_VOID: case TYPE_SCHAR: case TYPE_SINT:  case TYPE_UCHAR:
      case TYPE_UINT: case TYPE_SHORT: case TYPE_LONG:  case TYPE_LONGLONG:
      case TYPE_LONGDOUBLE: case TYPE_SSHORT: case TYPE_SLONG:
      case TYPE_SLONGLONG:  case TYPE_USHORT: case TYPE_ULONG:
      case TYPE_ULONGLONG:  case TYPE_NONE:
        break;

      default:
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                     "declaration has an invalid type: %u\n", tmp.type);
    }

    if (tmp.type != TYPE_NONE) {
      type = new (ctx->scratch.alloc(sizeof(*type))) (struct type_spec) {
        .type = tmp.type,
      };
    }
  } 

  if (type == nullptr) {
    type = find_type_in_scope(&tok.str, &ctx->scopes);
  }

  assert(type != nullptr);

  return { type, storage, qual };
}

static uint32_t get_ptr_level(struct context *ctx) {
  uint32_t level = 0;

  for (auto tok = ctx->lexer->peek();
       tok.type == CHARLIT && tok.charlit == '*';
       tok = ctx->lexer->peek()) {
    level++;
    ctx->lexer->get_tok();
  }

  return level;
}

struct type_spec *parse_type_expr(struct context *ctx,
                                  struct type_spec *base_type,
                                  struct string_view *identifier) {
  auto arena_save = ctx->arena->save();
  struct type_spec *type = new (ctx->arena->alloc(sizeof(*type))) 
                             struct type_spec;

  struct token tok = ctx->lexer->peek();
  if (tok.type == ID) {
    printf("%s\n", tok.str.view);
  }
  if (tok.type == CHARLIT) {
    switch (tok.charlit) {
      case '*': 
        type->type = TYPE_PTR;
        type->ptr.level = get_ptr_level(ctx);
        type->ptr.type = base_type;
        break;

      case '(':  // This will be handled in the next step
        ctx->lexer->get_tok();
        break;

      case ';':
        ctx->arena->restore(arena_save);
        return nullptr;
    }
  }

  if (type->type == TYPE_PTR) {
    printf("%u\n", type->ptr.level);
  }

  return type;
}
