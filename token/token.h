#ifndef _TOKEN_H_
#define _TOKEN_H_

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
typedef struct Type Type;
typedef struct Node Node;

// Variable or function
typedef struct Obj Obj;
struct Obj {
  Obj *next;
  char *name;       // Variable name
  bool is_local;    // local or global/function
  int offset;       // Local variable
  bool is_function; // Global variable or function
};

// AST node kind, differ from token
typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_RETURN,    // "return"
  ND_VAR,       // Variable
  ND_NUM,       // Integer
} NodeKind;

// AST node type
struct Node {
  NodeKind kind; // Node kind
  Node *next;    // Next node
  struct Token *tok;    // Representative token

  Node *lhs;     // Left-hand side
  Node *rhs;     // Right-hand side

  Obj *var;      // Used if kind == ND_VAR
  int val;       // Used if kind == ND_NUM
};

#endif // End of _TOKEN_H_