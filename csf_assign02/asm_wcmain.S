/*
 * Assembly language main function implementation
 */

#define MAX_WORDLEN 63


	.section .rodata

	sTooManyArgsMsg: .string "Error: too many arguments."
	sInvalidFileMsg: .string "Error: could not open file \"%s\" for reading."
	sReadingString: .string "r"

	.section .data

	filePointer: .space 8
	buckets: .space 8


	.section .text

	.globl main
main:
   /* Notes on registers used:
	* Callee saved registers: %r12, %r13
	* Caller saved registers: %r10
	* Subroutine arguments: %rdi
	* Return register: none, but store string in %rdi
	*/

	pushq %r12
	pushq %r13
	pushq %r14

	movl %edi, %r12d
	movq %rsi, %r13


	cmpl $2, %edi /* check if argc > 2 */
	
	ja .Lmain_too_many_arg
	
	movq $stdin, $filePointer

	cmpl $2, %edi

	jne .Lmain_stdin

	movq 1($rsi), %rdi
	movq $sReadingString, %rsi

	call fopen

	movq %rax, $filePointer

	testq $filePointer, $filePointer
	je .Lmain_invalid_file

	.Lmain_stdin:

	














.Lmain_invalid_file:
	movl $2, %eax

	movq $stderr, %rdi
	movq $sInvalidFileMsg, %rsi
	movq 1(%r13), %rdx

	call fprintf

	.Lmain_done


.Lmain_too_many_arg:
	movl $1, %eax


	movq $sTooManyArgsMsg, %rsi
	movq $stderr, %rdi

	call fprintf

	.Lmain_done




.Lmain_done:

	popq %r14
	popq %r13
	popq %r12
ret

/*
vim:ft=gas:
*/
