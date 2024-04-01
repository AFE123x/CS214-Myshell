	.file	"parser.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB42:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	subq	$1040, %rsp
	.cfi_def_cfa_offset 1056
	movq	%fs:40, %rax
	movq	%rax, 1032(%rsp)
	xorl	%eax, %eax
	movq	%rsp, %rbx
	movl	$128, %ecx
	movq	%rbx, %rdi
	rep stosq
	movl	$100, %edx
	movq	%rbx, %rsi
	movl	$0, %edi
	call	read@PLT
	leal	1(%rax), %edx
	movslq	%edx, %rdx
	movq	%rbx, %rsi
	movl	$1, %edi
	call	write@PLT
	movb	$0, 1023(%rsp)
	movq	1032(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L4
	movl	$0, %eax
	addq	$1040, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
.L4:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE42:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
