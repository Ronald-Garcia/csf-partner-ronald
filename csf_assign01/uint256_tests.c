/*
 * Unit Testing for uint256 functions
 * CSF Assignment 1
 * Ronald Garcia
 * Cameron Kolisko
 * rgarci47@jh.edu
 * ckolisk1@jh.edu
 */


#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
  UInt256 rot; // value used to test rotations
} TestObjs;

// Helper functions for implementing tests
void set_all(UInt256 *val, uint32_t wordval);

#define ASSERT_SAME(expected, actual) \
do { \
  ASSERT(expected.data[0] == actual.data[0]); \
  ASSERT(expected.data[1] == actual.data[1]); \
  ASSERT(expected.data[2] == actual.data[2]); \
  ASSERT(expected.data[3] == actual.data[3]); \
  ASSERT(expected.data[4] == actual.data[4]); \
  ASSERT(expected.data[5] == actual.data[5]); \
  ASSERT(expected.data[6] == actual.data[6]); \
  ASSERT(expected.data[7] == actual.data[7]); \
} while (0)

#define INIT_FROM_ARR(val, arr) \
do { \
  for (unsigned i = 0; i < 8; ++i) \
    val.data[i] = arr[i]; \
} while (0)

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Declarations of test functions
void test_get_bits(TestObjs *objs);
void test_create_from_u32(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_rotate_left(TestObjs *objs);
void test_rotate_right(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_get_bits);
  TEST(test_create_from_u32);
  TEST(test_create);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_negate);
  TEST(test_rotate_left);
  TEST(test_rotate_right);

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all(UInt256 *val, uint32_t wordval) {
  for (unsigned i = 0; i < 8; ++i) {
    val->data[i] = wordval;
  }
}

TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

  // initialize several UInt256 values "manually"
  set_all(&objs->zero, 0);
  set_all(&objs->one, 0);
  objs->one.data[0] = 1U;
  set_all(&objs->max, 0xFFFFFFFFU);

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR(objs->msb_set, msb_set_data);

  // value with nonzero values in least significant and most significant words
  // Note that the hex representation of this value is
  //   CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB
  uint32_t rot_data[8] = { 0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U };
  INIT_FROM_ARR(objs->rot, rot_data);

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_get_bits(TestObjs *objs) {
  ASSERT(0U == uint256_get_bits(objs->zero, 0));
  ASSERT(0U == uint256_get_bits(objs->zero, 1));
  ASSERT(0U == uint256_get_bits(objs->zero, 2));
  ASSERT(0U == uint256_get_bits(objs->zero, 3));
  ASSERT(0U == uint256_get_bits(objs->zero, 4));
  ASSERT(0U == uint256_get_bits(objs->zero, 5));
  ASSERT(0U == uint256_get_bits(objs->zero, 6));
  ASSERT(0U == uint256_get_bits(objs->zero, 7));

  ASSERT(1U == uint256_get_bits(objs->one, 0));
  ASSERT(0U == uint256_get_bits(objs->one, 1));
  ASSERT(0U == uint256_get_bits(objs->one, 2));
  ASSERT(0U == uint256_get_bits(objs->one, 3));
  ASSERT(0U == uint256_get_bits(objs->one, 4));
  ASSERT(0U == uint256_get_bits(objs->one, 5));
  ASSERT(0U == uint256_get_bits(objs->one, 6));
  ASSERT(0U == uint256_get_bits(objs->one, 7));

  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 0));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 1));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 2));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 3));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 4));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 5));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 6));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 7));
}

void test_create_from_u32(TestObjs *objs) {
  UInt256 zero = uint256_create_from_u32(0U);
  UInt256 one = uint256_create_from_u32(1U);

  ASSERT_SAME(objs->zero, zero);
  ASSERT_SAME(objs->one, one);
}

