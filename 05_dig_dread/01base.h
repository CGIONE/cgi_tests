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

//
typedef int8_t            u8;
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

// UTILS

//
#define get_name(var) #var



// COLORS

typedef union rgba_x 
{ u32 u32value; struct { u8 B; u8 G; u8 R; u8 A; } channels; } rgba;

     // make rgba 
     rgba make_rgba_4chan(u8 R, u8 G, u8 B, u8 A) { 
          rgba res = {0};
          res.channels.A = A; res.channels.R = R;
          res.channels.G = G; res.channels.B = B;
          return res;
     }

     // make_rgba_u32
     rgba make_rgba_u32(u32 value) { rgba res = {0}; res.u32value = value; return res; }

     // u32 from 4 channels // ARGB
     u32 make_u32ch(u8 R, u8 G, u8 B, u8 A) 
     { 
          return (u32)
               A << 24 |
               R << 16 |
               G <<  8 |
               B;
          
     }

     // TODO: recheck win32 vs argb
#if 0
     u32 make_u32ch2(u8 R, u8 G, u8 B, u8 A) 
     { 
     // Fix: Swap R and B channels to output BGRA, 
     // which the Windows GDI function StretchDIBits expects.
     return (u32)
          A << 24 | // A (Byte 4)
          B << 16 | // B (Byte 3, where GDI expects Red)
          G << 8  | // G (Byte 2)
          R;        // R (Byte 1, where GDI expects Blue)
     }
#endif //

     // store rgba
     rgba store_rgba(rgba src) { return src; }

     //
     #define debug_rgba(color_rgba)                       \
     printf("\nu32[%s]:[0x%08X]\nR[%d] G[%d] B[%d] A[%d]\n", \
            get_name(color_rgba), color_rgba.u32value, color_rgba.channels.R, color_rgba.channels.G, color_rgba.channels.B, color_rgba.channels.A);


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
