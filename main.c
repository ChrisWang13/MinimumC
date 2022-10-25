#include "token/token.h"

#define DEBUG_ON

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: Failed: argv[1] has to be input program string", argv[0]);

  // Split the token and 
  Token* tok = split_token(argv[1]);

  #ifdef DEBUG_ON
  const char* PrintType[]= {
    "IDENT",   // Identifiers
    "PUNCT",   // Punctuators
    "KEYWORD", // Keywords
    "NUM",     // Numeric literals
    "EOF"      // End-of-file markers
  };
  Token* t = tok;  
  for ( ; t->next != NULL; t = t->next) {
    printf("Kind = %s, Len = %d, Loc = %s, Val = %d\n",
   PrintType[t->kind],   t->len,   t->loc,   t->val);
  }
  #endif // End of DEBUG_ON

  return 0;
}
