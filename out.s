	.text
	.comm	a,4,4
	.text
	.text
	.globl	fred
	.type	fred, @function
fred:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$20, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
	.comm	result,4,4
	.comm	x,4,4
	.comm	y,0,0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$8, %r9
	movl	%r9d, a(%rip)
	movq	$10, %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$15, %r8
	movq	%r8, %rdi
	call	fred
	movq	%rax, %r9
	movl	%r9d, result(%rip)
	movzbl	result(%rip), %r8d
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$15, %r8
	movq	%r8, %rdi
	call	fred
	movq	%rax, %r9
	movq	$10, %r8
	addq	%r9, %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, x(%rip)
	leaq	x(%rip), %r8
	movq	%r8, y(%rip)
	movq	y(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movzbl	a(%rip), %r8d
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L2
L2:
	popq %rbp
	ret
