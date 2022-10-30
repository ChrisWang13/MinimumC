#include "../token/token.h"
#include "rules.h"

struct Obj *globals;

static char *
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

// program = (function-definition | global-variable)*
struct Obj *
parse_token(struct Token *tok) {
  globals = NULL;

  while (tok->type != TK_EOF) {
    struct Type *basety = declspec(&tok, tok);
    // TODO: Function

    // Global variable
    tok = global_variable(tok, basety);

  }
  return globals;
}
