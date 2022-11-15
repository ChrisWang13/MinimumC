// declspec = "int"
struct Type *
declspec(struct Token **rest, struct Token *tok);

// declarator = "*"* ident type-suffix
struct Type *
declarator(struct Token **rest, struct Token *tok, struct Type *ty);

// type-suffix = "(" func-params
//             | "[" num "]" type-suffix
//             | ε
struct Type *
type_suffix(struct Token **rest, struct Token *tok, struct Type *ty);

struct Token *
function(struct Token *tok, struct Type *basety);

struct Node *
stmt(struct Token **rest, struct Token *tok);

struct Node *
compound_stmt(struct Token **rest, struct Token *tok);

struct Node *
declaration(struct Token **rest, struct Token *tok);

struct Node *
new_node(NodeKind kind, struct Token *tok);

struct Node *
new_num(int val, struct Token *tok);

struct Node *
new_binary(NodeKind kind, struct Node *lhs, struct Node *rhs, struct Token *tok);

struct Node *
new_add(struct Node *lhs, struct Node *rhs, struct Token *tok);

struct Node *
new_sub(struct Node *lhs, struct Node *rhs, struct Token *tok);

struct Node *
new_unary(NodeKind kind, struct Node *expr, struct Token *tok);

struct Node *
expr(struct Token **rest, struct Token *tok);

struct Node *
assign(struct Token **rest, struct Token *tok);

struct Node *
equality(struct Token **rest, struct Token *tok);

struct Node *
relational(struct Token **rest, struct Token *tok);

struct Node *
add(struct Token **rest, struct Token *tok);

struct Node *
mul(struct Token **rest, struct Token *tok);

