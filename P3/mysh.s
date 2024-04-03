	.file	"mysh.c"
	.text
	.globl	DirectoryOne
	.section	.rodata
.LC0:
	.string	"/usr/local/bin"
	.section	.data.rel.local,"aw"
	.align 8
	.type	DirectoryOne, @object
	.size	DirectoryOne, 8
DirectoryOne:
	.quad	.LC0
	.globl	DirectoryTwo
	.section	.rodata
.LC1:
	.string	"/usr/bin"
	.section	.data.rel.local
	.align 8
	.type	DirectoryTwo, @object
	.size	DirectoryTwo, 8
DirectoryTwo:
	.quad	.LC1
	.globl	DirectoryThree
	.section	.rodata
.LC2:
	.string	"/bin"
	.section	.data.rel.local
	.align 8
	.type	DirectoryThree, @object
	.size	DirectoryThree, 8
DirectoryThree:
	.quad	.LC2
	.section	.rodata
.LC3:
	.string	"cd"
	.text
	.globl	cd
	.type	cd, @function
cd:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	chdir@PLT
	cmpl	$-1, %eax
	jne	.L3
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
.L3:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	cd, .-cd
	.globl	which
	.type	which, @function
which:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rax
	movq	%rax, -32(%rbp)
	leaq	.LC1(%rip), %rax
	movq	%rax, -24(%rbp)
	leaq	.LC2(%rip), %rax
	movq	%rax, -16(%rbp)
	nop
	movq	-8(%rbp), %rax
	subq	%fs:40, %rax
	je	.L5
	call	__stack_chk_fail@PLT
.L5:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	which, .-which
	.section	.rodata
.LC4:
	.string	"\n"
.LC5:
	.string	"getcwd() error"
	.text
	.globl	pwd
	.type	pwd, @function
pwd:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$1040, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-1040(%rbp), %rax
	movl	$1024, %esi
	movq	%rax, %rdi
	call	getcwd@PLT
	testq	%rax, %rax
	je	.L7
	leaq	-1040(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rax, %rdx
	leaq	-1040(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	write@PLT
	movl	$1, %edx
	leaq	.LC4(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	write@PLT
	jmp	.L6
.L7:
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	nop
.L6:
	movq	-8(%rbp), %rax
	subq	%fs:40, %rax
	je	.L11
	call	__stack_chk_fail@PLT
.L11:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	pwd, .-pwd
	.section	.rodata
.LC6:
	.string	"goodbye\n"
	.text
	.globl	shell_exit
	.type	shell_exit, @function
shell_exit:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$8, %edx
	leaq	.LC6(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	write@PLT
	movl	$0, %edi
	call	exit@PLT
	.cfi_endproc
.LFE9:
	.size	shell_exit, .-shell_exit
	.section	.rodata
.LC7:
	.string	"I am in batch mode?!?!?!"
	.align 8
.LC8:
	.string	"Welcome to my fiendish little bomb\n"
.LC9:
	.string	"exit"
.LC10:
	.string	"which"
	.text
	.globl	main
	.type	main, @function
main:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movl	$0, %edi
	call	isatty@PLT
	testl	%eax, %eax
	jne	.L14
	cmpl	$1, -36(%rbp)
	je	.L15
.L14:
	cmpl	$2, -36(%rbp)
	jne	.L16
.L15:
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
.L16:
	cmpl	$1, -36(%rbp)
	jne	.L17
	movl	$0, %edi
	call	isatty@PLT
	testl	%eax, %eax
	je	.L17
	movl	$36, %edx
	leaq	.LC8(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	write@PLT
.L21:
	movl	$0, %edi
	call	capygetline@PLT
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -20(%rbp)
	movq	-16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	leaq	.LC9(%rip), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L18
	movl	$0, %eax
	call	shell_exit
.L18:
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	leaq	.LC3(%rip), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L19
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	cd
.L19:
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	leaq	.LC10(%rip), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L20
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	which
.L20:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	jmp	.L21
.L17:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
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
