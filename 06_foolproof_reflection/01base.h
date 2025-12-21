/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
//
//   CGIONE
//   File: 01base.h
//
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

#pragma once

//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // datatypes

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

//
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


// COMPILER SPECIFIC
#define gcc_pragmas
#ifdef gcc_pragmas

// https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html

#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"
#pragma GCC diagnostic warning "-Wpedantic"
#pragma GCC diagnostic warning "-Wunknown-pragmas"
#pragma GCC diagnostic warning "-Wpadded"
/*        
// real fun
     #pragma GCC diagnostic warning "-Wsuggest-attribute=const"
     #pragma GCC diagnostic warning "-Wtraditional"
     #pragma GCC diagnostic warning "-Wtraditional-conversion"
     #pragma GCC diagnostic warning "-Wmissing-prototypes"
     #pragma GCC diagnostic warning "-Wmissing-declarations"

// argh!
     #pragma region start
     #pragma GCC diagnostic ignored "-Wunknown-pragmas"
     #pragma endregion start
// wow
     #pragma GCC diagnostic warning "-Wsystem-headers"
*/
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Winline"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wcast-qual"
#pragma GCC diagnostic warning "-Wcast-align"
#pragma GCC diagnostic warning "-Wlogical-op"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wparentheses"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wswitch-bool"
#pragma GCC diagnostic warning "-Wswitch-enum"
#pragma GCC diagnostic warning "-Wdangling-else"
#pragma GCC diagnostic warning "-Wpointer-arith"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wredundant-decls"
#pragma GCC diagnostic warning "-Wduplicated-cond"
#pragma GCC diagnostic warning "-Wunreachable-code"
#pragma GCC diagnostic warning "-Wdouble-promotion"
#pragma GCC diagnostic warning "-Wduplicated-branches"
#pragma GCC diagnostic warning "-Wunsuffixed-float-constants"
#pragma GCC diagnostic warning "-Waggressive-loop-optimizations"

#pragma GCC diagnostic warning "-Wvla"
#pragma GCC diagnostic warning "-Wabi"

#pragma GCC diagnostic warning "-Wunused"
#pragma GCC diagnostic warning "-Wunused-value"
#pragma GCC diagnostic warning "-Wunused-label"

#pragma GCC diagnostic warning "-Wunused-function"
#pragma GCC diagnostic warning "-Wunused-parameter"
#pragma GCC diagnostic warning "-Wunused-but-set-variable"
#pragma GCC diagnostic warning "-Wunused"

#pragma GCC diagnostic warning "-Wformat=5"
#pragma GCC diagnostic warning "-Wformat-signedness"
#pragma GCC diagnostic warning "-Wformat-overflow=5"
#pragma GCC diagnostic warning "-Wformat-truncation=5"

#pragma GCC diagnostic warning "-Warray-bounds"
#pragma GCC diagnostic warning "-Wstringop-overread"
#pragma GCC diagnostic warning "-Wstringop-overflow=5"
#pragma GCC diagnostic warning "-Wsign-compare"

#pragma GCC diagnostic warning "-Wstrict-overflow=2" // >2 == madness


#endif // gcc_pragmas

//
typedef int8_t            i8;
typedef uint8_t           u8;
typedef int32_t          b32;
typedef int32_t          i32;
typedef uint32_t         u32;

//
typedef struct _v2i { i32 x; i32 y; } v2i;

// 
typedef struct _rect2i { i32 left, top, right, bottom; } rect2i;

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// debug
#define debS printf("Stopped at line %d\n", __LINE__);

#define deb_i32(int) printf("%s = %d\n", #int, int);

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// does NOT include term char!
// https://gist.github.com/hstaos/774944b0c136ef0e815c 
// "\0" is a character, the null character with ASCII value 0.
size_t base_strlen(char const * s) { size_t len = 0; while(*s++) { len++; } return len; }

//