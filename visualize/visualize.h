#ifndef VISUALIZE_H
#define VISUALIZE_H

#include "../token/token.h"

// ANSI color codes for terminal output
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_DIM     "\033[2m"

// Background colors
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"

// Unicode box drawing characters for tree visualization
#define TREE_VERTICAL   "│"
#define TREE_BRANCH     "├──"
#define TREE_LAST       "└──"
#define TREE_SPACE      "    "
#define TREE_CONTINUE   "│   "

// Shape symbols
#define SHAPE_CIRCLE    "●"
#define SHAPE_SQUARE    "■"
#define SHAPE_TRIANGLE  "▲"
#define SHAPE_DIAMOND   "◆"
#define SHAPE_STAR      "★"
#define SHAPE_ARROW     "→"

// Visualize tokens with colors
void visualize_tokens(struct Token *tok);

// Visualize AST tree structure
void visualize_ast(struct Obj *prog);

// Helper function to visualize a single AST node
void visualize_node(struct Node *node, int depth, bool is_last, char *prefix);

#endif // VISUALIZE_H