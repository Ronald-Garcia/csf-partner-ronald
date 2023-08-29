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
  if (string_length > 64){
    hex = &hex[string_length - 64];
    string_length = 64;
  }
  int i;
  //make a string from increments of the hex string, then convert and store as ul.
  for (i = 1; i * 8 < string_length; i++) {
    //for loop for conversion of string.
    char temp_string[9];
    for (int j = 0; j < 8; j++) {
      temp_string[j] = hex[string_length - (i * 8) + j];
    }
    //add null terminator to end.
    temp_string[8] = '\0';
    //convert and store.
    result.data[i-1] = strtoul(temp_string, NULL, 16);
  }
  //implement special case for incomplete
  //remaining characters = string_length - (i - 1) * 8
  int remaining_chars = string_length - ((i - 1) * 8);
  char temp_string[remaining_chars + 1];
  for (int k = 0; k < remaining_chars; k++){
    temp_string[k] = hex[k];
  }
  //Add null terminator to smaller string, convert it to number and store.
  temp_string[remaining_chars] = '\0';
  result.data[i - 1] = strtoul(temp_string, NULL, 16);
  //Set everything else to 0.
  for (int k = i; k < 8; k++){
    result.data[k] = 0;
  }
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = NULL;
  // TODO: implement
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
  UInt256 sum;
  // TODO: implement
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  // TODO: implement
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
