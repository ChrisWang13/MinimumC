#include "../token/token.h"
#include "rules.h"

struct Obj *globals;
struct Obj *locals;

char *
get_ident(struct Token *tok) {
  if (tok->type != TK_IDENT)
    error_tok(tok, "Expected an identifier");
  return strndup(tok->loc, tok->len);
}


struct Obj *
new_var(char *name, struct Type *ty) {
  struct Obj *var = malloc(sizeof(struct Obj));
  var->name = name;
  var->ty = ty;
  return var;
}

struct Obj *
new_gvar(char *name, struct Type *ty) {
  struct Obj *var = new_var(name, ty);
  var->next = globals;
  globals = var;
  return var;
}

struct Node *
new_node(NodeKind kind, struct Token *tok) {
  struct Node *node = malloc(sizeof(struct Node));
  node->kind = kind;
  node->tok = tok;
  return node;
}

struct Node *
new_binary(NodeKind kind, struct Node *lhs, struct Node *rhs, struct Token *tok) {
  struct Node *node = new_node(kind, tok);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

struct Node *
new_unary(NodeKind kind, struct Node *expr, struct Token *tok) {
  struct Node *node = new_node(kind, tok);
  node->lhs = expr;
  return node;
}

struct Node *
new_num(int val, struct Token *tok) {
  struct Node *node = new_node(ND_NUM, tok);
  node->val = val;
  return node;
}

struct Node *
new_add(struct Node *lhs, struct Node *rhs, struct Token *tok) {
  add_type(lhs);
  add_type(rhs);

  // num + num
  if (integer_type(lhs->ty) && integer_type(rhs->ty))
    return new_binary(ND_ADD, lhs, rhs, tok);

  if (lhs->ty->base && rhs->ty->base)
    error_tok(tok, "invalid operands");

  exit(0);
}

// Like `+`, `-` is overloaded for the pointer type.
struct Node *
new_sub(struct Node *lhs, struct Node *rhs, struct Token *tok) {
  add_type(lhs);
  add_type(rhs);

  // num - num
  if (integer_type(lhs->ty) && integer_type(rhs->ty))
    return new_binary(ND_SUB, lhs, rhs, tok);

  error_tok(tok, "invalid operands");
  return 0;
}

struct Token *
global_variable(struct Token *tok, struct Type *basety) {
  bool first = true;

  while (!cur_token(&tok, tok, ";")) {
    if (!first)
      tok = next_token(tok, ",");
    first = false;

    struct Type *ty = declarator(&tok, tok, basety);
    new_gvar(get_ident(ty->name), ty);
  }
  return tok;
}

// Lookahead tokens and returns true if 
// a given token is a start
// of a function definition or declaration.
bool 
is_function(struct Token *tok) {
  if (is_match(tok, ";"))
    return false;

  struct Type dummy = {};
  struct Type *ty = declarator(&tok, tok, &dummy);
  return ty->kind == TY_FUNC;
}

struct Token *
function(struct Token *tok, struct Type *basety) {
  struct Type *ty = declarator(&tok, tok, basety);

  struct Obj *fn = new_gvar(get_ident(ty->name), ty);
  fn->is_function = true;

  // locals = NULL;
  // create_param_lvars(ty->params);
  // fn->params = locals;

  tok = next_token(tok, "{");
  fn->body = compound_stmt(&tok, tok);
  // fn->locals = locals;
  return tok;
}

// program = (function-definition | global-variable)*
struct Obj *
parse_token(struct Token *tok) {
  globals = NULL;

  while (tok->type != TK_EOF) {
    struct Type *basety = declspec(&tok, tok);
    // Function
    if (is_function(tok)) {
      tok = function(tok, basety);
      continue;
    }
    // Global variable
    tok = global_variable(tok, basety);

  }
  return globals;
}
