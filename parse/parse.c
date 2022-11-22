#include "../token/token.h"

static struct Obj *locals;
static struct Obj *globals;

static struct Type *declspec(struct Token **rest, struct Token *tok);
static struct Type *declarator(struct Token **rest, struct Token *tok, struct Type *ty);
static struct Node *declaration(struct Token **rest, struct Token *tok);
static struct Node *compound_stmt(struct Token **rest, struct Token *tok);
static struct Node *stmt(struct Token **rest, struct Token *tok);
static struct Node *expr_stmt(struct Token **rest, struct Token *tok);
static struct Node *expr(struct Token **rest, struct Token *tok);
static struct Node *assign(struct Token **rest, struct Token *tok);
static struct Node *equality(struct Token **rest, struct Token *tok);
static struct Node *relational(struct Token **rest, struct Token *tok);
static struct Node *add(struct Token **rest, struct Token *tok);
static struct Node *mul(struct Token **rest, struct Token *tok);
static struct Node *postfix(struct Token **rest, struct Token *tok);
static struct Node *unary(struct Token **rest, struct Token *tok);
static struct Node *primary(struct Token **rest, struct Token *tok);

// Find a local variable by name.
static struct Obj *
find_var(struct Token *tok) {
  for (struct Obj *var = locals; var; var = var->next)
    if (strlen(var->name) == tok->len && !strncmp(tok->loc, var->name, tok->len))
      return var;

  for (struct Obj *var = globals; var; var = var->next)
    if (strlen(var->name) == tok->len && !strncmp(tok->loc, var->name, tok->len))
      return var;

  return NULL;
}

static struct Node *
new_node(NodeKind kind, struct Token *tok) {
  struct Node *node = malloc(sizeof(struct Node));
  node->kind = kind;
  node->tok = tok;
  return node;
}

