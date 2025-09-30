#include "visualize.h"

static const char* get_token_color(TK_TYPE type) {
  switch (type) {
    case TK_IDENT:   return COLOR_GREEN;
    case TK_PUNCT:   return COLOR_YELLOW;
    case TK_KEYWORD: return COLOR_MAGENTA;
    case TK_NUM:     return COLOR_CYAN;
    case TK_EOF:     return COLOR_RED;
    default:         return COLOR_WHITE;
  }
}

static const char* get_token_shape(TK_TYPE type) {
  switch (type) {
    case TK_IDENT:   return SHAPE_CIRCLE;
    case TK_PUNCT:   return SHAPE_SQUARE;
    case TK_KEYWORD: return SHAPE_STAR;
    case TK_NUM:     return SHAPE_DIAMOND;
    case TK_EOF:     return SHAPE_TRIANGLE;
    default:         return SHAPE_CIRCLE;
  }
}

static const char* get_token_type_name(TK_TYPE type) {
  switch (type) {
    case TK_IDENT:   return "IDENT";
    case TK_PUNCT:   return "PUNCT";
    case TK_KEYWORD: return "KEYWORD";
    case TK_NUM:     return "NUM";
    case TK_EOF:     return "EOF";
    default:         return "UNKNOWN";
  }
}

