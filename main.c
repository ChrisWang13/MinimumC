#include "token/token.h"

// Set Debug mode to non-zoro to enable debug.
#define DEBUG_TOKEN 0
#define DEBUG_PARSE 0

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: Failed: argv[1] has to be input program string", argv[0]);

  // Split token
  struct Token* tok = split_token(argv[1]);
  // Parse token
  struct Obj *obj = parse_token(tok);
  // Generate naive x86 code
  codegen(obj);

#if DEBUG_TOKEN
  printf("----------------------------------------------\n");
  printf("Debug token info:\n");
  printf("----------------------------------------------\n");
  const char* token_type[]= {
    "IDENT",   // Identifiers
    "PUNCT",   // Punctuators
    "KEYWORD", // Keywords
    "NUM",     // Numeric literals
    "EOF"      // End-of-file markers
  };
  struct Token* debug_tok = tok;  
  for ( ; debug_tok != NULL; debug_tok = debug_tok->next) {
    printf("Kind = %s, Len = %d, Loc = %s, Val = %d\n",
            token_type[debug_tok->type], debug_tok->len, debug_tok->loc, debug_tok->val);
  }
#endif //End of DEBUG_TOKEN


#if DEBUG_PARSE
  printf("----------------------------------------------\n");
  printf("Debug parse info:\n");
  printf("----------------------------------------------\n");
  const char* type_kind[]= {
    "TY_INT",
    "TY_PTR",
    "TY_FUNC",
    "TY_ARRAY",
  };
  struct Obj* debug_obj = obj;  
  for ( ; debug_obj != NULL; debug_obj = debug_obj->next) {
    printf("Name = %s, kind = %s, is_function = %d, is_local = %d\n",
            debug_obj->name, type_kind[debug_obj->ty->kind], debug_obj->is_function, debug_obj->is_local);
  }
#endif // End of DEBUG_PARSE

  return 0;
}
