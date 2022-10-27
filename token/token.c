#include "token.h"

// Input string
static char *input_str;


/***********************************************************************
 * Error handling
 **********************************************************************/

// Reports an error and exit.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports an error location and exit.
static void verror_at(char *loc, char *fmt, va_list ap) {
  int pos = loc - input_str;
  fprintf(stderr, "%s\n", input_str);
  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(loc, fmt, ap);
}

void error_tok(struct Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->loc, fmt, ap);
}

/***********************************************************************
 * Util functions
 **********************************************************************/

// Check if token is matched to char* op.
bool is_match(struct Token *tok, const char *op) {
  return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0'; 
}

// Used for linked list token in parsing. Call is_match to check. 
bool cur_token(struct Token **rest, struct Token *tok, char *str) {
  if (is_match(tok, str)) {
    *rest = tok->next;
    return true;
  }
  *rest = tok;
  return false;
}

// Check if current token is `op`, return next token if current is matched.
struct Token *next_token(struct Token *tok, char *op) {
  if (!is_match(tok, op))
    error_tok(tok, "expected '%s'", op);
  return tok->next;
}


// Create a new token.
static struct Token *new_token(TK_TYPE type, char *start, char *end) {
  struct Token *tok = calloc(1, sizeof(struct Token));
  tok->type = type;
  tok->loc = start;
  tok->len = end - start;
  return tok;
}

static bool is_startwith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

// Returns true if c is valid as the first character of an identifier.
// INVALID: Starts with a special character other than '_'.
// INVALID: Starts with a digit.
static bool is_ident1(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// Returns true if c is valid as a non-first character of an identifier.
static bool is_ident2(char c) {
  return is_ident1(c) || ('0' <= c && c <= '9');
}

// Read a punctuator token from p and returns its length.
static int read_punct(char *p) {
  if (is_startwith(p, "==") || is_startwith(p, "!=") ||
      is_startwith(p, "<=") || is_startwith(p, ">="))
    return 2;
  return ispunct(*p) ? 1 : 0;
}

static bool is_keyword(struct Token *tok) {
  static const char *kw[] = {
    "return", "if", "else", "for", "while", "int",
  };

  for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
    if (is_match(tok, kw[i]))
      return true;
  return false;
}

// Indetifier? or keywords?
static void convert_keywords(struct Token *tok) {
  for (struct Token *t = tok; t->type != TK_EOF; t = t->next)
    if (is_keyword(t))
      t->type = TK_KEYWORD;
}

// Tokenize a given string and returns new tokens.
struct Token *split_token(char *p) {
  input_str = p;
  struct Token dummy = {}; // Dummy header
  struct Token *cur = &dummy;

  while (*p) {
    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Numeric literal
    if (isdigit(*p)) {
      cur->next = new_token(TK_NUM, p, p);
      cur = cur->next;
      char *q = p;
      cur->val = strtoul(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    // Identifier or keyword
    if (is_ident1(*p)) {
      char *start = p;
      do {
        p++;
      } while (is_ident2(*p));
      cur->next = new_token(TK_IDENT, start, p);
      cur = cur->next;
      continue;
    }

    // Punctuators
    int punct_len = read_punct(p);
    if (punct_len) {
      cur->next = new_token(TK_PUNCT, p, p + punct_len);
      cur = cur->next;
      p += cur->len;
      continue;
    }

    error_at(p, "invalid token");
  }

  cur->next = new_token(TK_EOF, p, p);
  cur = cur->next;
  convert_keywords(dummy.next);

  return dummy.next; // Returns next element of dummy header
}
