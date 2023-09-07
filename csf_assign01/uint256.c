/*
 * Source file for uint256 functions
 * CSF Assignment 1
 * Ronald Garcia
 * Cameron Kolisko
 * rgarci47@jh.edu
 * ckolisk1@jh.edu
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

/** Function to create a uint256 from uint32_t value (least significant)
 * @param val the uint32_t value
 * @return the new uint256 number.
 */
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

/** Function to create a uint256 from an array of 8 uint32s
 * @param data the list of uint32_t's to save
 * @return the new uint256
 */
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  for (int i = 0; i < 8; i ++){
    result.data[i] = data[i];
  }
  //return the new uint256.
  return result;
}


/** Function to create a uint256 from a hex code
 * @param hex the hex value as a string
 * @return the new uint256
 */
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

/** Function to return the hex code associated with a uint256 value
 * @param val the uint256 value
 * @return the hex code as a string
 */
char *uint256_format_as_hex(UInt256 val) {

  //Malloc a sufficiently large buffer array to store 64 letters plus null terminator.
  char *hex = calloc(65, sizeof(char));
  int size_of_hex = 0;
  int first_ind = 7;
  //Assumes each index can be represented in at most 8 letters.
  char buffer [9];

  //Loop through each element in val, adding to buffer, from most to least significant.
  for (int i = 7; i >= 0; i--) {

    //First, check for the first index that is not just leading 0's
    if (val.data[i] == 0U && (i != 0)) { //Second bool for case of "all 0's"
      continue;
    }
    else { 
      first_ind = i;
      break; //Once we find index, we leave loop.
      }
  }
  //Now, we know we have at least some number at index i. This number is
  //copied over to hex from buffer WITHOUT leading 0's.
  int letters_copied = sprintf(buffer, "%x", val.data[first_ind]);
  for (int i = 0; i < letters_copied; i++) {
    hex[size_of_hex++] = buffer[i];
  }

  //Now we have written the most signifigant digit without leading 0's
  //The next step is to write the rest of the number, if any is left, WITH 
  //leading zeros.
  for (int i = first_ind - 1; i >= 0; i--){
    sprintf(buffer, "%08x", val.data[i]);

    //For loop to write the letters to hex.
    for (int j = 0; j < 8; j++){
      hex[size_of_hex++] = buffer[j];
    }
  }
  //Now, we have written all of the letters to the hex, and can return it.
  return hex;
}

/** Function to get a certain chunk of 32 bits (0 - 7) of a uint256
 * @param val the uint256 to get the chunk from
 * @param index which chunk to get
 * @return the uint32 value at that chunk
 */
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

/** Function to add two uint256 numbers
 * @param left first number to add
 * @param right second number to add
 * @return the sum of the two numbers
 * 
 */
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

/** Function to handle subtraction of two uint256 numbers
 * @param left the number to be subtracted from
 * @param right the number that is negated
 * @return the difference of left and right
 */
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  // variable to return
  UInt256 result;
  // negate the right term
  UInt256 neg_right = uint256_negate(right);

  // add the left and the negative of the right
  result = uint256_add(left, neg_right);
  return result;
}

/** Function to handle negation of a uint256 number
 * @param val the uint256 to negate
 * @return the negated number
 */
UInt256 uint256_negate(UInt256 val) {
  
  // the return variable
  UInt256 result;

  // the value of one
  UInt256 one = uint256_create_from_u32(1);

  // invert each bit
  for (int i = 0; i < 8; i++) {
    val.data[i] = ~val.data[i];
  }

  // add one to the inverted value
  result = uint256_add(one, val);

  // return the result
  return result;
}


/** Function to handle the shifting left operator for uint256
 * @param val the number to shift
 * @param nbits the number of bits to shift by
 * @return the resulting number after the shift
 */
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {

  // result to return
  UInt256 result;
  
  // modulate the bits to move
  nbits %= 256;

  // the amount of big bits (the members of the array val.data) to shift
  int shift_big_bits = nbits / 32;

  // for each index
  for (unsigned i = 0; i < 8; i++) {
    // shift the index by the number of big bits to shift (and modulate so it can wrap around from 0 => 7)
    result.data[(i - shift_big_bits) % 8] = val.data[i];
  }
  
  // the amount of bits to shift (in each member of val.data)
  int shift_small_bits = nbits % 32;

  // for each number
  for (int j = 7; j >= 0; j--) {
    // get the bit(s) that are to be moved to the next member of val.data
    uint32_t sig_bit = ((val.data[j])) >> (32 - shift_small_bits);

    // get the index of the previous member
    int ind = (j + 1) % 8; 

    // shift the previous member and add the bits to be shifted into it
    result.data[ind] = (val.data[ind] << shift_small_bits) + sig_bit;
  }
  
  // return the result
  return result;
}


/** Function to handle the shifting right operator for uint256
 * @param val the number to shift
 * @param nbits the number of bits to shift by
 * @return the resulting number after the shift
 */

UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  // result to return
  UInt256 result;
  
  // modulate the bits to move
  nbits %= 256;

  // the amount of big bits (the members of the array val.data) to shift
  int shift_big_bits = nbits / 32;

  // for each index
  for (unsigned i = 0; i < 8; i++) {
    // shift the index by the number of big bits to shift (and modulate so it can wrap around from 7 => 0)
    result.data[(i + shift_big_bits) % 8] = val.data[i];
  }

  // the amount of bits to shift (in each member of val.data)
  int shift_small_bits = nbits % 32;

  // for each number
  for (unsigned j = 0; j < 8; j++) {
    // get the bit(s) that are to be moved to the next member of val.data
    uint32_t sig_bit = val.data[j] & ~(~0U << shift_small_bits);

    // get the index of the previous member
    int ind = (j - 1) % 8; 

    // shift the previous member and add the bits to be shifted into it
    result.data[ind] = (val.data[ind] >> shift_small_bits) + (sig_bit << (32 - shift_small_bits));
  }
  return result;
}