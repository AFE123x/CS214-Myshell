	.file	"mysh.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"opendir"
.LC1:
	.string	"%s/%s"
.LC2:
	.string	"."
.LC3:
	.string	".."
.LC4:
	.string	"\n"
	.text
	.type	search_directory, @function
search_directory:
.LFB66:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$1048, %rsp
	.cfi_def_cfa_offset 1088
	movq	%rdi, %r12
	movq	%rsi, %r13
	movq	%fs:40, %rax
	movq	%rax, 1032(%rsp)
	xorl	%eax, %eax
	call	opendir@PLT
	movq	%rax, %rbp
	testq	%rax, %rax
	je	.L11
.L2:
	movq	%rbp, %rdi
	call	readdir@PLT
	movq	%rax, %rbx
	testq	%rax, %rax
	je	.L12
	movq	%rsp, %r10
	movl	$128, %ecx
	movl	$0, %eax
	movq	%r10, %rdi
	rep stosq
	addq	$19, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 1096
	pushq	%rbx
	.cfi_def_cfa_offset 1104
	movq	%r12, %r9
	leaq	.LC1(%rip), %r8
	movl	$1024, %ecx
	movl	$1, %edx
	movl	$1024, %esi
	movq	%r10, %rdi
	call	__snprintf_chk@PLT
	leaq	.LC2(%rip), %rsi
	movq	%rbx, %rdi
	call	strcmp@PLT
	addq	$16, %rsp
	.cfi_def_cfa_offset 1088
	testl	%eax, %eax
	je	.L2
	leaq	.LC3(%rip), %rsi
	movq	%rbx, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L2
	movq	%r13, %rsi
	movq	%rbx, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L2
	movq	%rsp, %rbx
	movq	%rbx, %rdi
	call	strlen@PLT
	movq	%rax, %rdx
	movq	%rbx, %rsi
	movl	$1, %edi
	call	write@PLT
	movl	$1, %edx
	leaq	.LC4(%rip), %rsi
	movl	$1, %edi
	call	write@PLT
	movq	%rbp, %rdi
	call	closedir@PLT
	movl	$1, %eax
	jmp	.L1
.L11:
	leaq	.LC0(%rip), %rdi
	call	perror@PLT
	movl	$-1, %eax
	jmp	.L1
.L12:
	movq	%rbp, %rdi
	call	closedir@PLT
	movl	$0, %eax
.L1:
	movq	1032(%rsp), %rdx
	subq	%fs:40, %rdx
	jne	.L13
	addq	$1048, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
.L13:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE66:
	.size	search_directory, .-search_directory
	.section	.rodata.str1.1
.LC5:
	.string	"cd"
	.text
	.globl	cd
	.type	cd, @function
cd:
.LFB65:
	.cfi_startproc
	endbr64
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	chdir@PLT
	cmpl	$-1, %eax
	je	.L17
.L14:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L17:
	.cfi_restore_state
	leaq	.LC5(%rip), %rdi
	call	perror@PLT
	jmp	.L14
	.cfi_endproc
.LFE65:
	.size	cd, .-cd
	.globl	which
	.type	which, @function
which:
.LFB67:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	%rdi, %rsi
	movq	DirectoryOne(%rip), %rdi
	call	search_directory
	testl	%eax, %eax
	je	.L22
.L19:
	testl	%eax, %eax
	je	.L23
.L18:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L22:
	.cfi_restore_state
	movq	%rbx, %rsi
	movq	DirectoryTwo(%rip), %rdi
	call	search_directory
	jmp	.L19
.L23:
	movq	%rbx, %rsi
	movq	DirectoryThree(%rip), %rdi
	call	search_directory
	jmp	.L18
	.cfi_endproc
.LFE67:
	.size	which, .-which
	.section	.rodata.str1.1
.LC6:
	.string	"getcwd() error"
	.text
	.globl	pwd
	.type	pwd, @function
pwd:
.LFB68:
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
	movq	%rsp, %rdi
	movl	$1024, %esi
	call	getcwd@PLT
	testq	%rax, %rax
	je	.L25
	movq	%rsp, %rbx
	movq	%rbx, %rdi
	call	strlen@PLT
	movq	%rax, %rdx
	movq	%rbx, %rsi
	movl	$1, %edi
	call	write@PLT
	movl	$1, %edx
	leaq	.LC4(%rip), %rsi
	movl	$1, %edi
	call	write@PLT
