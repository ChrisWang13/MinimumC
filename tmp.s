  .globl main
  .text
main:
  push %rbp
  mov %rsp, %rbp
  sub $0, %rsp
  mov $0, %rax
  jmp .L.return.main
.L.return.main:
  mov %rbp, %rsp
  pop %rbp
  ret