void visualize_tokens(struct Token *tok) {
  printf("\n");
  printf("%s%s══════════════════════════════════════════════════════════════%s\n",
         COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
  printf("%s%s  TOKEN VISUALIZATION - Lexical Analysis Stage  %s\n",
         COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
  printf("%s%s══════════════════════════════════════════════════════════════%s\n",
         COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
  printf("\n");

  int token_count = 0;
  for (struct Token *t = tok; t != NULL && t->type != TK_EOF; t = t->next) {
    token_count++;
    const char *color = get_token_color(t->type);
    const char *shape = get_token_shape(t->type);

    printf("%s%s %s", color, shape, COLOR_RESET);
    printf(" %s%-8s%s ", COLOR_BOLD, get_token_type_name(t->type), COLOR_RESET);
    printf("%s│%s ", COLOR_DIM, COLOR_RESET);

    // Print token text
    printf("%s", color);
    for (int i = 0; i < t->len; i++) {
      printf("%c", t->loc[i]);
    }
    printf("%s", COLOR_RESET);

    // Print value for numbers
    if (t->type == TK_NUM) {
      printf(" %s(value: %d)%s", COLOR_DIM, t->val, COLOR_RESET);
    }

    printf("\n");

    // Add connector between tokens
    if (t->next != NULL && t->next->type != TK_EOF) {
      printf("  %s%s%s\n", COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
    }
  }

  printf("\n");
  printf("%s%sTotal tokens: %d%s\n", COLOR_BOLD, COLOR_CYAN, token_count, COLOR_RESET);
  printf("\n");
}

static const char* get_node_color(NodeKind kind) {
  switch (kind) {
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
      return COLOR_YELLOW;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
      return COLOR_MAGENTA;
    case ND_ASSIGN:
      return COLOR_RED;
    case ND_RETURN:
      return COLOR_CYAN;
    case ND_IF:
    case ND_FOR:
      return COLOR_BLUE;
    case ND_VAR:
      return COLOR_GREEN;
    case ND_NUM:
      return COLOR_CYAN;
    case ND_FUNCALL:
      return COLOR_MAGENTA;
    default:
      return COLOR_WHITE;
  }
}

static const char* get_node_shape(NodeKind kind) {
  switch (kind) {
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
      return SHAPE_DIAMOND;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
      return SHAPE_TRIANGLE;
    case ND_ASSIGN:
      return SHAPE_ARROW;
    case ND_RETURN:
    case ND_IF:
    case ND_FOR:
      return SHAPE_STAR;
    case ND_VAR:
      return SHAPE_CIRCLE;
    case ND_NUM:
      return SHAPE_SQUARE;
    case ND_FUNCALL:
      return SHAPE_STAR;
    default:
      return SHAPE_CIRCLE;
  }
}

static const char* get_node_name(NodeKind kind) {
  switch (kind) {
    case ND_ADD:       return "ADD (+)";
    case ND_SUB:       return "SUB (-)";
    case ND_MUL:       return "MUL (*)";
    case ND_DIV:       return "DIV (/)";
    case ND_NEG:       return "NEG (-)";
    case ND_EQ:        return "EQ (==)";
    case ND_NE:        return "NE (!=)";
    case ND_LT:        return "LT (<)";
    case ND_LE:        return "LE (<=)";
    case ND_ASSIGN:    return "ASSIGN (=)";
    case ND_ADDR:      return "ADDR (&)";
    case ND_DEREF:     return "DEREF (*)";
    case ND_RETURN:    return "RETURN";
    case ND_IF:        return "IF";
    case ND_FOR:       return "FOR/WHILE";
    case ND_BLOCK:     return "BLOCK";
    case ND_FUNCALL:   return "FUNCALL";
    case ND_EXPR_STMT: return "EXPR_STMT";
    case ND_VAR:       return "VAR";
    case ND_NUM:       return "NUM";
    default:           return "UNKNOWN";
  }
}

void visualize_node(struct Node *node, int depth, bool is_last, char *prefix) {
  if (node == NULL) return;

  // Print current node with proper indentation
  printf("%s", prefix);

  if (depth > 0) {
    printf("%s%s %s", COLOR_DIM, is_last ? TREE_LAST : TREE_BRANCH, COLOR_RESET);
  }

  const char *color = get_node_color(node->kind);
  const char *shape = get_node_shape(node->kind);

  printf("%s%s %s%s", color, shape, get_node_name(node->kind), COLOR_RESET);

  // Add extra info for specific node types
  if (node->kind == ND_NUM) {
    printf(" %s[%d]%s", COLOR_DIM, node->val, COLOR_RESET);
  } else if (node->kind == ND_VAR && node->var) {
    printf(" %s[%s]%s", COLOR_DIM, node->var->name, COLOR_RESET);
  } else if (node->kind == ND_FUNCALL) {
    printf(" %s[%s()]%s", COLOR_DIM, node->funcname, COLOR_RESET);
  }

  printf("\n");

  // Calculate new prefix for children
  char new_prefix[1024];
  snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix,
           depth > 0 ? (is_last ? TREE_SPACE : TREE_CONTINUE) : "");

  // Traverse children based on node type
  if (node->kind == ND_IF) {
    if (node->cond) {
      printf("%s%s%sCOND:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->cond, depth + 1, false, new_prefix);
    }
    if (node->then) {
      printf("%s%s%sTHEN:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->then, depth + 1, node->els == NULL, new_prefix);
    }
    if (node->els) {
      printf("%s%s%sELSE:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->els, depth + 1, true, new_prefix);
    }
  } else if (node->kind == ND_FOR) {
    if (node->init) {
      printf("%s%s%sINIT:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->init, depth + 1, false, new_prefix);
    }
    if (node->cond) {
      printf("%s%s%sCOND:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->cond, depth + 1, false, new_prefix);
    }
    if (node->inc) {
      printf("%s%s%sINC:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->inc, depth + 1, false, new_prefix);
    }
    if (node->then) {
      printf("%s%s%sBODY:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->then, depth + 1, true, new_prefix);
    }
  } else if (node->kind == ND_BLOCK) {
    struct Node *stmt = node->body;
    while (stmt) {
      visualize_node(stmt, depth + 1, stmt->next == NULL, new_prefix);
      stmt = stmt->next;
    }
  } else if (node->kind == ND_FUNCALL) {
    if (node->args) {
      printf("%s%s%sARGS:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      struct Node *arg = node->args;
      while (arg) {
        visualize_node(arg, depth + 1, arg->next == NULL, new_prefix);
        arg = arg->next;
      }
    }
  } else {
    // Binary/Unary operations
    if (node->lhs) {
      printf("%s%s%sLEFT:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->lhs, depth + 1, node->rhs == NULL, new_prefix);
    }
    if (node->rhs) {
      printf("%s%s%sRIGHT:%s\n", new_prefix, COLOR_DIM, TREE_VERTICAL, COLOR_RESET);
      visualize_node(node->rhs, depth + 1, true, new_prefix);
    }
  }
}

void visualize_ast(struct Obj *prog) {
  printf("\n");
  printf("%s%s══════════════════════════════════════════════════════════════%s\n",
         COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
  printf("%s%s  AST VISUALIZATION - Syntax Analysis Stage  %s\n",
         COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
  printf("%s%s══════════════════════════════════════════════════════════════%s\n",
         COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
  printf("\n");

  for (struct Obj *obj = prog; obj != NULL; obj = obj->next) {
    const char *type_name = "UNKNOWN";
    if (obj->ty) {
      switch (obj->ty->kind) {
        case TY_INT:   type_name = "int"; break;
        case TY_PTR:   type_name = "ptr"; break;
        case TY_FUNC:  type_name = "func"; break;
        case TY_ARRAY: type_name = "array"; break;
      }
    }

    if (obj->is_function) {
      printf("%s%s━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━%s\n",
             COLOR_BOLD, COLOR_BLUE, COLOR_RESET);
      printf("%s%s FUNCTION: %s%s %s%s()%s\n",
             COLOR_BOLD, SHAPE_STAR, COLOR_BLUE, obj->name, COLOR_RESET,
             COLOR_BOLD, COLOR_RESET);
      printf("%s%s━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━%s\n",
             COLOR_BOLD, COLOR_BLUE, COLOR_RESET);

      // Show parameters
      if (obj->params) {
        printf("\n%s%sParameters:%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
        for (struct Obj *param = obj->params; param; param = param->next) {
          printf("  %s%s %s%s%s\n", SHAPE_CIRCLE, COLOR_GREEN, param->name, COLOR_RESET,
                 param->next ? "," : "");
        }
      }

      // Show function body AST
      if (obj->body) {
        printf("\n%s%sFunction Body AST:%s\n", COLOR_BOLD, COLOR_YELLOW, COLOR_RESET);
        visualize_node(obj->body, 0, true, "");
      }
      printf("\n");
    } else {
      printf("%s%s GLOBAL VAR: %s%s %s%s%s\n",
             SHAPE_CIRCLE, COLOR_GREEN, type_name, COLOR_RESET,
             COLOR_BOLD, obj->name, COLOR_RESET);
    }
  }

  printf("\n");
}