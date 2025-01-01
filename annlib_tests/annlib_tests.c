#include "annlib_tests/annlib_tests.h"
#include "annlib/annlib.h"
#include "annunit/annunit.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

void initAnnLibTests(void)
{
	initTests();
}

void concludeAnnLibTests(void)
{
	concludeTests();
}

void runAllTests(void)
{
	runArenaTests();
	runMathTests();
	runStringTests_Utility();
	runStringTests_Conversion();
}

void runArenaTests(void)
{
	printHeading(makeString("Arena tests"));

	// makeArena tests
	printSubheading(makeString("makeArena"));
	u64 arenaLength = 4;
	arena a = makeArena(arenaLength);
	printf("Arena was allocated successfully\n");
	testPointer(false, a.content);
	printf("Arena is correct length\n");
	testU64(arenaLength, a.length);
	printf("Arena.used is set to 0\n");
	testU64(0, a.used);

	// allocateFromArena and resetArena tests
	printSubheading(makeString("allocateFromArena and resetArena"));
	ptr(s32) p = allocateFromArena(addr(a), sizeof(s32));
	printf("Can allocate a pointer in arena\n");
	testPointer(false, p);
	s32 value = INT_MIN;
	deref(p) = value;
	printf("Can set value of pointer\n");
	testS32(value, deref(p));
	ptr(s32) p2 = allocateFromArena(addr(a), sizeof(s32));
	printf("Cannot allocate if arena is out of space\n");
	testPointer(true, p2); // Don't have enough space
	resetArena(addr(a)); 
	p2 = allocateFromArena(addr(a), sizeof(s32));
	printf("Can allocate again after resetting arena\n");
	testPointer(false, p2);
	printf("Arena's memory was set to 0 after reset\n");
	testS32(0, deref(p2));
	deref(p2) = value;
	printf("Can set value of pointer after reset\n");
	testS32(value, deref(p2));

	// Cleanup
	freeArena(a);
}

void runMathTests(void)
{
	printHeading(makeString("Math tests"));

	// getS32Length tests
	printSubheading(makeString("getS32Length"));
	printf("Correct length for positive number\n");
	testU32(10, getS32Length(INT_MAX));
	printf("Correct length for negative number\n");
	testU32(11, getS32Length(INT_MIN));
	printf("Correct length for zero\n");
	testU32(1, getS32Length(0));

	// getU64Length tests
	printSubheading(makeString("getU64Length"));
	printf("Correct length for positive number\n");
	testU64(20, getU64Length(ULLONG_MAX));
	printf("Correct length for zero\n");
	testU64(1, getU64Length(0));

	// absS64 tests
	printSubheading(makeString("absS64"));
	printf("Correct absolute value for positive number\n");
	testS64(LLONG_MAX, absS64(LLONG_MAX));
	printf("Correct absolute value for negative number\n");
	testS64(LLONG_MAX, absS64(LLONG_MIN + 1));
	printf("Correct absolute value for zero\n");
	testS64(0, absS64(0));
}

// These tests cover the "search" or "equality" string functions
void runStringTests_Utility(void)
{
	printHeading(makeString("String search tests"));

	// areStringsEqual tests
	printSubheading(makeString("areStringsEqual"));
	string foo = makeString("foo");
	string bar = makeString("bar");
	string foobar = makeString("foobar");
	printf("Detects equal strings\n");
	testBool(true, areStringsEqual(foo, makeString("foo")));
	printf("Detects unequal strings\n");
	testBool(false, areStringsEqual(foo, bar));

	// stringStartsWith tests
	printSubheading(makeString("stringStartsWith"));
	printf("Detects a string that starts with another\n");
	testBool(true, stringStartsWith(foobar, foo));
	printf("Detects a string that does not start with another\n");
	testBool(false, stringStartsWith(foobar, bar));

	// findString tests
	printSubheading(makeString("findString"));
	printf("Detects a valid substring at the beginning\n");
	testS64(0, findString(foo, foobar));
	printf("Detects a valid substring in the middle\n");
	testS64(2, findString(makeString("oba"), foobar));
	printf("Detects a valid substring at the end\n");
	testS64(5, findString(makeString("r"), foobar));
	printf("Returns -1 for an invalid substring\n");
	testS64(-1, findString(makeString("baz"), foobar));

	// stringIsNumber tests
	printSubheading(makeString("stringIsNumber"));
	printf("Detects valid characters\n");
	testBool(true, stringIsNumber(makeString("1234")));
	printf("Detects invalid characters\n");
	testBool(false, stringIsNumber(makeString("1234a")));

	// buildString tests
	printSubheading(makeString("buildString"));
	arena a = makeArena(50);
	printf("Builds a single string\n");
	string str = makeString("A single string");
	string result = buildString(addr(a), 1, str);
	testString(str, result);
	resetArena(addr(a));
	printf("Builds multiple strings\n");
	str = makeString("Another rather long string");
	result = buildString(addr(a), 4,
		makeString("Another "),
		makeString("rather "),
		makeString("long "),
		makeString("string")
	);
	testString(str, result);
	printf("String is empty when arena is not big enough\n");
	result = buildString(addr(a), 1,
		makeString("A string that is much too big")
	);
	testString(makeString(""), result);

	// Cleanup
	freeArena(a);
}

