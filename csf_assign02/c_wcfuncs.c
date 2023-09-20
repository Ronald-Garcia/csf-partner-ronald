// Important: do not add any additional #include directives!
// The only C library functions that may be used are
//
// - fgetc
// - malloc
// - free

#include <stdlib.h>
#include "wcfuncs.h"

// Compute a hash code for the given NUL-terminated
// character string.
//
// The hash algorithm should be implemented as follows:
//
// uint32_t hash_code = 5381
// for each character c of w in order {
//   hash_code = hash_code * 33 + c
// }
//
// Note that the character values should be treated as
// being unsigned (in the range 0..255)
uint32_t wc_hash(const unsigned char *w) {
  
  uint32_t hash_code = 5381;

  for (int i = 0; w[i] != '\0'; i++) {
    hash_code = hash_code * 33 + w[i];
  }

  return hash_code;

}

// Compare two strings lexicographically. Return
//
// - a negative value if lhs string is less than rhs string
// - 0 if lhs string is identical to rhs string
// - a positive value if lhs string is greater than rhs string
//
// Lexicographical comparison is a generalization of alphabetical
// order, but using character codes. If one string is a prefix
// of the other, it is considered as "less than". E.g.,
// "hi" would compare as less than "high".
int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs) {

  int res = 0;
  if (lhs[0] == '\0' && rhs[0] == '\0') { //If both are just null terminator.
    return 0;
  }
  if (lhs[0] == '\0') {//If only lhs is null terminator
    return -1;
  }
  if (rhs[0] == '\0') {//If only rhs is null terminator
    return 1;
  }
  for (int i = 0; lhs[i] != '\0' && rhs[i] != '\0'; i++) {

    if (lhs[i + 1] == '\0' && rhs[i + 1] != '\0') return -1;
    if (lhs[i + 1] != '\0' && rhs[i + 1] == '\0') return 1;
    
    if (lhs[i] < rhs[i]) return -1; // If not same character return.
    else if (lhs[i] > rhs[i]) return 1;
    //Otherwise, go through loop again.
  }



  return res; //Only happens if both are the same word.
}

// Copy NUL-terminated source string to the destination buffer.
void wc_str_copy(unsigned char *dest, const unsigned char *source) {
  int i;
  for (i = 0; source[i] != '\0'; i++) {
    dest[i] = source[i];
  }

  dest[i] = source[i];
}

// Return 1 if the character code in c is a whitespace character,
// false otherwise.
//
// For the purposes of this function, a whitespace character is one of
//
//   ' '
//   '\t'
//   '\r'
//   '\n'
//   '\f'
//   '\v'
int wc_isspace(unsigned char c) {
  return c == ' ' ||
          c == '\t' ||
          c == '\r' ||
          c == '\n' ||
          c == '\f' ||
          c == '\v';
}

// Return 1 if the character code in c is an alphabetic character
// ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
int wc_isalpha(unsigned char c) {
  return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

// Read the next word from given input stream, storing
// the word in the array pointed-to by w. (This array should be
// assumed to be MAX_WORDLEN+1 elements in size.) Return 1
// if a word is read successfully, 0 otherwise.
//
// For the purposes of this function, a word is a sequence of
// 1 or more non-whitespace characters.
//
// If a sequence of non-whitespace characters has more than
// MAX_WORDLEN characters, then only the first MAX_WORDLEN
// characters in the sequence should be stored in the array.
int wc_readnext(FILE *in, unsigned char *w) {

  int i = 0;
  while (i < MAX_WORDLEN) {
    char cur_char = fgetc(in);

    if (cur_char == EOF && i == 0) { // if end of file or error, complete current word and return 0
      return 0;
    }

    if (wc_isspace(cur_char) || cur_char == EOF) { // if a whitespace character is reached, stop reading.
      break;
    }

    w[i] = cur_char; // otherwise, append character to the word and keep reading
    i++;
  }

  // null terminate word
  w[i] = '\0';

  return 1; // if here, word was successfully read.

}

// Convert the NUL-terminated character string in the array
// pointed-to by w so that every letter is lower-case.
void wc_tolower(unsigned char *w) {
  for (int i = 0; w[i] != '\0'; i++) {
    if (wc_isalpha(w[i]) && w[i] < 97) {
      w[i] += 32;
    }
  }
}

// Remove any non-alphaabetic characters from the end of the
// NUL-terminated character string pointed-to by w.
void wc_trim_non_alpha(unsigned char *w) {
  
  int end_index = 0;

  // goes until end_index is at the null terminator.
  while(w[end_index] != '\0') { 
    end_index++;
  }

  int new_end_index = end_index - 1; //character before null terminator

  // starting at the index of the character before the null terminator, go back until there is an alphabetic character
  while(!wc_isalpha(w[new_end_index])) {
    new_end_index--;
  }

  // set the end after the alphabetic character
  w[new_end_index + 1] = '\0';
}

// Search the specified linked list of WordEntry objects for an object
// containing the specified string.
//
// If a matching object is found, set the int variable pointed-to by
// inserted to 0 and return a pointer to the matching object.
//
// If a matching object is not found, allocate a new WordEntry object,
// set its next pointer to point to what head points to (i.e., so the
// new object is at the head of the list), set the variable pointed-to
// by inserted to 1, and return a pointer to the new node. Note that
// the new node should have its count value set to 0. (It is the caller's
// job to update the count.)
struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted) {
  struct WordEntry* cur = head;

  while (cur != NULL) {
    // if the words match, set inserted to 0 and return the pointer of the matching node
    if (wc_str_compare(s, cur->word) == 0) {
      *inserted = 0;
      return cur;
    }

    cur = cur->next;
  }

  // otherwise, prepend the list
  struct WordEntry *new_head = (struct WordEntry*) malloc(sizeof(struct WordEntry));

  *inserted = 1; // set inserted to 1
  new_head->next = head; // prepend list
  new_head->count = 0; // set count to 0
  wc_str_copy(new_head->word, s);

  return new_head; // return new head
}

// Find or insert the WordEntry object for the given string (s), returning
// a pointer to it. The head of the linked list which contains (or should
// contain) the entry for s is the element of buckets whose index is the
// hash code of s mod num_buckets.
//
// Returns a pointer to the WordEntry object in the appropriate linked list
// which represents s.
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s) {

  uint32_t s_hash = wc_hash(s) % num_buckets;

  int inserted_node = 0;

  struct WordEntry* new_node = wc_find_or_insert(buckets[s_hash], s, &inserted_node);

  if (inserted_node) {
    buckets[s_hash] = new_node;
  }

  return buckets[s_hash];
}

// Free all of the nodes in given linked list of WordEntry objects.
void wc_free_chain(struct WordEntry *p) {

  struct WordEntry* cur = p;
  
  while (cur != NULL) {
    cur = cur->next;
    free(p);
    p = cur;
  } 


}
