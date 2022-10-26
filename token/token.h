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

// Called in main function
struct Token *split_token(char *input);

#endif // End of _TOKEN_H_