// These tests cover the "ToString" and "ToNumber" functions
void runStringTests_Conversion(void)
{
	printHeading(makeString("String conversion tests"));

	// stringToNumber tests
	printSubheading(makeString("stringToNumber"));
	printf("Positive number is converted to string\n");
	arena a = makeArena(1024);
	ptr(s32) number = allocateFromArena(addr(a), sizeof(s32));
	assert(number);
	bool result = stringToNumber(makeString("123"), number);
	testBool(true, result);
	testS32(123, deref(number));
	printf("Negative number is converted to string\n");
	result = stringToNumber(makeString("-123"), number);
	testBool(true, result);
	testS32(-123, deref(number));
	printf("Zero is converted to string\n");
	result = stringToNumber(makeString("0"), number);
	testBool(true, result);
	testS32(0, deref(number));
	deref(number) = 123; // Reset number to non-zero value
	printf("Invalid input is not converted and set to 0\n");
	result = stringToNumber(makeString("12a3"), number);
	testBool(false, result);
	testS32(0, deref(number));
	resetArena(addr(a));

	// pointerToString tests
	printSubheading(makeString("pointerToString"));
	printf("Converts valid pointer\n");
	string str = pointerToString(addr(a), number);
	s32 bufferLen = 20;
	ptr(s8) buffer = allocateFromArena(addr(a), sizeof(s8) * bufferLen);
	assert(buffer);
	snprintf(buffer, bufferLen, "%zX", (size_t) number);
	buffer[bufferLen - 1] = 0;
	string strFromBuffer = charPtrToString(buffer);
	testString(strFromBuffer, str);
	printf("Null pointer returns NULL\n");
	testString(makeString("NULL"), pointerToString(addr(a), NULL));
	resetArena(addr(a));

	// u64ToString tests
	printSubheading(makeString("u64ToString"));
	printf("Positive number is converted to string\n");
	str = u64ToString(addr(a), ULLONG_MAX);
	testString(makeString("18446744073709551615"), str);
	printf("Zero is converted to string\n");
	str = u64ToString(addr(a), 0);
	testString(makeString("0"), str);

	// s64ToString tests
	printSubheading(makeString("s64ToString"));
	printf("Positive number is converted to string\n");
	str = s64ToString(addr(a), LLONG_MAX);
	testString(makeString("9223372036854775807"), str);
	printf("Negative number is converted to string\n");
	str = s64ToString(addr(a), LLONG_MIN);
	testString(makeString("-9223372036854775808"), str);
	printf("Zero is converted to string\n");
	str = s64ToString(addr(a), 0);
	testString(makeString("0"), str);

	// s32ToString tests
	printSubheading(makeString("s32ToString"));
	printf("Positive number is converted to string\n");
	str = s32ToString(addr(a), INT_MAX);
	testString(makeString("2147483647"), str);
	printf("Negative number is converted to string\n");
	str = s32ToString(addr(a), INT_MIN);
	testString(makeString("-2147483648"), str);
	printf("Zero is converted to string\n");
	str = s32ToString(addr(a), 0);
	testString(makeString("0"), str);

	// Cleanup
	freeArena(a);
}