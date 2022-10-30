#ifndef TOKEN_H
#define TOKEN_H

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//
// token.c
//

// Token type
typedef enum {
  TK_IDENT,   // Identifiers
  TK_PUNCT,   // Punctuators
  TK_KEYWORD, // Keywords
  TK_NUM,     // Numeric literals
  TK_EOF,     // End-of-file markers
} TK_TYPE;


// Token type
struct Token {
  TK_TYPE type;       // Token kind
  struct Token *next; // Next token
  int val;            // If kind is TK_NUM, its value
  char *loc;          // Token location
  int len;            // Token length
};

// Error handling
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(struct Token *tok, char *fmt, ...);

// Util functions
bool is_match(struct Token *tok, const char *op);
bool cur_token(struct Token **rest, struct Token *tok, char *str);
struct Token *next_token(struct Token *tok, char *op);

// Called in main function
struct Token *split_token(char *input);

//
// parse.c
//

// Variable or function
struct Obj {
  struct Obj *next;
  char *name;         // Variable name
  struct Type *ty;    // Type
  bool is_local;      // local or global/function

  // Local variable
  int offset;

  // Global variable or function
  bool is_function;

  // Function
  struct Obj *params;
  struct Node *body;
  struct Obj *locals;
  int stack_size;
};

// AST node
typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NEG,       // unary -
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_ADDR,      // unary &
  ND_DEREF,     // unary *
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_FOR,       // "for" or "while"
  ND_BLOCK,     // { ... }
  ND_FUNCALL,   // Function call
  ND_EXPR_STMT, // Expression statement
  ND_VAR,       // Variable
  ND_NUM,       // Integer
} NodeKind;

// AST node type
struct Node {
  NodeKind kind; // Node kind
  struct Node *next;    // Next node
  struct Type *ty;      // Type, e.g. int or pointer to int
  struct Token *tok;    // Representative token

  struct Node *lhs;     // Left-hand side
  struct Node *rhs;     // Right-hand side

  // "if" or "for" statement
  struct Node *cond;
  struct Node *then;
  struct Node *els;
  struct Node *init;
  struct Node *inc;

  // Block
  struct Node *body;

  // Function call
  char *funcname;
  struct Node *args;

  struct Obj *var;      // Used if kind == ND_VAR
  int val;       // Used if kind == ND_NUM
};

struct Obj *parse_token(struct Token *tok);

//
// type.c
//

typedef enum {
  TY_INT,
  TY_PTR,
  TY_FUNC,
  TY_ARRAY,
} TypeKind;

struct Type {
  TypeKind kind;
  int size;      // sizeof() value

  // Pointer-to or array-of type
  struct Type *base;

  // Declaration
  struct Token *name;

  // Array
  int array_len;

  // Function type
  struct Type *return_ty;
  struct Type *params;
  struct Type *next;
};

extern struct Type *ty_int;

bool integer_type(struct Type *ty);
struct Type *copy_type(struct Type *ty);
struct Type *ptr_type(struct Type *base);
struct Type *func_type(struct Type *return_ty);
struct Type *array_type(struct Type *base, int size);
void add_type(struct Node *node);

#endif // End of TOKEN_H