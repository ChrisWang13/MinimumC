#include "token/token.h"

#define DEBUG_TOKEN

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: Failed: argv[1] has to be input program string", argv[0]);

  // Split token
  struct Token* tok = split_token(argv[1]);

  #ifdef DEBUG_TOKEN
  const char* PrintType[]= {
    "IDENT",   // Identifiers
    "PUNCT",   // Punctuators
    "KEYWORD", // Keywords
    "NUM",     // Numeric literals
    "EOF"      // End-of-file markers
  };
  struct Token* t = tok;  
  for ( ; t->next != NULL; t = t->next) {
    printf("Kind = %s, Len = %d, Loc = %s, Val = %d\n",
   PrintType[t->type],   t->len,   t->loc,   t->val);
  }
  #endif // End of DEBUG_TOKEN

  return 0;
}