void test_create(TestObjs *objs) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  
  UInt256 val1 = uint256_create(data1);
  ASSERT(1U == val1.data[0]);
  ASSERT(2U == val1.data[1]);
  ASSERT(3U == val1.data[2]);
  ASSERT(4U == val1.data[3]);
  ASSERT(5U == val1.data[4]);
  ASSERT(6U == val1.data[5]);
  ASSERT(7U == val1.data[6]);
  ASSERT(8U == val1.data[7]);
}

void test_create_from_hex(TestObjs *objs) {
  UInt256 zero = uint256_create_from_hex("0");
  ASSERT_SAME(objs->zero, zero);

  UInt256 one = uint256_create_from_hex("1");
  ASSERT_SAME(objs->one, one);

  UInt256 max = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, max);

  UInt256 two_and_a_half_bits = uint256_create_from_hex("a5b5c6c6ca5b5c6c6c12");
  uint32_t dat[8] = {1550609426,3334916699,42421,0,0,0,0,0};
  UInt256 test_two_half = uint256_create(dat);
  ASSERT_SAME(test_two_half, two_and_a_half_bits);
}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = uint256_format_as_hex(objs->zero);
  ASSERT(0 == strcmp("0", s));
  free(s);

  s = uint256_format_as_hex(objs->one);
  ASSERT(0 == strcmp("1", s));
  free(s);

  s = uint256_format_as_hex(objs->max);
  ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
  free(s);

  //Test for leading 0 problem.
  UInt256 pap = uint256_create_from_hex("6e1dfbd00000264803aceda8f2d2b43177c72f6e20e9e6cd32a54027633b932");
  s = uint256_format_as_hex(pap);
  ASSERT(0 == strcmp("6e1dfbd00000264803aceda8f2d2b43177c72f6e20e9e6cd32a54027633b932", s));
  free(s);
}

void test_add(TestObjs *objs) {
  UInt256 result;

  result = uint256_add(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->zero, objs->one);
  ASSERT_SAME(objs->one, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_add(objs->one, objs->one);
  ASSERT_SAME(two, result);

  result = uint256_add(objs->max, objs->one);
  ASSERT_SAME(objs->zero, result);

  UInt256 left = uint256_create_from_hex("6e1dfbdee110264803aceda8f2d2b43177c72f6e20e9e6cd32a54027633b932");
  UInt256 right = uint256_create_from_hex("ae1ce1fdd45187f7c206a17d74106e06d9e43a67999dc6c70d215aa79eef9e2");
  UInt256 expected = uint256_create_from_hex("11c3adddcb561ae3fc5b38f2666e3223851ab69d5ba87ad943fc69acf022b314");

  ASSERT_SAME(expected, uint256_add(left, right));
}

void test_sub(TestObjs *objs) {
  UInt256 result;

  result = uint256_sub(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->one, objs->one);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->zero, objs->one);
  ASSERT_SAME(objs->max, result);

  UInt256 left = uint256_create_from_hex("733cf23e74b0c561ec370300b95b873cf6cb7574f3e029249e999bfaeda3790");
  UInt256 right = uint256_create_from_hex("1247d82a3cac5b8762c9d265ecc22f3354e50c7930e8bd4605d61277a01496e");
  UInt256 expected = uint256_create_from_hex("60f51a14380469da896d309acc995809a1e668fbc2f76bde98c389834d8ee22");
  ASSERT_SAME(expected, uint256_sub(left, right));

}

