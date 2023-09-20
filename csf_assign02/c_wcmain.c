#include <stdio.h>
#include <stdint.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

// TODO: prototypes for helper functions

int main(int argc, char **argv) {

  if (argc > 2) {
    fprintf(stderr, "Error: too many arguments.");
    return 1;
  }

  FILE* fp = stdin;

  if (argc == 2) {
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
      fprintf(stderr, "Error: could not open file \"%s\" for reading.", argv[1]);
      return 2;
    }
  }


  struct WordEntry* buckets[HASHTABLE_SIZE] = {0};

  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  unsigned char best_word[MAX_WORDLEN]; 
  unsigned char buffer[MAX_WORDLEN];

  best_word[0] = '\0';
  
  uint32_t best_word_count = 0;

  while (wc_readnext(fp, buffer)) {

    if (buffer[0] == '\0') continue; // if reading a new line, move to next word

    total_words++;
    wc_tolower(buffer);

    wc_trim_non_alpha(buffer);

    struct WordEntry* current_word_entry = wc_dict_find_or_insert(buckets, HASHTABLE_SIZE, buffer);

    if (current_word_entry->count == 0) { // if this is the first iteration of this word, add to unique_words
      unique_words++;
    }

    current_word_entry->count++;
  }

  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    
    if (buckets[i] != NULL) {
      struct WordEntry* cur_word = buckets[i];

      while (cur_word != NULL) {
        if (cur_word->count > best_word_count) {
          wc_str_copy(best_word, cur_word->word);
          best_word_count = cur_word->count;
        } else if ((cur_word->count == best_word_count) && (wc_str_compare(cur_word->word, best_word) == -1)) {
          wc_str_copy(best_word, cur_word->word);
        }

        cur_word = cur_word->next;
      }
    }
  }

  printf("Total words read: %u\n", (unsigned int) total_words);
  printf("Unique words read: %u\n", (unsigned int) unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

  // TODO: make sure file is closed (if one was opened)
  // TODO: make sure memory is freed

  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    wc_free_chain(buckets[i]);
  }

  if (fp != stdin) {
    fclose(fp);
    fp = NULL;
  }

  return 0;
}

// TODO: definitions of helper functions
