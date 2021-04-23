/*
 * CS:APP Data Lab
 *
 * <Sittiphat Narkmanee 805190133>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function.
     The max operator count is checked by dlc. Note that '=' is not
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */

// Demorgan's law says you can distribute negation and will negate values and flip and/or signs
int bitAnd(int x, int y) {
  return ~(~x | ~y);
}
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */

// First we shift 'x' 8n times (n << 3 is is n*(2^3)) of our desired byte index of interest
// until to the very end.
// Then we create a mask by having all binary 1's since ~0 is -1 
// logically shift it 8 bits (one byte) then invert all the numbers so we only have eight 1's
// at the end.
// Now we "&" the right shifted x with the mask to copy only desired first 8 bits.
int getByte(int x, int n) {
  // V1
  // return (((0xFF << (n << 3)) & x) >> (n >> 3)) & ~(~0 << 8);
  return (x >> (n << 3)) & ~(~0 << 8); 
}
/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */

// Goal is to convert arithmetic right shifts to logic shifts
// First perform normal arithmetic shift first then use a mask with &
// to convert the copied MSB to all 0's especially when MSB is 1.
// We get the mask by using 1 and logically shifting until 100000...
// If should then be shifted n-1, but we cannot use '-' so we use << 1.
int logicalShift(int x, int n) {

  // int mask = ~(1 << (32 - n));
  // return mask & ((x >> n) | mask);
  int mask = ~((1 << 31) >> n << 1);
  return (x >> n) & mask;

}
/*
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3
 */

// We create a "saved falloff of bits" by logically shifting left
// until our bits of concern are in position and expressed whereas
// the left of the right side only contains 1's.
// Then we logically shift the bits (taken from last function) then compare using
// "|" since this will create a mask for the the 0's in saved falloff
// and copy the rest of nonzeros so the bits of concern are "rotated". 
int rotateRight(int x, int n) {
  //int x_shifted = x >> n;
  /*int mask = ~(~0 << n);
  int saved_falloff = x & mask;
  // Reposition the saved falloff bits then 0's create mask for '|'
  return (saved_falloff << 31 >> n) | x_shifted;*/
  int saved_falloff = x << (32 + (~n + 1));
  
  int mask = ~((1 << 31) >> n << 1);
  int logic_shifted = (x >> n) & mask;
  
  return logic_shifted | saved_falloff;

}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */

