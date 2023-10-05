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
	* Callee saved registers: %r12, %r13, %r14
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
	
	ja .Lmain_too_many_arg /*Too many args, leave program*/
	movq $stdin, %r10 /*TODO: HOW DOES I/O WORK*/
	movq %r10, filePointer /*Prep default file pointer.*/

	cmpl $1, %edi /*check if argc == 1. If yes, use standard input*/

	je .Lmain_stdin

	/*Otherwise, we must open file given as second arg in %rsi.*/
	movq %rsi, %rdi
	movq $sReadingString, %rsi

	call fopen

	movq %rax, filePointer
	movq %rax, %r10
	testq %r10, %r10
	je .Lmain_invalid_file

	.Lmain_stdin:

	








.Lmain_invalid_file:
	movl $2, %eax

	/*TODO: PRINT TO STDERR*/
	/*movq $stderr, %rdi*/
	movq $sInvalidFileMsg, %rdi
	movq %r13, %rdx

	call printf

	jmp .Lmain_done


.Lmain_too_many_arg: /*Area for when there are too many args in rdi*/
	movl $1, %eax

	/*TODO: make it print to stderr*/
	movq $sTooManyArgsMsg, %rdi
	/*movq $stderr, %rdi*/

	call printf

	jmp .Lmain_done


.Lmain_done:

	popq %r14
	popq %r13
	popq %r12
ret

/*
vim:ft=gas:
*/
