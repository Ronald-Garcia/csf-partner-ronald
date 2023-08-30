#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  
  // set the least significant bit to the value passed
  result.data[0] = val;

  // set all other bits to zero
  for(int i = 1; i < 8; i++) {
    result.data[i] = 0;
  }

  // return the new UInt256
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 8 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  for (int i = 0; i < 8; i ++){
    result.data[i] = data[i];
  }
  //return the new uint256.
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  //Find the strelen
  int string_length = strlen(hex);
  
  //If there is more than 64 characters, only look at the right most 64
  if (string_length > 64) {
    hex = &hex[string_length - 64];
    string_length = 64;
  }

  // get remaining characters (the "extra" block)
  int remaining_chars = string_length % 8;
  // get the current most significant bit (7 being most significant)
  int cur_index = string_length / 8;

  // set the more significant bits (if there are any) to 0
  for (int i = 8; i > cur_index; i--) {
    result.data[i] = 0;
  }

  // create temp string to save 8 block bits
  char temp_string[9];

  // if there is an unfilled bit,
  if (remaining_chars != 0) {
    // save all the characters
    for (int i = 0; i < remaining_chars; i++) {
      temp_string[i] = hex[i];
    }
    // terminate the string
    temp_string[remaining_chars] = '\0';
    // save the number in the most significant bit
    result.data[cur_index] = strtoul(temp_string, NULL, 16);
  }

  // move the hex ptr to the start of the next bit
  hex = &hex[remaining_chars];
  // move to the next most significant digit
  cur_index--;

  // for all indices
  while(cur_index >= 0) {
    // save the 8 character long bit
    for (int j = 0; j < 8; j++) {
      temp_string[j] = hex[j];
    }
    // terminate string
    temp_string[8] = '\0';
    // add it to the current bit and move to the next
    result.data[cur_index--] = strtoul(temp_string, NULL, 16);
    // move to the start of the next bit
    hex = &hex[8];
  }
  
  // return the result
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {

  //Malloc a sufficiently large buffer array to store 64 letters plus null terminator.
  char *hex = calloc(65, sizeof(char));
  int size_of_hex = 0;

  //Assumes each index can be represented in at most 8 letters.
  char buffer [9];

  //Loop through each element in val, adding to buffer, from most to least significant.
  for (int i = 7; i > 0; i--) {
    //Check to not write extra zeros in.
    if (val.data[i] != 0){
      sprintf(buffer, "%x", val.data[i]);
      for (int j = 0; j < 8; j++) {
        hex[size_of_hex] = buffer[j];
        size_of_hex++;
      }
    }
  }
  //At the end, need to write whatever is left to buffer, no matter what.
  int nums_copied = sprintf(buffer, "%x", val.data[0]);

  //copy the buffer to hex.
  for(int i = 0; i < nums_copied; i++){
    hex[size_of_hex + i] = buffer[i];
  }
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 8 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {

  // variable to return
  UInt256 sum;
  
  // flag to see whether or not we should carry over
  int flag = 0;

  // loop through each bit
  for(int i = 0; i < 8; i++) {

    // get the left bit
    uint32_t l = left.data[i];
    // get the right bit
    uint32_t r = right.data[i];
    // add them together (plus the carry from the previous bit)
    uint32_t s = l + r + flag;

    // save the sum
    sum.data[i] = s;

    // store the carry (if an overflow occured, a carry is to be added to the next bit)
    flag = (s < l) || (s < r);
  }

  // return the sum
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  // variable to return
  UInt256 result;
  // negate the right term
  UInt256 neg_right = uint256_negate(right);

  // add the left and the negative of the right
  result = uint256_add(left, neg_right);
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  UInt256 one = uint256_create_from_u32(1);

  for(int i = 0; i < 8; i++) {
    val.data[i] = ~val.data[i];
  }

  result = uint256_add(one, val);

  return result;
}


// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}
