/*
 * Assembly language main function implementation
 */

#define MAX_WORDLEN 63


	.section .rodata

	sTooManyArgsMsg: .string "Error: too many arguments."
	


	.section .text

	.globl main
main:
   /* Notes on registers used:
	* Callee saved registers: %r12, %r13
	* Caller saved registers: %r10
	* Subroutine arguments: %rdi
	* Return register: none, but store string in %rdi
	*/

	cmpl $2, %edi /* check if argc > 2 */
	
	ja .Lmain_too_many_arg
	





.Lmain_too_many_arg:
	movl $1, %eax


	movq $sTooManyArgsMsg, %rsi
	movq $stderr, %rdi

	call fprintf

	.Lmain_done




.Lmain_done:


ret

/*
vim:ft=gas:
*/
