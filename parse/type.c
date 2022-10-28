#include "../token/token.h"

struct Type *ty_int = &(struct Type){TY_INT, 8};

bool 
integer_type(struct Type *ty) {
  return ty->kind == TY_INT;
}

struct Type *
copy_type(struct Type *ty) {
  struct Type *ret = malloc(sizeof(struct Type));
  *ret = *ty;
  return ret;
}

struct Type *
ptr_type(struct Type *base) {
  struct Type *ty = malloc(sizeof(struct Type));
  ty->kind = TY_PTR;
  ty->size = 8;
  ty->base = base;
  return ty;
}

struct Type *
func_type(struct Type *return_ty) {
  struct Type *ty = malloc(sizeof(struct Type));
  ty->kind = TY_FUNC;
  ty->return_ty = return_ty;
  return ty;
}

struct Type *
array_type(struct Type *base, int len) {
  struct Type *ty = malloc(sizeof(struct Type));
  ty->kind = TY_ARRAY;
  ty->size = base->size * len;
  ty->base = base;
  ty->array_len = len;
  return ty;
}

void 
add_type(struct Node *node) {
  if (!node || node->ty)
    return;

  add_type(node->lhs);
  add_type(node->rhs);
  add_type(node->cond);
  add_type(node->then);
  add_type(node->els);
  add_type(node->init);
  add_type(node->inc);

  for (struct Node *n = node->body; n; n = n->next)
    add_type(n);
  for (struct Node *n = node->args; n; n = n->next)
    add_type(n);

  switch (node->kind) {
    // 0-4
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_NEG:
      node->ty = node->lhs->ty;
      return;
    
    // 5-8, 16, 19
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_FUNCALL:
    case ND_NUM:
      node->ty = ty_int;
      return;

    // 9
    case ND_ASSIGN:
      if (node->lhs->ty->kind == TY_ARRAY)
        error_tok(node->lhs->tok, "not an lvalue");
      node->ty = node->lhs->ty;
      return;

    // 10
    case ND_ADDR:
      if (node->lhs->ty->kind == TY_ARRAY)
        node->ty = ptr_type(node->lhs->ty->base);
      else
        node->ty = ptr_type(node->lhs->ty);
      return;

    // 11
    case ND_DEREF:
      if (!node->lhs->ty->base)
        error_tok(node->tok, "Invalid pointer dereference");
      node->ty = node->lhs->ty->base;
      return;

    // 18
    case ND_VAR:
      node->ty = node->var->ty;
      return;

    // 12-15, 17
    case ND_RETURN:
    case ND_IF:
    case ND_FOR:
    case ND_BLOCK:
    case ND_EXPR_STMT:
      return;   
  }
}

