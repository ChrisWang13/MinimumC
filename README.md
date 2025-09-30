## MinimumC

MinimumC is a minimum C compiler intended to parse tokens, check grammar correctness and generate x86 code.


## Usage
To compile MinimumC, run `make` in the root folder of this project.

To run MinimumC with specific arguments, run `./MinimumC "YOUR_INPUT_PROGRAM_HERE"` and output of  `printf` will be redirect to asm file.

## Testing
To test the code works, run `make test`. `tesh.sh` is the test
file, it first generate `lib.o` with gcc options `-x`(specificy input language, here as C),`-c`(no link) from redirect texts.
Then it checks whether the actual return value and the estimated value are equal.

## Debugging
To enable terminal debugging in MinimumC, set
`DEBUG_TOKEN`, `DEBUG_PARSE`, `DEBUG_CODEGEN` to value `1`
```
// Set Debug mode to non-zoro to enable debug.
#define DEBUG_TOKEN 1
#define DEBUG_PARSE 1
#define DEBUG_CODEGEN 1
```

## Visualization
To enable colorful visualization of the compilation process, set `VISUALIZE` to value `1` in `main.c`:
```c
#define VISUALIZE 1
```

The visualization provides:
- **Token Visualization**: Shows all tokens with different colors and shapes:
  - Keywords (★) in magenta
  - Identifiers (●) in green
  - Punctuators (■) in yellow
  - Numbers (◆) in cyan

- **AST Visualization**: Shows the abstract syntax tree structure with:
  - Tree structure using Unicode box drawing characters
  - Different colors and shapes for different node types
  - Function parameters and local variables
  - Proper indentation showing parent-child relationships

## Warning:
Since both debugging/visualization and code generation print the output to stdout, it is buggy to enable debugging or visualization while generating asm file. 

## Reference:
1. C Coding Standard: https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html

2. Crafting Interpreters:https://craftinginterpreters.com/contents.html