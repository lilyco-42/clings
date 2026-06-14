# runtime_x86.s - x86-64 runtime stubs for nccl-cc (LINUX ONLY)
#
# Provides: _start, putchar, print_int, printf
# Links with compiled output to create standalone x86-64 executables:
#   gcc -nostdlib -static out.s runtime_x86.s
#
# NOTE: macOS forbids raw syscalls / custom _start; on macOS link
# against libc instead (cc out.s) and use the real printf.
#
# Linux x86-64 syscall convention:
#   syscall number in %rax
#   args in %rdi, %rsi, %rdx, %r10, %r8, %r9
#   return value in %rax

.text

# _start: entry point, calls main(), then exit(result)
.globl _start
_start:
    call main
    movl %eax, %edi         # exit code = main() return value
    movl $60, %eax          # syscall: exit
    syscall

# putchar(int ch): write one character to stdout
.globl putchar
putchar:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movb %dil, -1(%rbp)    # store char on stack
    movl $1, %eax           # syscall: write
    movl $1, %edi           # fd = stdout
    leaq -1(%rbp), %rsi    # buf = &char on stack
    movl $1, %edx           # len = 1
    syscall
    leave
    ret

# print_int(int n): print integer to stdout (recursive)
.globl print_int
print_int:
    pushq %rbp
    movq %rsp, %rbp
    pushq %rbx
    subq $16, %rsp
    # saved %rbx lives at -8(%rbp); keep n BELOW it — storing at
    # -4(%rbp) would overwrite the upper half of the saved register
    movl %edi, -12(%rbp)   # save n

    # handle negative
    testl %edi, %edi
    jge .pi_pos
    movl $45, %edi          # '-'
    call putchar
    negl -12(%rbp)

.pi_pos:
    movl -12(%rbp), %eax
    cltd
    movl $10, %ecx
    idivl %ecx              # eax = n/10, edx = n%10
    movl %edx, %ebx        # save remainder

    testl %eax, %eax
    je .pi_single

    # recursively print n/10
    movl %eax, %edi
    call print_int

.pi_single:
    # print last digit
    movl %ebx, %edi
    addl $48, %edi          # convert to ASCII '0'-'9'
    call putchar

    addq $16, %rsp
    popq %rbx
    leave
    ret

# printf(fmt, arg1, arg2, ...): simplified printf supporting %d and %c
.globl printf
printf:
    pushq %rbp
    movq %rsp, %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    subq $48, %rsp

    # save potential args (rsi, rdx, rcx, r8, r9).
    # callee-saved regs occupy -8..-40(%rbp); the arg array must live
    # strictly below them or popq would restore clobbered values
    movq %rsi, -88(%rbp)
    movq %rdx, -80(%rbp)
    movq %rcx, -72(%rbp)
    movq %r8, -64(%rbp)
    movq %r9, -56(%rbp)

    movq %rdi, %r12         # r12 = fmt string pointer
    leaq -88(%rbp), %r13    # r13 = pointer to saved args
    xorl %r14d, %r14d       # r14 = arg index

.pf_loop:
    movzbl (%r12), %eax
    testl %eax, %eax
    je .pf_done

    cmpl $37, %eax          # '%'
    jne .pf_char

    # format specifier
    incq %r12
    movzbl (%r12), %eax

    cmpl $100, %eax         # 'd'
    jne .pf_check_c

    # %d: print integer
    movq %r14, %rcx
    shlq $3, %rcx
    addq %r13, %rcx
    movl (%rcx), %edi
    incq %r14
    call print_int
    jmp .pf_next

.pf_check_c:
    cmpl $99, %eax          # 'c'
    jne .pf_check_s

    # %c: print character
    movq %r14, %rcx
    shlq $3, %rcx
    addq %r13, %rcx
    movl (%rcx), %edi
    incq %r14
    call putchar
    jmp .pf_next

.pf_check_s:
    cmpl $115, %eax         # 's'
    jne .pf_next

    # %s: print string
    movq %r14, %rcx
    shlq $3, %rcx
    addq %r13, %rcx
    movq (%rcx), %r15
    incq %r14
.pf_str_loop:
    movzbl (%r15), %edi
    testl %edi, %edi
    je .pf_next
    call putchar
    incq %r15
    jmp .pf_str_loop

.pf_char:
    # literal character (escape sequences are already decoded
    # by the compiler's tokenizer - bytes here are real bytes)
    movl %eax, %edi
    call putchar

.pf_next:
    incq %r12
    jmp .pf_loop

.pf_done:
    addq $48, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    leave
    ret
