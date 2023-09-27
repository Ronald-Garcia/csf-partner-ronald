/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)

	.section .rodata
/* Define any string constants or read-only data here */

	.section .text

/*
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */
	.globl wc_hash
wc_hash:
	
	sub $8, %rsp /*Align the stack pointer.*/

	mov %rdi, %r10 /*Store string r10*/
	mov $5381, %rax /* Storing hash code */
	mov $0, %r9 /*Just stores 0*/

	.Lwc_hash_loop:

		/*Store current letter into r11*/
		movq (%r10), %r11

		/* Check if %r11 == '\0' */
		test %r11, %r11
		je .Lwc_hash_done
		
		leaq (%r9, %rax, 4), %rax
		leaq (%rax, %rax, 8), %rax
		addq %r11, %rax

		/*Otherwise, increment pointers and restart loop.*/
		inc %r10
		
		jmp .Lwc_hash_loop

		
	.Lwc_hash_done:
	/*Realign stack pointer and return*/
		add $8, %rsp
		leave
		ret

/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
	.globl wc_str_compare
wc_str_compare:
	/* Notes on registers used:
	* Caller saved registers: %r10 %r11
	* Subroutine arguments: %rdi, %rsi
	* Return register: %rax
	*
	*/
	/*Align the stack pointer.*/
	sub $8, %rsp
	
	.Lstr_compare_loop:
		/*Store first letter of left into r10*/
		mov (%rdi), %r10
		/*Store first letter of right into r11*/
		mov (%rsi), %r11

		/*Check if the two characters are the same*/
		cmp %r10, %r11
		jne .Lstr_compare_done

		/* Check if %r10 == '\0' */
		test %r10, %r10
		je .Lstr_compare_done
		/*Otherwise, increment pointers and restart loop.*/
		inc %rdi
		inc %rsi
		jmp .Lstr_compare_loop

		
	.Lstr_compare_done:
	/*Realign stack pointer and return*/
		sub %r11, %r10
		mov %r10, %rax

		add $8, %rsp
		leave
		ret

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	/* Notes on registers used:
	* Caller saved registers: %r10
	* Subroutine arguments: %rdi, %rsi
	* Return register: none, but store string in %rdi
	*/

	/*Allign the stack pointer*/
	sub $8, %rsp
	
	/*Enter loop, leave when at end of sorce string*/
	.Lstr_copy_loop:
	/*Store first letter of source into r10*/
	mov (%rsi), %r10
	/*Then, check if that letter was 0.*/
	test %r10, %r10
	/*If it was, return. Otherwise, keep increment pointer, store r10 in rdi,
	keep going.*/
	je .Lstr_copy_done
	mov %r10, %rdi
	inc %rsi
	inc %rdi
	jmp .Lstr_copy_loop

	/*Get here when function is done.*/
	.Lstr_copy_done:
	/*Null terminate rdi*/
	mov %r10, %rdi
	/*Realign stack pointer*/
	add $8, %rsp
	ret

/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 * C function prototype:
 *    int wc_isspace(unsigned char c);
 */
	.globl wc_isspace
wc_isspace:
	/*Align the stack pointer.*/
	subq $8, %rsp

	/* if argument is equal to: */
	cmpq $9, %rdi  /* tab */
	je .Lisspace 
	cmpq $10, %rdi  /* new line */
	je .Lisspace 
	cmpq $11, %rdi  /* vertical tab */
	je .Lisspace 
	cmpq $12, %rdi  /* form feed */
	je .Lisspace 
	cmpq $13, %rdi  /* carriage return */
	je .Lisspace 
	cmpq $32, %rdi  /* space */
	je .Lisspace 

	/* if its not a whitespace character, put 0 into the return register */
	movq $0, %rax
	jmp .Lisspace_done

.Lisspace:
	movq $1, %rax /* put 1 into the return register */
	jmp .Lisspace_done

.Lisspace_done:
	/*realign stack pointer*/
	addq $8, %rsp
	ret

/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
	.globl wc_isalpha
wc_isalpha:
	/*Align the stack pointer*/
	subq $8, %rsp
	/*Enter comparisions*/
	cmpq $65, %rdi 
	jb .Lnotalpha /* If below 'A', not a letter */
	cmpq $90, %rdi 
	jbe .Lalpha /* If below or equal to 'Z' (and above 'A'), then is a letter */
	cmpq $97, %rdi
	jb .Lnotalpha /* If below 'a' and not in 'A' - 'Z', not a letter */
	cmpq $122, %rdi
	jbe .Lalpha /* If above or equal to 'a' and below or equal to 'z', is a letter */
	jmp .Lnotalpha /* If above 122, not a letter */

.Lnotalpha:
	movq $0, %rax /* Put 0 in return register */
	jmp .Lisalpha_done
.Lalpha:
	movq $1, %rax /* Put 1 in return register */
	jmp .Lisalpha_done

.Lisalpha_done:
	/*realign stack pointer*/
	addq $8, %rsp
	ret

/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */
	.globl wc_readnext
wc_readnext:
	/* TODO: implement */
	ret

/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
wc_tolower:
	/* TODO: implement */
	ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
	/* TODO: implement */
	ret

/*
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
	.globl wc_find_or_insert
wc_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Find or insert the WordEntry object for the given string (s), returning
 * a pointer to it. The head of the linked list which contains (or should
 * contain) the entry for s is the element of buckets whose index is the
 * hash code of s mod num_buckets.
 *
 * Returns a pointer to the WordEntry object in the appropriate linked list
 * which represents s.
 *
 * C function prototype:
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