static struct Node *
new_binary(NodeKind kind, struct Node *lhs, struct Node *rhs, struct Token *tok) {
  struct Node *node = new_node(kind, tok);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static struct Node *
new_unary(NodeKind kind, struct Node *expr, struct Token *tok) {
  struct Node *node = new_node(kind, tok);
  node->lhs = expr;
  return node;
}

static struct Node *
new_num(int val, struct Token *tok) {
  struct Node *node = new_node(ND_NUM, tok);
  node->val = val;
  return node;
}

static struct Node *
new_var_node(struct Obj *var, struct Token *tok) {
  struct Node *node = new_node(ND_VAR, tok);
  node->var = var;
  return node;
}

static struct Obj *
new_var(char *name, struct Type *ty) {
  struct Obj *var = malloc(sizeof(struct Obj));
  var->name = name;
  var->ty = ty;
  return var;
}

static struct Obj *
new_lvar(char *name, struct Type *ty) {
  struct Obj *var = new_var(name, ty);
  var->is_local = true;
  var->next = locals;
  locals = var;
  return var;
}

static struct Obj *
new_gvar(char *name, struct Type *ty) {
  struct Obj *var = new_var(name, ty);
  var->next = globals;
  globals = var;
  return var;
}

static char *
get_ident(struct Token *tok) {
  if (tok->type != TK_IDENT)
    error_tok(tok, "expected an identifier");
  return strndup(tok->loc, tok->len);
}

static int 
get_number(struct Token *tok) {
  if (tok->type != TK_NUM)
    error_tok(tok, "expected a number");
  return tok->val;
}

// declspec = "int"
static struct Type *
declspec(struct Token **rest, struct Token *tok) {
  // Currently support declaration of type int.
  // Use switch case for other type.
  *rest = next_token(tok, "int");
  return ty_int;
}

// func-params = (param ("," param)*)? ")"
// param       = declspec declarator
static struct Type *
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
static struct Type *
type_suffix(struct Token **rest, struct Token *tok, struct Type *ty) {
  if (is_match(tok, "("))
    return func_params(rest, tok->next, ty);

  if (is_match(tok, "[")) {
    int sz = get_number(tok->next);
    tok = next_token(tok->next->next, "]");
    ty = type_suffix(rest, tok, ty);
    return array_type(ty, sz);
  }

  *rest = tok;
  return ty;
}

// declarator = "*"* ident type-suffix
static struct Type *
declarator(struct Token **rest, struct Token *tok, struct Type *ty) {
  while (cur_token(&tok, tok, "*"))
    ty = ptr_type(ty);

  if (tok->type != TK_IDENT)
    error_tok(tok, "Expected a variable name");
  ty = type_suffix(rest, tok->next, ty);
  ty->name = tok;
  return ty;
}

// declaration = declspec (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
static struct Node *
declaration(struct Token **rest, struct Token *tok) {
  struct Type *basety = declspec(&tok, tok);

  struct Node head = {};
  struct Node *cur = &head;
  int i = 0;

  while (!is_match(tok, ";")) {
    if (i++ > 0)
      tok = next_token(tok, ",");

    struct Type *ty = declarator(&tok, tok, basety);
    struct Obj *var = new_lvar(get_ident(ty->name), ty);

    if (!is_match(tok, "="))
      continue;

    struct Node *lhs = new_var_node(var, ty->name);
    struct Node *rhs = assign(&tok, tok->next);
    struct Node *node = new_binary(ND_ASSIGN, lhs, rhs, tok);
    cur = cur->next = new_unary(ND_EXPR_STMT, node, tok);
  }

  struct Node *node = new_node(ND_BLOCK, tok);
  node->body = head.next;
  *rest = tok->next;
  return node;
}

// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "for" "(" expr-stmt expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | "{" compound-stmt
//      | expr-stmt
static struct Node *
stmt(struct Token **rest, struct Token *tok) {
  if (is_match(tok, "return")) {
    struct Node *node = new_node(ND_RETURN, tok);
    node->lhs = expr(&tok, tok->next);
    *rest = next_token(tok, ";");
    return node;
  }

  if (is_match(tok, "if")) {
    struct Node *node = new_node(ND_IF, tok);
    tok = next_token(tok->next, "(");
    node->cond = expr(&tok, tok);
    tok = next_token(tok, ")");
    node->then = stmt(&tok, tok);
    if (is_match(tok, "else"))
      node->els = stmt(&tok, tok->next);
    *rest = tok;
    return node;
  }

  if (is_match(tok, "for")) {
    struct Node *node = new_node(ND_FOR, tok);
    tok = next_token(tok->next, "(");

    node->init = expr_stmt(&tok, tok);

    if (!is_match(tok, ";"))
      node->cond = expr(&tok, tok);
    tok = next_token(tok, ";");

    if (!is_match(tok, ")"))
      node->inc = expr(&tok, tok);
    tok = next_token(tok, ")");

    node->then = stmt(rest, tok);
    return node;
  }

  if (is_match(tok, "while")) {
    struct Node *node = new_node(ND_FOR, tok);
    tok = next_token(tok->next, "(");
    node->cond = expr(&tok, tok);
    tok = next_token(tok, ")");
    node->then = stmt(rest, tok);
    return node;
  }

  if (is_match(tok, "{"))
    return compound_stmt(rest, tok->next);

  return expr_stmt(rest, tok);
}

// compound-stmt = (declaration | stmt)* "}"
static struct Node *
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

// expr-stmt = expr? ";"
static struct Node *
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
static struct Node *
expr(struct Token **rest, struct Token *tok) {
  return assign(rest, tok);
}

// assign = equality ("=" assign)?
static struct Node *
assign(struct Token **rest, struct Token *tok) {
  struct Node *node = equality(&tok, tok);

  if (is_match(tok, "="))
    return new_binary(ND_ASSIGN, node, assign(rest, tok->next), tok);

  *rest = tok;
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
static struct Node *equality(struct Token **rest, struct Token *tok) {
  struct Node *node = relational(&tok, tok);

  for (;;) {
    struct Token *start = tok;

    if (is_match(tok, "==")) {
      node = new_binary(ND_EQ, node, relational(&tok, tok->next), start);
      continue;
    }

    if (is_match(tok, "!=")) {
      node = new_binary(ND_NE, node, relational(&tok, tok->next), start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static struct Node *
relational(struct Token **rest, struct Token *tok) {
  struct Node *node = add(&tok, tok);

  for (;;) {
    struct Token *start = tok;

    if (is_match(tok, "<")) {
      node = new_binary(ND_LT, node, add(&tok, tok->next), start);
      continue;
    }

    if (is_match(tok, "<=")) {
      node = new_binary(ND_LE, node, add(&tok, tok->next), start);
      continue;
    }

    if (is_match(tok, ">")) {
      node = new_binary(ND_LT, add(&tok, tok->next), node, start);
      continue;
    }

    if (is_match(tok, ">=")) {
      node = new_binary(ND_LE, add(&tok, tok->next), node, start);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// In C, `+` operator is overloaded to perform the pointer arithmetic.
// If p is a pointer, p+n adds not n but sizeof(*p)*n to the value of p,
// so that p+n points to the location n elements (not bytes) ahead of p.
// In other words, we need to scale an integer value before adding to a
// pointer value. This function takes care of the scaling.
static struct Node *
new_add(struct Node *lhs, struct Node *rhs, struct Token *tok) {
  add_type(lhs);
  add_type(rhs);

  // num + num
  if (integer_type(lhs->ty) && integer_type(rhs->ty))
    return new_binary(ND_ADD, lhs, rhs, tok);

  if (lhs->ty->base && rhs->ty->base)
    error_tok(tok, "invalid operands");

  // Canonicalize `num + ptr` to `ptr + num`.
  if (!lhs->ty->base && rhs->ty->base) {
    struct Node *tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }

  // ptr + num
  rhs = new_binary(ND_MUL, rhs, new_num(lhs->ty->base->size, tok), tok);
  return new_binary(ND_ADD, lhs, rhs, tok);
}

// Like `+`, `-` is overloaded for the pointer type.
static struct Node *
new_sub(struct Node *lhs, struct Node *rhs, struct Token *tok) {
  add_type(lhs);
  add_type(rhs);

  // num - num
  if (integer_type(lhs->ty) && integer_type(rhs->ty))
    return new_binary(ND_SUB, lhs, rhs, tok);

  // ptr - num
  if (lhs->ty->base && integer_type(rhs->ty)) {
    rhs = new_binary(ND_MUL, rhs, new_num(lhs->ty->base->size, tok), tok);
    add_type(rhs);
    struct Node *node = new_binary(ND_SUB, lhs, rhs, tok);
    node->ty = lhs->ty;
    return node;
  }

  // ptr - ptr, which returns how many elements are between the two.
  if (lhs->ty->base && rhs->ty->base) {
    struct Node *node = new_binary(ND_SUB, lhs, rhs, tok);
    node->ty = ty_int;
    return new_binary(ND_DIV, node, new_num(lhs->ty->base->size, tok), tok);
  }

  error_tok(tok, "invalid operands");
  return 0;
}

// add = mul ("+" mul | "-" mul)*
static struct Node *
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

// mul = unary ("*" unary | "/" unary)*
static struct Node *
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

// unary = ("+" | "-" | "*" | "&") unary
//       | postfix
static struct Node *
unary(struct Token **rest, struct Token *tok) {
  if (is_match(tok, "+"))
    return unary(rest, tok->next);

  if (is_match(tok, "-"))
    return new_unary(ND_NEG, unary(rest, tok->next), tok);

  if (is_match(tok, "&"))
    return new_unary(ND_ADDR, unary(rest, tok->next), tok);

  if (is_match(tok, "*"))
    return new_unary(ND_DEREF, unary(rest, tok->next), tok);

  return postfix(rest, tok);
}

// postfix = primary ("[" expr "]")*
static struct Node *
postfix(struct Token **rest, struct Token *tok) {
  struct Node *node = primary(&tok, tok);

  while (is_match(tok, "[")) {
    // x[y] is short for *(x+y)
    struct Token *start = tok;
    struct Node *idx = expr(&tok, tok->next);
    tok = next_token(tok, "]");
    node = new_unary(ND_DEREF, new_add(node, idx, start), start);
  }
  *rest = tok;
  return node;
}

// funcall = ident "(" (assign ("," assign)*)? ")"
static struct Node *
funcall(struct Token **rest, struct Token *tok) {
  struct Token *start = tok;
  tok = tok->next->next;

  struct Node head = {};
  struct Node *cur = &head;

  while (!is_match(tok, ")")) {
    if (cur != &head)
      tok = next_token(tok, ",");
    cur = cur->next = assign(&tok, tok);
  }

  *rest = next_token(tok, ")");

  struct Node *node = new_node(ND_FUNCALL, start);
  node->funcname = strndup(start->loc, start->len);
  node->args = head.next;
  return node;
}

// primary = "(" expr ")" | "sizeof" unary | ident func-args? | num
static struct Node *
primary(struct Token **rest, struct Token *tok) {
  if (is_match(tok, "(")) {
    struct Node *node = expr(&tok, tok->next);
    *rest = next_token(tok, ")");
    return node;
  }

  if (is_match(tok, "sizeof")) {
    struct Node *node = unary(rest, tok->next);
    add_type(node);
    return new_num(node->ty->size, tok);
  }

  if (tok->type == TK_IDENT) {
    // Function call
    if (is_match(tok->next, "("))
      return funcall(rest, tok);

    // Variable
    struct Obj *var = find_var(tok);
    if (!var)
      error_tok(tok, "undefined variable");
    *rest = tok->next;
    return new_var_node(var, tok);
  }

  if (tok->type == TK_NUM) {
    struct Node *node = new_num(tok->val, tok);
    *rest = tok->next;
    return node;
  }

  error_tok(tok, "expected an expression");
  return 0;
}

static void 
create_param_lvars(struct Type *param) {
  if (param) {
    create_param_lvars(param->next);
    new_lvar(get_ident(param->name), param);
  }
}

static struct Token *
function(struct Token *tok, struct Type *basety) {
  struct Type *ty = declarator(&tok, tok, basety);

  struct Obj *fn = new_gvar(get_ident(ty->name), ty);
  fn->is_function = true;

  locals = NULL;
  create_param_lvars(ty->params);
  fn->params = locals;

  tok = next_token(tok, "{");
  fn->body = compound_stmt(&tok, tok);
  fn->locals = locals;
  return tok;
}

static struct Token *
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

// Lookahead tokens and returns true if a given token is a start
// of a function definition or declaration.
static bool 
is_function(struct Token *tok) {
  if (is_match(tok, ";"))
    return false;

  struct Type dummy = {};
  struct Type *ty = declarator(&tok, tok, &dummy);
  return ty->kind == TY_FUNC;
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