void test_negate(TestObjs *objs) {
  UInt256 result;

  result = uint256_negate(objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_negate(objs->one);
  ASSERT_SAME(objs->max, result);

  result = uint256_negate(objs->max);
  ASSERT_SAME(objs->one, result);
}

void test_rotate_left(TestObjs *objs) {
  UInt256 result;

  // rotating the value with just the most significant bit set
  // one position to the left should result in the value equal to 1
  // (i.e., the value with only the least significant bit set)
  result = uint256_rotate_left(objs->msb_set, 1);
  ASSERT_SAME(objs->one, result);

  // after rotating the "rot" value left by 4 bits, the resulting value should be
  //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
  result = uint256_rotate_left(objs->rot, 4);
  ASSERT(0x00000ABCU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xD0000000U == result.data[7]);

  UInt256 pillar;
  uint32_t pillar_data[8] = { 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U };
  pillar = uint256_create(pillar_data);
  
  result = uint256_rotate_left(pillar, 1);
  ASSERT(0x00000003U == result.data[0]);
  ASSERT(0x00000003U == result.data[1]);
  ASSERT(0x00000003U == result.data[2]);
  ASSERT(0x00000003U == result.data[3]);
  ASSERT(0x00000003U == result.data[4]);
  ASSERT(0x00000003U == result.data[5]);
  ASSERT(0x00000003U == result.data[6]);
  ASSERT(0x00000003U == result.data[7]);

  //More rotate tests.
  //First test: Rotating by more than 32 bits. Identical numbers.
  
  result = uint256_rotate_left(pillar, 33);
  ASSERT(0x00000003U == result.data[0]);
  ASSERT(0x00000003U == result.data[1]);
  ASSERT(0x00000003U == result.data[2]);
  ASSERT(0x00000003U == result.data[3]);
  ASSERT(0x00000003U == result.data[4]);
  ASSERT(0x00000003U == result.data[5]);
  ASSERT(0x00000003U == result.data[6]);
  ASSERT(0x00000003U == result.data[7]);

  //Rotating by more than 256 bits. Identical numbers.
  
  result = uint256_rotate_left(pillar, 257);
  ASSERT(0x00000003U == result.data[0]);
  ASSERT(0x00000003U == result.data[1]);
  ASSERT(0x00000003U == result.data[2]);
  ASSERT(0x00000003U == result.data[3]);
  ASSERT(0x00000003U == result.data[4]);
  ASSERT(0x00000003U == result.data[5]);
  ASSERT(0x00000003U == result.data[6]);
  ASSERT(0x00000003U == result.data[7]);

  //rotating by 0. Identical numbers
  result = uint256_rotate_left(pillar, 0);
  ASSERT(0x80000001U == result.data[0]);
  ASSERT(0x80000001U == result.data[1]);
  ASSERT(0x80000001U == result.data[2]);
  ASSERT(0x80000001U == result.data[3]);
  ASSERT(0x80000001U == result.data[4]);
  ASSERT(0x80000001U == result.data[5]);
  ASSERT(0x80000001U == result.data[6]);
  ASSERT(0x80000001U == result.data[7]);

  //Rotate random number by 4 left overall.
  UInt256 testing_num = uint256_create_from_hex("8733cf23e74b0c561ec3700000000000000000000f3e029249e999bfaeda3790");
  result = uint256_rotate_left(testing_num, 516);
  ASSERT(0xeda37908U == result.data[0]);
  ASSERT(0x9e999bfaU == result.data[1]);
  ASSERT(0xf3e02924U == result.data[2]);
  ASSERT(0x00000000U == result.data[3]);
  ASSERT(0x00000000U == result.data[4]);
  ASSERT(0xec370000U == result.data[5]);
  ASSERT(0x74b0c561U == result.data[6]);
  ASSERT(0x733cf23eU == result.data[7]);

  //Rotate same random number by 36 left overall.
  result = uint256_rotate_left(testing_num, 36);
  ASSERT(0x733cf23eU == result.data[0]);
  ASSERT(0xeda37908U == result.data[1]);
  ASSERT(0x9e999bfaU == result.data[2]);
  ASSERT(0xf3e02924U == result.data[3]);
  ASSERT(0x00000000U == result.data[4]);
  ASSERT(0x00000000U == result.data[5]);
  ASSERT(0xec370000U == result.data[6]);
  ASSERT(0x74b0c561U == result.data[7]);
}

void test_rotate_right(TestObjs *objs) {
  UInt256 result;

  // rotating 1 right by 1 position should result in a value with just
  // the most-significant bit set
  result = uint256_rotate_right(objs->one, 1);
  ASSERT_SAME(objs->msb_set, result);

  // after rotating the "rot" value right by 4 bits, the resulting value should be
  //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
  result = uint256_rotate_right(objs->rot, 4);
  ASSERT(0x0000000AU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xBCD00000U == result.data[7]);

  UInt256 pillar;
  uint32_t pillar_data[8] = { 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U, 0x80000001U };
  pillar = uint256_create(pillar_data);

  result = uint256_rotate_right(pillar, 1);
  ASSERT(0xC0000000U == result.data[0]);
  ASSERT(0xC0000000U == result.data[1]);
  ASSERT(0xC0000000U == result.data[2]);
  ASSERT(0xC0000000U == result.data[3]);
  ASSERT(0xC0000000U == result.data[4]);
  ASSERT(0xC0000000U == result.data[5]);
  ASSERT(0xC0000000U == result.data[6]);
  ASSERT(0xC0000000U == result.data[7]);

  //More rotate tests.
  //First test: Rotating by more than 32 bits. Identical numbers.
  
  result = uint256_rotate_right(pillar, 33);
  ASSERT(0xC0000000U == result.data[0]);
  ASSERT(0xC0000000U == result.data[1]);
  ASSERT(0xC0000000U == result.data[2]);
  ASSERT(0xC0000000U == result.data[3]);
  ASSERT(0xC0000000U == result.data[4]);
  ASSERT(0xC0000000U == result.data[5]);
  ASSERT(0xC0000000U == result.data[6]);
  ASSERT(0xC0000000U == result.data[7]);

  //First test: Rotating by more than 256 bits. Identical numbers.
  
  result = uint256_rotate_right(pillar, 257);
  ASSERT(0xC0000000U == result.data[0]);
  ASSERT(0xC0000000U == result.data[1]);
  ASSERT(0xC0000000U == result.data[2]);
  ASSERT(0xC0000000U == result.data[3]);
  ASSERT(0xC0000000U == result.data[4]);
  ASSERT(0xC0000000U == result.data[5]);
  ASSERT(0xC0000000U == result.data[6]);
  ASSERT(0xC0000000U == result.data[7]);

  //rotating by 0. Identical numbers
  result = uint256_rotate_right(pillar, 0);
  ASSERT(0x80000001U == result.data[0]);
  ASSERT(0x80000001U == result.data[1]);
  ASSERT(0x80000001U == result.data[2]);
  ASSERT(0x80000001U == result.data[3]);
  ASSERT(0x80000001U == result.data[4]);
  ASSERT(0x80000001U == result.data[5]);
  ASSERT(0x80000001U == result.data[6]);
  ASSERT(0x80000001U == result.data[7]);

  //Rotate random number by 4 left overall.
  UInt256 testing_num = uint256_create_from_hex("8733cf23e74b0c561ec3700000000000000000000f3e029249e999bfaeda3790");
  result = uint256_rotate_right(testing_num, 516);
  ASSERT(0xfaeda379U == result.data[0]);
  ASSERT(0x249e999bU == result.data[1]);
  ASSERT(0x00f3e029U == result.data[2]);
  ASSERT(0x00000000U == result.data[3]);
  ASSERT(0x00000000U == result.data[4]);
  ASSERT(0x61ec3700U == result.data[5]);
  ASSERT(0x3e74b0c5U == result.data[6]);
  ASSERT(0x08733cf2U == result.data[7]);

  //Rotate same random number by 36 left overall.
  result = uint256_rotate_right(testing_num, 36);
  ASSERT(0x249e999bU == result.data[0]);
  ASSERT(0x00f3e029U == result.data[1]);
  ASSERT(0x00000000U == result.data[2]);
  ASSERT(0x00000000U == result.data[3]);
  ASSERT(0x61ec3700U == result.data[4]);
  ASSERT(0x3e74b0c5U == result.data[5]);
  ASSERT(0x08733cf2U == result.data[6]);
  ASSERT(0xfaeda379U == result.data[7]);
}