.L24:
	movq	1032(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L29
	addq	$1040, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
.L25:
	.cfi_restore_state
	leaq	.LC6(%rip), %rdi
	call	perror@PLT
	jmp	.L24
.L29:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE68:
	.size	pwd, .-pwd
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC7:
	.string	"its so hard to say goodbye :(\n"
	.text
	.globl	shell_exit
	.type	shell_exit, @function
shell_exit:
.LFB69:
	.cfi_startproc
	endbr64
	pushq	%rax
	.cfi_def_cfa_offset 16
	popq	%rax
	.cfi_def_cfa_offset 8
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$31, %edx
	leaq	.LC7(%rip), %rsi
	movl	$1, %edi
	call	write@PLT
	movl	$0, %edi
	call	exit@PLT
	.cfi_endproc
.LFE69:
	.size	shell_exit, .-shell_exit
	.globl	run_program
	.type	run_program, @function
run_program:
.LFB70:
	.cfi_startproc
	endbr64
	ret
	.cfi_endproc
.LFE70:
	.size	run_program, .-run_program
	.section	.rodata.str1.1
.LC8:
	.string	"I am in batch mode?!?!?!"
	.section	.rodata.str1.8
	.align 8
.LC9:
	.string	"Welcome to my fiendish little bomb\n"
	.section	.rodata.str1.1
.LC10:
	.string	"exit"
.LC11:
	.string	"which"
.LC12:
	.string	"pwd"
	.text
	.globl	main
	.type	main, @function
main:
.LFB71:
	.cfi_startproc
	endbr64
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	%edi, %ebx
	movl	$0, %edi
	call	isatty@PLT
	testl	%eax, %eax
	jne	.L34
	cmpl	$1, %ebx
	je	.L35
.L34:
	cmpl	$2, %ebx
	je	.L35
.L36:
	cmpl	$1, %ebx
	je	.L44
.L37:
	movl	$0, %eax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
.L35:
	.cfi_restore_state
	leaq	.LC8(%rip), %rsi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk@PLT
	jmp	.L36
.L44:
	movl	$0, %edi
	call	isatty@PLT
	testl	%eax, %eax
	je	.L37
	movl	$36, %edx
	leaq	.LC9(%rip), %rsi
	movl	$1, %edi
	call	write@PLT
	jmp	.L42
.L45:
	call	shell_exit
.L46:
	movq	8(%rbx), %rdi
	call	cd
	jmp	.L39
.L47:
	movq	8(%rbx), %rdi
	call	which
	jmp	.L40
.L41:
	movq	%rbx, %rdi
	call	free@PLT
	movq	%rbp, %rdi
	call	free@PLT
.L42:
	movl	$0, %edi
	call	capygetline@PLT
	movq	%rax, %rbp
	movq	(%rax), %rbx
	movq	(%rbx), %r12
	leaq	.LC10(%rip), %rsi
	movq	%r12, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L45
	leaq	.LC5(%rip), %rsi
	movq	%r12, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L46
.L39:
	leaq	.LC11(%rip), %rsi
	movq	(%rbx), %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L47
.L40:
	leaq	.LC12(%rip), %rsi
	movq	(%rbx), %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L41
	call	pwd
	jmp	.L41
	.cfi_endproc
.LFE71:
	.size	main, .-main
	.globl	DirectoryThree
	.section	.rodata.str1.1
.LC13:
	.string	"/bin"
	.section	.data.rel.local,"aw"
	.align 8
	.type	DirectoryThree, @object
	.size	DirectoryThree, 8
DirectoryThree:
	.quad	.LC13
	.globl	DirectoryTwo
	.section	.rodata.str1.1
.LC14:
	.string	"/usr/bin"
	.section	.data.rel.local
	.align 8
	.type	DirectoryTwo, @object
	.size	DirectoryTwo, 8
DirectoryTwo:
	.quad	.LC14
	.globl	DirectoryOne
	.section	.rodata.str1.1
.LC15:
	.string	"/usr/local/bin"
	.section	.data.rel.local
	.align 8
	.type	DirectoryOne, @object
	.size	DirectoryOne, 8
DirectoryOne:
	.quad	.LC15
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
