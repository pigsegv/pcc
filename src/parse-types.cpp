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

  struct token tok = ctx->lexer->get_tok();
  
  // get types, qualifiers and storage classes
  if (auto s = keywords.find(TO_STD_SV(tok.str));
      s != keywords.end()) {
    
    struct type_spec tmp = {
      .type = TYPE_NONE,
    };

    for (; s != keywords.end(); 
         tok = ctx->lexer->get_tok(), 
         s = keywords.find(TO_STD_SV(tok.str))) {
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

void parse_type_expr(struct context *ctx, class arena *arena,
                     struct type_spec *base_type,
                     struct string_view *identifier) {
  struct type_spec *type = new (arena->alloc(sizeof(*type))) 
                             (struct type_spec) {
                               .type = TYPE_NONE,
                             };

  for (auto tok = ctx->lexer->get_tok(); ; tok = ctx->lexer->get_tok()) {
    switch (tok.type) {
      case CHARLIT: {
        switch (tok.charlit) {
          case '*': {
            if (type->type == TYPE_PTR) {
              type->ptr.level++;

            } else if (type->type == TYPE_NONE) {
              type->type = TYPE_PTR;
              type->ptr.level = 1;
            } else {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location,
                           "Invalid type declaration\n");
            }
          } break;

          case '[':
            assert(0 && "TODO: Parse array declaration"); break;
          case '(':

        }
      } break;

    }
  }
}
