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