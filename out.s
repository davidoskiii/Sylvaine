	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.comm	i,8,8
	.comm	j,8,8
	.comm	k,8,8
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$6, %r8
	movq	%r8, i(%rip)
	movq	$12, %r8
	movq	%r8, j(%rip)
	movq	i(%rip), %r8
	movq	j(%rip), %r9
	cmpq	%r9, %r8
	jge	L1
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	jmp	L2
L1:
	movq	j(%rip), %r8
	movq	%r8, %rdi
	call	printint
L2:
	movq	$0, %r8
	movq	%r8, j(%rip)
L3:
	movq	j(%rip), %r8
	movq	$6, %r9
	cmpq	%r9, %r8
	jge	L4
	movq	j(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	j(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, j(%rip)
	jmp	L3
L4:
	movq	$0, %r8
	movq	%r8, k(%rip)
L5:
	movq	k(%rip), %r8
	movq	$11, %r9
	cmpq	%r9, %r8
	jge	L6
	movq	k(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	k(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, k(%rip)
	jmp	L5
L6:
	movl	$0, %eax
	popq	%rbp
	ret