// We first create a boolean represented as an int using xor and '!!' to
// also make nonzeroes become just 1.
// Then we would like to convert it to be just all 1's or 0's.
// If boolean is 0 it is already done for us but if just 1, then we need to
// shift left then right 31 times (makes 0 the same).
// Now we either have all 1's or 0's so then we can use this as a mask for
// our return statement. If 1 we copy everything y and if 0 then we negate/invert
// the mask to make it that it copies nothing and just evaluates 0, which becomes
// another mask for the '|'.
int conditional(int x, int y, int z) {
  int boolean = !!(x ^ 0);
  int convert = boolean << 31 >> 31;
  return (convert & y) | (~convert & z);
  
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */

// The evaluation of "|" x and its two's complement will always evaluate to -1 or -2 except
// when it is 0 which will evaluate to 1. Shifting the MSB then we know for nonzeros, MSB will 
// always be 1 and zero will be the only one with a 0 MSB. By adding 1, we return 1 if it is 0
// and 1 for the rest.  
int bang(int x) {
  int neg_x = ~x + 1;
  return 1 + ((neg_x | x) >> 31);
}
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
// Taken similar to a binary search algorithm which has a O(log2n), we continually 
// split the 32 bits incrementally and compare using xor to make two 1's equal to 
// 0 in x. Therefore if we compare it with 1 which is ..0001 with & it will return 0
// since the LSB if it is even will be 0 and 1 if it is 0 since the -1 (which is 1111..)
// will be compared always returning 1 or true.
int bitParity(int x) {
  x = (x >> 1) ^ x;
  x = (x >> 2) ^ x;
  x = (x >> 4) ^ x;
  x = (x >> 8) ^ x;
  x = (x >> 16) ^ x;

  return 1 & x;
}
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
// If we add one to Tmax then it becomes a Tmin which is all 0's except MSB.
// Then the inverse of Tmax is also Tmin, so plus Tmin + Tmin causes an overflow
// which equates it to be 1. Putting these properties together, if it is Tmax then
// the left side will be Tmin or 0 and right side will be 1, which banged will return two
// different numbers and be true. However, if x is not Tmax, then the left side will
// return the same value since any other number will return both sides not 1 or both sides 
// not 0, accounting if x is -1.
int isTmax(int x) {
  /*int Tmax = ~(1 << 31);
  return !(x + (~Tmax + 1));*/

  return !(1 + x) ^ !(~x+~x);   
}
/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
// To see if x will fit in bits we shift 31 - n bits left then right
// so that we can cut off the MSB if the bit does not fit. 
// After we do this we compare to the original x by using xor
// so inorder to do this we invert all of x so that if it is the same,
// then we will get all 1's or -1. Inverting -1, we get 0 so the negation of 
// that is 1 or true. If the x's are not the same after shifting, then
// it will return 0 indicating the value does not fit in n bits. 
int fitsBits(int x, int n) {

  int shift = (32 + (~n + 1));
  int shifted_x = (x << shift) >> shift;
  return !~(~x ^ shifted_x);
}
/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
// If x is positive it is a simple x >> n, but when it is negative then we have a problem.
// To account for this and knowing that we need to +1 to n if x is negative, we use a mask
// with an & to account for +1 if x is negative (MSB is 1) and therefore there must be a +1. 
int divpwr2(int x, int n) {
  return (x + ((x >> 31) & ((1 << n) + ~0))) >> n;
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */

// Definition of 2's complement to get negative, invert then add 1.
int negate(int x) {
  return ~x + 1;
} 
/*
 * isPositive - return 1 if x > 0, return 0 otherwise
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
// First we store x's MSB to determine if it is postive or negative.
// After shifting the MSB to the LSB it can either be 1 (neg) or 0 (pos).
// We put a bang to return true if it is positive or 0, and also check if 
// the value is '0' as well since 0 had a 0 MSB but is not positive.
// Additionally the double bang is to account if 0 is not 0 which will
// just equate to 1 and act as a mask to the right side.
int isPositive(int x) {
  return !!x & !((x & (1 << 31)) >> 31);
}
/*
 * isGreater - if x > y  then return 1, else return 0
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
// If x is greater than y then when we subtract the two it should evaluate to be a positive number (0 MSB).
// Else it is a negative number. We also need to account for both being the same number where the diff
// is 0 but not 1, so we account for that with the diff being 0 cancelling same number being greater and 
// the rest of the time when it is not the same number just being a mask.
int isGreater(int x, int y) {
  
  int diff = x + (~y + 1);
  return (!!diff & !(1 & (diff >> 31)) & !((x ^ (1 << 31))) & (y ^ ~(1 << 31))) & ((x ^ ~(1 << 31)));
}
/*
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */

// First we need to stores as 1 or 0 the MSB of x, y, and its result during subtraction.
// Then we need to check for the condition that if both x and y are either negative or
// positive then overflow will never happen. However, when the signs are opposite overflow does
// occur when the difference is negative when x is positive and y is negative, and also when
// the difference is positive and x is negative where y is positive.
// The return statement checks for this by saying at least one condition (since a 1 will overide one side)
// on BOTH sides of the '&' comparison must be true (1) for there to not be overflow. 
int subOK(int x, int y) {

int neg_y = ~y + 1;
int diff = x + neg_y;

int x_msb = !(1 & (x >> 31)); // If MSB of x is pos return 1, else 0
int y_msb = !(1 & (y >> 31)); // If MSB of y is pos return 1, else 0
int diff_msb = !(1 & (diff >> 31)); // If MSB of diff is pos return 1, else 0


return (!x_msb | y_msb | diff_msb) & (x_msb | !y_msb | !diff_msb);  


  // V1
  /*int neg_y = ~y + 1;
  int orig_diff = x + ~neg_y;
  int same_check = !(orig_diff + 1); 
  int x_msb = (x >> 31) && 1;
  int y_msb = (neg_y >> 31) && 1;
  int diff = (x_msb + (~y_msb + 1));
  int sum = x_msb + y_msb;
  int xor = x_msb ^ y_msb;
  return (!(sum + (~xor + 1))) | same_check;
  // return !(x_msb + y_msb) & !!diff;
  // return (x_msb ^ y_msb) & !!(x_msb + y_msb); */
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */

// Similar to a binary search algorithm which has O(log2n), we are trying to find the location of a 1
// in a string of the rest being 0's, its index from right to left is the answer. 
// If it is on the right half side the 1 will be shifted out. If it is on the left side, then it will return 1 or true.
// Then we increment our counter to 16 first where until if there are no other hits after then that is our answer. Since we 
// know where the 1 is at we throw our the side it is not on.
int ilog2(int x) {
  int floor = 0;

  int left_side = !!(x >> 16) << 31 >> 31;
  floor = floor + (16 & left_side);
  x = x >> (16 & left_side);
  
  left_side = !!(x >> 8) << 31 >> 31;
  floor = floor + (8 & left_side);
  x = x >> (8 & left_side);

  left_side = !!(x >> 4) << 31 >> 31;
  floor = floor + (4 & left_side);
  x = x >> (4 & left_side);
 
 
  left_side = !!(x >> 2) << 31 >> 31;
  floor = floor + (2 & left_side);
  x = x >> (2 & left_side);
  
  left_side = !!(x >> 1) << 31 >> 31;
  floor = floor + (1 & left_side);
  
  
  return floor; 
}
