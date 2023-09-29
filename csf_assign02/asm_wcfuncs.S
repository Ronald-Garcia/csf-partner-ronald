/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_SIZE          (80)
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
	subq $8, %rsp
	
	/*Enter loop, leave when at end of sorce string*/
	.Lstr_copy_loop:
	/*Store first letter of source into r10*/
	movb (%rsi), %r10b
	/*Then, check if that letter was 0.*/
	test %r10b, %r10b
	/*If it was, return. Otherwise, keep increment pointer, store r10 in rdi,
	keep going.*/
	je .Lstr_copy_done
	movb %r10b, (%rdi)
	incq %rsi
	incq %rdi
	jmp .Lstr_copy_loop

	/*Get here when function is done.*/
	.Lstr_copy_done:
	/*Null terminate rdi*/
	movb %r10b, (%rdi)
	/*Realign stack pointer*/
	addq $8, %rsp
	leave
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
	cmpb $65, %dil 
	jb .Lnotalpha /* If below 'A', not a letter */
	cmpb $90, %dil 
	jbe .Lalpha /* If below or equal to 'Z' (and above 'A'), then is a letter */
	cmpb $97, %dil
	jb .Lnotalpha /* If below 'a' and not in 'A' - 'Z', not a letter */
	cmpb $122, %dil
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
    /* Notes on registers used:
	* Callee saved registers: %r12, %r13
	* Caller saved registers: %r10
	* Subroutine arguments: %rdi
	* Return register: none, but store string in %rdi
	*/
	/*Allign the stack pointer*/
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	movq %rdi, %r12 /* Move argument file pointer into register */
	movq %rsi, %r13 /* Move argument string into register */
	movq $0, %r14   /* Set counter to 0 */
	.Lwc_readnext_remove_leading_whitespace:
		
		/* get next char */
		movq %r12, %rdi
		call fgetc

		movq %rax, %r15
		/* check if EOF or is space */
		cmpq $-1, %r15
		jne .Lwc_readnext_leave_whitespace_loop
		movq %r15, %rdi
		call wc_isspace
		cmpq $1, %rax  
		jne .Lwc_readnext_leave_whitespace_loop

		/* if both, continue loop */
		jmp .Lwc_readnext_remove_leading_whitespace


	.Lwc_readnext_leave_whitespace_loop:
		cmpq $-1, %r15
		je .Lwc_readnext_unsuccessful
	
	.Lwc_readnext_loop:
		/* check if we reached max word length */
		cmpq $MAX_WORDLEN, %r14
		/* exit if not */
		jae .Lwc_readnext_successful


		/* move letter into the word */
		movq %r15, (%r13)
		incq %r14
		incq %r13

		/* get next char from r12 */
		movq %r12, %rdi
		call fgetc
		movq %rax, %r15

		/* if letter is EOF, get out */
		cmpq $-1, %r15
		je .Lwc_readnext_successful

		/* check if whitespace */
		movq %r15, %rdi
		call wc_isspace

		/* if whitespace, get out */
		test %rax, %rax
		jne .Lwc_readnext_successful

	.Lwc_readnext_successful:
		movq $0, (%r13)
		jmp .Lwc_readnext_done


	.Lwc_readnext_unsuccessful:
		movq $0, %rax
		jmp .Lwc_readnext_done

	.Lwc_readnext_done:
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		leave
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
	/* Notes on registers used:
	* Callee saved registers: %r12, %r13
	* Subroutine arguments: %rdi
	* Return register: none, but store string in %rdi
	*/

	/*Allign the stack pointer*/
	pushq %r12
	pushq %r13
	
	movb %dil, %r12b /* Move argument string into register */

	/*Enter loop, leave when at end of the string*/

	.Lwc_tolower_loop:
		/*Store first letter of the string into r10*/
		movb %r12b, %r13b /*Can't () deref with these*/
		/*Then, check if that letter was 0.*/
		test %r13b, %r13b
		je .Lwc_tolower_done

		/*If it was, return. Otherwise, check if the character is a letter */
		movb %r13b, %dil
		
		call wc_isalpha /* check if it is a letter */

		/* check if isalpha returned 0 */
		test %rax, %rax
		
		je .Lwc_tolower_arithmetic_skip

		/* if here, it is a letter. Check if upper */

		cmpb $97, %r13b
		/* skip arithmetic calculation if it is already lower */
		jae .Lwc_tolower_arithmetic_skip 

		/* if here, it is an upper case letter. change letter by adding 32 */
		addb $32, %r12b /*Can't () deref with these*/

		.Lwc_tolower_arithmetic_skip:
		/* move to next letter */
		incb %r12b

		jmp .Lwc_tolower_loop

	/*Get here when function is done.*/
	.Lwc_tolower_done:
		/*Realign stack pointer*/
		popq %r13
		popq %r12
		leave
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
   /* Notes on registers used:
	* Callee saved registers: %r12
	* Caller saved registers: %r10
	* Subroutine arguments: %rdi
	* Return register: none, but store string in %rdi
	*/

	/*Allign the stack pointer*/
	pushq %r12
	
	movq %rdi, %r12 /* Move argument string into register */
	/* r12 will be used as the end pointer */

	movb (%r12), %r10b /* save character */

	/* check if the input string is just null */
	test %r10b, %r10b
	je .Lwc_trim_non_alpha_done /* if it is, just end method */

	.Lwc_trim_non_alpha_get_end:

		movb (%r12), %r10b /* save character */

		/* check if r10 is '\0' */
		test %r10b, %r10b 
		/* if yes, go to loop */
		je .Lwc_trim_non_alpha_loop

		/* if not, keep going */
		incq %r12
		jmp .Lwc_trim_non_alpha_get_end

	.Lwc_trim_non_alpha_loop:
		
		decq %r12
		movb (%r12), %r10b

		/*If it was, return. Otherwise, check if the character is a letter */
		movb %r10b, %dil
		
		call wc_isalpha /* check if it is a letter */

		/* check if isalpha returned 0 */
		test %rax, %rax
		
		/* if it is a letter, we are done */
		jne .Lwc_trim_non_alpha_null_insert

		/* if it is not a letter, keep going */
		jmp .Lwc_trim_non_alpha_loop

	.Lwc_trim_non_alpha_null_insert:
		incq %r12
		movb $0, (%r12)

	/*Get here when function is done.*/
	.Lwc_trim_non_alpha_done:
		/*Realign stack pointer*/
		popq %r12
		leave
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
   /* Notes on registers used:
	* Callee saved registers: %r12, %r13
	* Caller saved registers: %r10
	* Subroutine arguments: %rdi
	* Return register: none, but store string in %rdi
	*/
	/*Allign the stack pointer*/
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	movq %rdi, %r12 /* Move argument wordentry into register */
	movq %rsi, %r13 /* Move argument string into register */
	movq %rdx, %r14 /* Move argument integer pointer into register */
	movq %rdi, %r15 /* Move argument wordentry into register */


	/* r12 will be used as the head wordentry */

	.Lwc_find_or_insert_loop:

		/* check if "cur == null" */
		test %r12, %r12
		je .Lwc_find_or_insert_prepend /* if end of list is reached, prepend list */

		movq %r13, %rdi /* add the two strings as parameters for string compare */
		addq WORDENTRY_WORD_OFFSET, %r12 /*Just done to get ref to string*/
		movq %r12, %rsi
		subq WORDENTRY_WORD_OFFSET, %r12

		call wc_str_compare /* call string compare */

		test %rax, %rax /*check if they are the same */

		je .Lwc_find_or_insert_match_found /* if match, go to match_found area */

		/* otherwise, move to the pointer to the next word entry pointer */
		addq WORDENTRY_NEXT_OFFSET, %r12
		/* save the word entry pointer */
		movq (%r12), %r12 

		jmp .Lwc_find_or_insert_loop

	.Lwc_find_or_insert_prepend:

		/* malloc a new word entry */
		movq $WORDENTRY_SIZE, %rdi

		call malloc /*Pointer to allocated space in rax*/

		/* make *inserted = 1 */
		movq $1, (%r14)

		/*r8 will act as a local variable for counting this loop*/
		movq $0, %r8
		/* Move string into rax->word */
		.Lwc_find_or_insert_prepend_loop:
			/*Store a letter*/
			movb (%r13), %r9b
			movb %r9b, (%rax)
			/*If null, leave*/
			test %r9b, %r9b
			je .Lwc_find_or_insert_prepend_loop
			/*If not null, increment rax pointer, maintain count*/
			incq %rax
			incq %r8
		/*Now that word is stored, reset rax pointer to start of struct*/
		subq %r8, %rax

		/* Move 0 into rax->count */
		addq $WORDENTRY_COUNT_OFFSET, %rax
		movl $0, (%rax)

		/* Move r15 into rax->next */
		addq $8, %rax
		movq %r15, (%rax)
		/*Reset rax pointer to look at start of word entry again.*/
		subq $WORDENTRY_NEXT_OFFSET, %rax

		jmp .Lwc_find_or_insert_done

	.Lwc_find_or_insert_match_found:
		/* make *inserted = 0 */
		movq $0, (%r14)
		movq %r15, %rax /*R15 at this location is just ref to start struct*/

	.Lwc_find_or_insert_done:
		pushq %r15
		pushq %r14
		pushq %r13
		pushq %r12
		leave
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
	/* Notes on registers used:
	* Callee saved registers: %r12, %r13, %r14, %r15
	* Caller saved registers: %r10
	* Subroutine arguments: %rdi, %rsi, %rdx
	* Return register: none, but store string in %rdi
	*/
	/*Allign the stack pointer*/
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	/* storing old parameters */
	movq %rdi, %r12 /* storing buckets */
	movq %rsi, %r13 /* storing num_buckets */
	movq %rdx, %r14 /* storing string */

	/* getting the hash of r14 */
	movq %r14, %rdi
	call wc_hash

	/* removing bits from rdx to divide */
	movq $0, %rdx

	/* divide hash in rax by num_buckets to get remainder in rdx */
	divq %r13

	/* hash is in r15 */
	movq %rdx, %r15

	/* moving buckets[hash] into first param */
	addq %r15, %r12
	movq (%r12), %rdi
	
	/* moving string into second param */
	movq %r14, %rsi

	/* moving 0 into third param (inserted) */
	movq $0, %rdx

	call wc_find_or_insert /* find or insert a word entry */

	/* check if inserted a new node */
	test %rdx, %rdx

	/* if a node has not been inserted, skip insertion */
	jne .Lwc_dict_find_or_insert_done

	/* if a node has been inserted, add it to buckets[hash] */
	movq %rax, (%r12)

	.Lwc_dict_find_or_insert_done:
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		leave
		ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	
	/*Need callee saved registers, since call malloc*/
	/*Also alligns stack pointer*/
	pushq %r12
	pushq %r13

	movq %rdi, %r12 /* lagging pointer */
	movq %rdi, %r13 /* leading pointer */
	/* r11 = cur */
	/* r10 = p */
	

	.Lwc_free_chain_loop:
		test %r13, %r13 /* check if null */
		je .Lwc_free_chain_done /* if null, we are done */
	
		/* move reference to next into r13 (cur = cur->next) */
		movq WORDENTRY_NEXT_OFFSET(%r13), %r13

		/* set up free for cur node (free p) */
		movq %r12, %rdi
		/*Set r12 to null*/
		movq $0, %r12

		/*Free thing refed by rdi*/
		call free
		/*Set rdi to null*/
		movq $0, %rdi
		
		/* move to next node (p = cur) */
		movq %r13, %r12

		/* loop again */
		jmp .Lwc_free_chain_loop

	.Lwc_free_chain_done:
		popq %r13
		popq %r12
		leave
		ret

/*
vim:ft=gas:
*/
