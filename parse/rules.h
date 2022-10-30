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
