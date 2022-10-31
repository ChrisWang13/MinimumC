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

// func-params = (param ("," param)*)? ")"
// param       = declspec declarator
struct Type *
func_params(struct Token **rest, struct Token *tok, struct Type *ty) {
  struct Type head = {};
  struct Type *cur = &head;

  while (!is_match(tok, ")")) {
    if (cur != &head)
      tok = next_token(tok, ",");
    struct Type *basety = declspec(&tok, tok);
    struct Type *ty = declarator(&tok, tok, basety);
    cur = cur->next = copy_type(ty);
  }

  ty = func_type(ty);
  ty->params = head.next;
  *rest = tok->next;
  return ty;
}

// type-suffix = "(" func-params
//             | "[" num "]" type-suffix
//             | ε
struct Type *
type_suffix(struct Token **rest, struct Token *tok, struct Type *ty) {
  // TODO: Rule 1
  if (is_match(tok, "("))
    return func_params(rest, tok->next, ty);
  // T0DO: Rule 2

  // Rule 3
  *rest = tok;
  return ty;
}

// expr-stmt = expr? ";"
struct Node*
expr_stmt(struct Token **rest, struct Token *tok) {
  if (is_match(tok, ";")) {
    *rest = tok->next;
    return new_node(ND_BLOCK, tok);
  }

  struct Node *node = new_node(ND_EXPR_STMT, tok);
  // node->lhs = expr(&tok, tok);
  *rest = next_token(tok, ";");
  return node;
}

// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "for" "(" expr-stmt expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | "{" compound-stmt
//      | expr-stmt
struct Node *
stmt(struct Token **rest, struct Token *tok) {
  // TODO: RULE 1-3
  
  // Rule 4:
  if (is_match(tok, "{"))
    return compound_stmt(rest, tok->next);
  
  // Rule 5:
  return expr_stmt(rest, tok);
}

// compound-stmt = (declaration | stmt)* "}"
struct Node *
compound_stmt(struct Token **rest, struct Token *tok) {
  struct Node *node = new_node(ND_BLOCK, tok);

  struct Node head = {};
  struct Node *cur = &head;
  while (!is_match(tok, "}")) {
    if (is_match(tok, "int"))
      cur = cur->next = declaration(&tok, tok);
    else
      cur = cur->next = stmt(&tok, tok);
    add_type(cur);
  }

  node->body = head.next;
  *rest = tok->next;
  return node;
}

// declaration = declspec (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
struct Node *
declaration(struct Token **rest, struct Token *tok) {

  struct Node head = {};

  struct Node *node = new_node(ND_BLOCK, tok);
  node->body = head.next;
  *rest = tok->next;
  return node;
}
