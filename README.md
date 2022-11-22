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

## Warning:
Since both debugging and code generation print the output to stdout, it is buggy to enable debugging while generating asm file. 

## Reference:
1. C Coding Standard: https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html

2. Crafting Interpreters:https://craftinginterpreters.com/contents.html