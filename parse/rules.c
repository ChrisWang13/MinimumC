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
  node->lhs = expr(&tok, tok);
  *rest = next_token(tok, ";");
  return node;
}

// expr = assign
struct Node *
expr(struct Token **rest, struct Token *tok) {
  return assign(rest, tok);
}

// assign = equality ("=" assign)?
struct Node *
assign(struct Token **rest, struct Token *tok) {
  struct Node *node = equality(&tok, tok);

  if (is_match(tok, "="))
    return new_binary(ND_ASSIGN, node, assign(rest, tok->next), tok);

  *rest = tok;
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
struct Node *equality(struct Token **rest, struct Token *tok) {
  struct Node *node = relational(&tok, tok);

  for (;;) {
    struct Token *start = tok;

    if (is_match(tok, "==")) {
      node = new_binary(ND_EQ, node, relational(&tok, tok->next), start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
struct Node *
relational(struct Token **rest, struct Token *tok) {
  struct Node *node = add(&tok, tok);
  return node;
}

struct Node *
add(struct Token **rest, struct Token *tok) {
  struct Node *node = mul(&tok, tok);

  for (;;) {
    struct Token *start = tok;

    if (is_match(tok, "+")) {
      node = new_add(node, mul(&tok, tok->next), start);
      continue;
    }

    if (is_match(tok, "-")) {
      node = new_sub(node, mul(&tok, tok->next), start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// unary = ("+" | "-" | "*") unary
struct Node *
unary(struct Token **rest, struct Token *tok) {
  if (is_match(tok, "+"))
    return unary(rest, tok->next);

  if (is_match(tok, "-"))
    return new_unary(ND_NEG, unary(rest, tok->next), tok);

  if (is_match(tok, "*"))
    return new_unary(ND_DEREF, unary(rest, tok->next), tok);

  exit(0);
}


// mul = unary ("*" unary | "/" unary)*
struct Node *
mul(struct Token **rest, struct Token *tok) {
  struct Node *node = unary(&tok, tok);

  for (;;) {
    struct Token *start = tok;

    if (is_match(tok, "*")) {
      node = new_binary(ND_MUL, node, unary(&tok, tok->next), start);
      continue;
    }

    if (is_match(tok, "/")) {
      node = new_binary(ND_DIV, node, unary(&tok, tok->next), start);
      continue;
    }

    *rest = tok;
    return node;
  }
}


// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "for" "(" expr-stmt expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | "{" compound-stmt
//      | expr-stmt
struct Node *
stmt(struct Token **rest, struct Token *tok) {
  // TODO: RULE 2-3
  // Rule 1: 
  if (is_match(tok, "return")) {
    struct Node *node = new_node(ND_RETURN, tok);
    node->lhs = expr(&tok, tok->next);
    *rest = next_token(tok, ";");
    return node;
  }
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
