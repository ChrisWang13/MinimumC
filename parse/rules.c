#include "../token/token.h"
#include "rules.h"

// declspec = "int"
struct Type *
declspec(struct Token **rest, struct Token *tok) {
  // Currently support declaration of type int.
  // Use switch case for other type.
  *rest = next_token(tok, "int");
  return ty_int;
}

// declarator = "*"* ident type-suffix
struct Type *
declarator(struct Token **rest, struct Token *tok, struct Type *ty) {
  // TODO: Pointer of int type

  if (tok->type != TK_IDENT)
    error_tok(tok, "Expected a variable name");
  ty = type_suffix(rest, tok->next, ty);
  ty->name = tok;
  return ty;
}


// type-suffix = "(" func-params
//             | "[" num "]" type-suffix
//             | ε
struct Type *
type_suffix(struct Token **rest, struct Token *tok, struct Type *ty) {
  // TODO: Rule 1
 
  // T0DO: Rule 2

  // Rule 3
  *rest = tok;
  return ty;
}