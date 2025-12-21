/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
//
//   CGIONE
//   File: 04game.h
//
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

#pragma once

//
#include "02data.h"
#include "03platform.h"

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// GAME DEFINEs
// --- Time Constants (Define your fixed logic rate) ---
#define TARGET_FPS 60 // Example: 60 FPS (1 second / 60 = 0.016667s)
#define TARGET_FPS_US 16667ULL // Example: 60 FPS (1 second / 60 = 0.016667s)
#define MAX_LOGIC_TICKS_PER_FRAME 5 // Safety: Max ticks allowed to catch up lag (Prevents the "spiral of death")


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


typedef struct _game {

     b32 is_game_running;
     v2i buffer_xy;
     size_t buffer_size;
     uint64_t logtick;

     //
     void *ptr_main;

} gameproto;

gameproto gameOne; gameproto *game = &gameOne;


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

#define MAX_VIZ_LINES         5999
#define FILL_PAT              0x88888888
#define NUM_LINE_FUNCS        2
#define NUM_BUFFERS           2 // forward and reverse buffers
#define NUM_TEST_NAMES        4

#define NUM_SYMMETRY_LINES    5
#define NUM_OCTANT_LINES      8
#define NUM_CARDINALTB_LINES  8
#define NUM_BS_LINES          6

// very long buffer size to shush compiler warning
#define STRING_LENGTH_SAFETY 384

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// GTR

//
b32 line_bres_gtr(void *buf, line2i line, u32 color) 
{
    u32 *base_ptr = (u32 *)buf;
    i32 width = game->buffer_xy.x;
    i32 height = game->buffer_xy.y;

    // 1. TRIVIAL REJECT
    if (line.x1 < 0 || line.y1 < 0 || line.x1 >= width || line.y1 >= height ||
        line.x2 < 0 || line.y2 < 0 || line.x2 >= width || line.y2 >= height) {
        return 0;
    }

    // 2. NORMALIZATION
    if (line.x1 > line.x2) {
        i32 tx = line.x1; line.x1 = line.x2; line.x2 = tx;
        i32 ty = line.y1; line.y1 = line.y2; line.y2 = ty;
    }

    // 3. UNSIGNED DELTAS (Prevents Undefined Behavior / Warnings)
    u32 dx = (u32)line.x2 - (u32)line.x1; 
    i32 dy_raw = line.y2 - line.y1;
    u32 dy_abs = (dy_raw < 0) ? (u32)-(i64)dy_raw : (u32)dy_raw;
    
    i32 stride_y = (dy_raw >= 0) ? width : -width;
    u32 *ptr = base_ptr + (line.y1 * width + line.x1);

    // 4. X-MAJOR
    if (dx >= dy_abs) {
        // err = 2*dy - dx. We use i64 for the intermediate math to 
        // absolutely guarantee no overflow during the decision setup.
        i64 err = (2 * (i64)dy_abs) - (i64)dx;
        i64 d_err_diag = 2 * ((i64)dy_abs - (i64)dx);
        i64 d_err_straight = 2 * (i64)dy_abs;

        for (u32 i = 0; i <= dx; i++) {
            *ptr = color;
            if (err >= 0) {
                ptr += stride_y;
                err += d_err_diag;
            } else {
                err += d_err_straight;
            }
            ptr += 1;
        }
    } 
    // 5. Y-MAJOR
    else {
        i64 err = (2 * (i64)dx) - (i64)dy_abs;
        i64 d_err_diag = 2 * ((i64)dx - (i64)dy_abs);
        i64 d_err_straight = 2 * (i64)dx;

        for (u32 i = 0; i <= dy_abs; i++) {
            *ptr = color;
            if (err >= 0) {
                ptr += 1;
                err += d_err_diag;
            } else {
                err += d_err_straight;
            }
            ptr += stride_y;
        }
    }
    return 1;
}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

b32 line_sut_broken(void *buf, line2i line, u32 color) 
{
    u32 *base_ptr = (u32 *)buf;
    i32 width = game->buffer_xy.x;
    i32 height = game->buffer_xy.y;

    // Reject out of bounds
    if (line.x1 < 0 || line.y1 < 0 || line.x1 >= width || line.y1 >= height ||
        line.x2 < 0 || line.y2 < 0 || line.x2 >= width || line.y2 >= height) {
        return 0;
    }

    // --- NO NORMALIZATION HERE ---
    // This is the intentional "Break" that our suite should catch.

    // Force unsigned subtraction to stop the compiler from assuming things about overflow
    i32 dx_raw = line.x2 - line.x1;
    i32 dy_raw = line.y2 - line.y1;
    
    // Explicit unsigned absolute values
    u32 dx_abs = (dx_raw < 0) ? (u32)-(i64)dx_raw : (u32)dx_raw;
    u32 dy_abs = (dy_raw < 0) ? (u32)-(i64)dy_raw : (u32)dy_raw;
    
    i32 step_x = (dx_raw >= 0) ? 1 : -1;
    i32 step_y = (dy_raw >= 0) ? width : -width;
    
    u32 *ptr = base_ptr + (line.y1 * width + line.x1);

    if (dx_abs >= dy_abs) {
        // Tie-breaker setup
        i32 err = (2 * (i32)dy_abs) - (i32)dx_abs;
        for (u32 i = 0; i <= dx_abs; i++) {
            *ptr = color;
            if (err >= 0) {
                ptr += step_y;
                err -= (2 * (i32)dx_abs);
            }
            err += (2 * (i32)dy_abs);
            ptr += step_x;
        }
    } else {
        i32 err = (2 * (i32)dx_abs) - (i32)dy_abs;
        for (u32 i = 0; i <= dy_abs; i++) {
            *ptr = color;
            if (err >= 0) {
                ptr += step_x;
                err -= (2 * (i32)dy_abs);
            }
            err += (2 * (i32)dx_abs);
            ptr += step_y;
        }
    }
    return 1;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


// CGI

// plot pixel
static inline void plot_pixel(v2i pixel_pos, u32 pixel_color)
{
     u32 *ptr_pixel = (u32 *)game->ptr_main;
     if (      pixel_pos.x >= 0 
          &&   pixel_pos.y >= 0 
          &&   pixel_pos.x < game->buffer_xy.x 
          &&   pixel_pos.y < game->buffer_xy.y
     )
     { ptr_pixel[pixel_pos.y * game->buffer_xy.x + pixel_pos.x] = pixel_color; }
}

// same xy as main!
static inline u32 readbuf_pixel(void *buf, v2i pixel_pos)
{
    // The pointer to the base of the pixel buffer (same as in plot_pixel)
    u32 *ptr_pixel = (u32 *)buf;
    
    i32 width = game->buffer_xy.x;
    i32 height = game->buffer_xy.y;

    // --- 1. Bounds Check ---
    if (
        pixel_pos.x >= 0 && 
        pixel_pos.y >= 0 && 
        pixel_pos.x < width && 
        pixel_pos.y < height
    )
    {
        // --- 2. Memory Indexing and Read ---
        // Formula: Y * Width + X
        return ptr_pixel[pixel_pos.y * width + pixel_pos.x];
    }

    // --- 3. Out-of-Bounds Handling ---
    // If the coordinates are outside the buffer, return a default color.
    // 0x00000000 is typically black.
    return 0x0; 
}

static inline u32 read_pixel(v2i pixel_pos)
{
    // The pointer to the base of the pixel buffer (same as in plot_pixel)
    u32 *ptr_pixel = (u32 *)game->ptr_main;
    
    i32 width = game->buffer_xy.x;
    i32 height = game->buffer_xy.y;

    // --- 1. Bounds Check ---
    if (
        pixel_pos.x >= 0 && 
        pixel_pos.y >= 0 && 
        pixel_pos.x < width && 
        pixel_pos.y < height
    )
    {
        // --- 2. Memory Indexing and Read ---
        // Formula: Y * Width + X
        return ptr_pixel[pixel_pos.y * width + pixel_pos.x];
    }

    // --- 3. Out-of-Bounds Handling ---
    // If the coordinates are outside the buffer, return a default color.
    // 0x00000000 is typically black.
    return 0x0; 
}



//
void cgi_fill_u32(u32 color)
{
     u32 *buf = (u32 *)game->ptr_main;
     for (size_t i = 0; i < game->buffer_size / sizeof(u32); i++)
     {
          *buf++ = color;
     }
     
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

//
b32 is_cursor_over(rect2i src)
{
     i32 right = src.left + src.width;
     i32 bottom = src.top + src.height;

     i32 mx = ptr_imouse->curr_pos.x;
     i32 my = ptr_imouse->curr_pos.y;
          
     
     if (
          mx >= src.left &&
          mx <  right    && // MouseX must be strictly less than the end boundary
          my >= src.top  &&
          my <  bottom      // MouseY must be strictly less than the end boundary
          )
     {
          return 1; // TRUE
     }

     return 0; // FALSE
}

//
void plot_rect2i(rect2i src, u32 color)
{
    v2i bufxy = game->buffer_xy;

    // Create a local copy for clipping
    rect2i cl_rect = src;

    // Pre-calculate the right and bottom coordinates for clarity
    i32 src_right = src.left + src.width;
    i32 src_bottom = src.top + src.height;

    // --- CLIPPING LOGIC ---
    
    // Completely off-screen checks (fast early exit)
    if (src_right <= 0 || src_bottom <= 0 || src.left >= bufxy.x || src.top >= bufxy.y)
    {
        return;
    }

    // Clip left side
    if (cl_rect.left < 0)
    {
        cl_rect.width += cl_rect.left; // cl_rect.left is negative, so this subtracts
        cl_rect.left = 0;
    }

    // Clip top side
    if (cl_rect.top < 0)
    {
        cl_rect.height += cl_rect.top; // cl_rect.top is negative
        cl_rect.top = 0;
    }

    // Clip right side
    if (cl_rect.left + cl_rect.width > bufxy.x)
    {
        cl_rect.width = bufxy.x - cl_rect.left;
    }

    // Clip bottom side
    if (cl_rect.top + cl_rect.height > bufxy.y)
    {
        cl_rect.height = bufxy.y - cl_rect.top;
    }

    // Check if the clipped rectangle is still valid
    if (cl_rect.width <= 0 || cl_rect.height <= 0)
    {
        return;
    }

     u32 *ptrS = (u32 *)game->ptr_main;

     // Move the pointer to the starting position of the clipped rectangle
     ptrS += cl_rect.top * bufxy.x + cl_rect.left;


     // Calculate the "stride" (how many pixels to skip at the end of a row)
     // to align with the start of the next row.
     i32 stride = bufxy.x - cl_rect.width;

     for (i32 y = 0; y < cl_rect.height; y++)
     {
          for (i32 x = 0; x < cl_rect.width; x++)
          {
               *ptrS++ = color;
          }

          ptrS += stride;
     }
 

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



typedef enum
{
     LINE_GTR  = 11,
     LINE_SUT_BROKEN
} line_type;

// test wrapper
void wrap_plot_line(i32 algo_type, void *buf, line2i line, u32 lcolor)
{

     switch (algo_type)
     {
          default: break;

          case LINE_GTR:
          {
               //
               line_bres_gtr(buf, line, lcolor);

          } break; // gtr

          case LINE_SUT_BROKEN:
          {
               //
               line_sut_broken(buf, line, lcolor);
          }

     } //

}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


void DrawChar(i32 x, i32 y, char c, u32 char_color)
{
     //
     i32 charHeight = 8;
     i32 charWidth = 8;

     //convert the character to an index
     c = c & 0x7F;
     if (c < ' ')   { c = 0;    }
     else           { c -= ' '; }

     //draw pixels
     for (i32 glyphY = 0; glyphY < charHeight; glyphY++)
     {
          for (i32 glyphX = 0; glyphX < charWidth; glyphX++)
          {
               if ( ( (1 << glyphY) & fontLarge[(u8)c][glyphX]) )
               {
                    plot_pixel((v2i){x + glyphX, y + glyphY}, char_color);
               }
          }
     }
}


void plot_text(v2i screen_pos, char *txt, u32 text_color)
{
     i32 current_x = screen_pos.x;
     for (i32 i = 0; i < (i32)base_strlen(txt); i++)
     {  
          DrawChar(current_x, screen_pos.y, txt[i], text_color); 
          current_x += 8;
     }
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// screen debug

//
void screendeb_i32(v2i pos, i32 src)
{
     char buf1[32] = {0};
     sprintf(buf1, "%d", src);

     u32 debcol = 0xFFFFFFFF;
     plot_text(pos, buf1, debcol);
}

void screendeb_v2i(v2i pos, v2i src)
{
     char buf1[64] = {0};
     sprintf(buf1, "%d x %d", src.x, src.y);

     u32 debcol = 0xFFFFFFFF;
     plot_text(pos, buf1, debcol);
}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// memcmp_result
typedef struct
{
     i32 result; // 0 pass, 1 fail
    u32 fail_color_bres; // New: Color of the first differing pixel in BUF0 (Bresenham)
    u32 fail_color_abr;  // Color of the first differing pixel in BUF1 (Abrash DDA)
     v2i fail_pix_coords; //
} memcmp_result;


memcmp_result game_memcmp_debug(const void *ptr1, const void *ptr2)
{
    // Cast to u32 pointers for pixel-wise comparison
    const u32 *p1 = (const u32 *)ptr1;
    const u32 *p2 = (const u32 *)ptr2;

    i32 width = game->buffer_xy.x;
    size_t size_bytes = game->buffer_size;


    // Calculate the number of pixels to compare
    size_t num_pixels = size_bytes / sizeof(u32);
    
    // Loop through the blocks, checking for inequality, pixel by pixel
    for (size_t i = 0; i < num_pixels; i++)
    {
        // If the current pixels differ
        if (p1[i] != p2[i])
        {
            // Calculate coordinates
            int y_coord = (int)(i / (size_t)width);
            int x_coord = (int)(i % (size_t)width);

            // Construct and return the failure result struct
            memcmp_result fail_data = {
                .result = 1, // 1 for failure
    .fail_color_bres = p1[i], // The color Bresenham drew
    .fail_color_abr = p2[i],  // The color Abrash drew (Background 0x8)
                .fail_pix_coords = {x_coord, y_coord}
            };

            return fail_data;
        }
    }

    // If the loop completes without finding a difference, the blocks are equal
    // Return a success struct (result = 0, with dummy coordinates)
    memcmp_result pass_data = {
        .result = 0,
        .fail_color_bres = 0,
        .fail_color_abr = 0,
        .fail_pix_coords = {0, 0}
    };

    return pass_data;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// suit
typedef struct 
{
     u32 scol;
     u32 fractal_col;
     i32 current_run_func;
     b32 is_gold[NUM_LINE_FUNCS];
     b32 tgl_line_func[NUM_LINE_FUNCS];

     b32 res_ttl_test[NUM_TEST_NAMES];
     b32 res_test1[NUM_SYMMETRY_LINES]; 
     b32 res_test2[NUM_OCTANT_LINES];
     b32 res_test3[NUM_CARDINALTB_LINES];
     b32 res_test4[NUM_BS_LINES];

     stw time_func;

     char buf_stat[STRING_LENGTH_SAFETY];
     void *buf[NUM_BUFFERS];

} test_suit_proto;

test_suit_proto suit1 = {0}; test_suit_proto *ptrs1 = &suit1;

// start
b32 suit_prepare()
{
     // buf size should already be non zero
     if(game->buffer_size < 1) { puts("buf size fail"); }

     for (size_t i = 0; i < NUM_BUFFERS; i++)
     {
          //
          ptrs1->buf[i] = malloc(game->buffer_size);
          if (ptrs1->buf[i] == NULL) { puts("FAIL: malloc buf[i]!"); }
          
          // fill
          base_memset_u32(ptrs1->buf[i], FILL_PAT, game->buffer_size / sizeof(u32));

     }
     
     // cmp bufs, calloc'd
     if(game_memcmp_debug(ptrs1->buf[0], ptrs1->buf[1]).result != 0)
     { puts("memcmp of buf0 vs buf1 failed on start!"); return FALSE; }

     // set single color
     ptrs1->scol = 0xAA00FF;

     // def tgl
     ptrs1->tgl_line_func[0] = TRUE;
     ptrs1->current_run_func = 0; // start gtr

     return TRUE;
}

// reset buffers to fill pattern calloc'd state
b32 suit_reset()
{
     b32 reset_res = TRUE;
     for (size_t i = 0; i < NUM_BUFFERS; i++)
     {
          if(ptrs1->buf[i] == NULL) 
          { puts("reset suit failed, buf[i] == NULL!"); reset_res = FALSE; }
          
          // refill
          base_memset_u32(ptrs1->buf[i], FILL_PAT, game->buffer_size / sizeof(u32));

     }

     // cmp bufs, calloc'd
     if(game_memcmp_debug(ptrs1->buf[0], ptrs1->buf[1]).result != 0)
     { puts("memcmp of buf0 vs buf1 failed on reset!"); reset_res = FALSE; }

     return reset_res;

}

//
b32 symmetry_runner(line2i forward, line2i backward)
{
     b32 res = FALSE;

     b32 use_gtr = !ptrs1->current_run_func; // invert, so we run gtr as 0'th current func

     wrap_plot_line(use_gtr ? LINE_GTR : LINE_SUT_BROKEN, ptrs1->buf[0], forward, ptrs1->scol);
     wrap_plot_line(use_gtr ? LINE_GTR : LINE_SUT_BROKEN, ptrs1->buf[1], backward, ptrs1->scol);

     if(game_memcmp_debug(ptrs1->buf[0], ptrs1->buf[1]).result != 0) 
     { res = FALSE; }
     else 
     { res = TRUE; }

     if(!suit_reset()) { res = FALSE; }

     return res;


}

// symmetry test
void test1_symmetry()
{
     line2i sym_forward[NUM_SYMMETRY_LINES] =
     {
          {10, 10, 50, 10},   // horizontal
          {10, 10, 10, 50},   // vertical
          {0, 0, 20, 20},     // diagonal
     
          {0, 0, 100, 30},    // x-major 
          {0, 0, 30, 100}     // y-major
     
     };

     line2i sym_backward[NUM_SYMMETRY_LINES] =
     {
          {50, 10, 10, 10},   // horizontal
          {10, 50, 10, 10},   // vertical
          {20, 20, 0, 0},     // diagonal
     
          {100, 30, 0, 0},    // x-major 
          {30, 100, 0, 0}     // y-major
     
     };

     //
     for (size_t i = 0; i < NUM_SYMMETRY_LINES; i++)
     {
          ptrs1->res_test1[i] = symmetry_runner(sym_forward[i], sym_backward[i]);
     }

     //
     ptrs1->res_ttl_test[0] = 
          ptrs1->res_test1[0] &&
          ptrs1->res_test1[1] &&
          ptrs1->res_test1[2] &&
          ptrs1->res_test1[3] &&
          ptrs1->res_test1[4] ? TRUE : FALSE;


}

// octant test
void test2_octant()
{
     line2i octant_walk[NUM_OCTANT_LINES] = 
     {
          {100, 100, 150, 120}, // Octant 0: X-Major, +dx, +dy (Shallow Right-Down)
          {100, 100, 120, 150}, // Octant 1: Y-Major, +dx, +dy (Steep Right-Down)
          {100, 100, 80,  150}, // Octant 2: Y-Major, -dx, +dy (Steep Left-Down)
          {100, 100, 50,  120}, // Octant 3: X-Major, -dx, +dy (Shallow Left-Down)
          {100, 100, 50,  80},  // Octant 4: X-Major, -dx, -dy (Shallow Left-Up)
          {100, 100, 80,  50},  // Octant 5: Y-Major, -dx, -dy (Steep Left-Up)
          {100, 100, 120, 50},  // Octant 6: Y-Major, +dx, -dy (Steep Right-Up)
          {100, 100, 150, 80}   // Octant 7: X-Major, +dx, -dy (Shallow Right-Up)
     };

     // Generate mirrored pairs for all 8 octants
     for (size_t i = 0; i < NUM_OCTANT_LINES; i++)
     {
          line2i forward = octant_walk[i];
          line2i backward = { forward.x2, forward.y2, forward.x1, forward.y1 };
          
          // Save result in res_test2
          ptrs1->res_test2[i] = symmetry_runner(forward, backward);
          
     }

     ptrs1->res_ttl_test[1] =
          ptrs1->res_test2[0] &&
          ptrs1->res_test2[1] &&
          ptrs1->res_test2[2] &&
          ptrs1->res_test2[3] &&
          ptrs1->res_test2[4] &&
          ptrs1->res_test2[5] &&
          ptrs1->res_test2[6] &&
          ptrs1->res_test2[7] ? TRUE : FALSE;



}

// cardinal
void test3_cardinaltb()
{
     line2i line_ctb[NUM_CARDINALTB_LINES] = 
     {
          {10, 10, 100, 10}, // Horizontal Right, dy = 0
          {100, 10, 10, 10}, // Horizontal Left, normalization zero delta
          {10, 10, 10,  100}, // Vertical Down, dx = 0
          {50, 50, 50,  50}, //  Singularity, i <= dx
          
          {0, 0, 2, 1},   // Slope 0.5: Exactly between Y=0 and Y=1 at X=1
          {0, 0, 1, 2},   // Slope 2.0: Exactly between X=0 and X=1 at Y=1
          {0, 0, 10, 5},  // Larger scale 0.5 slope
          {0, 0, 5, 10}   // Larger scale 2.0 slope
     };

     // Generate mirrored pairs 
     for (size_t i = 0; i < NUM_CARDINALTB_LINES; i++)
     {
          line2i forward = line_ctb[i];
          line2i backward = { forward.x2, forward.y2, forward.x1, forward.y1 };
          
          // Save result in res_test3
          ptrs1->res_test3[i] = symmetry_runner(forward, backward);
          
     }


     ptrs1->res_ttl_test[2] = 
          ptrs1->res_test3[0] &&
          ptrs1->res_test3[1] &&
          ptrs1->res_test3[2] &&
          ptrs1->res_test3[3] &&
          ptrs1->res_test3[4] &&
          ptrs1->res_test3[5] &&
          ptrs1->res_test3[6] &&
          ptrs1->res_test3[7] ? TRUE : FALSE;

}

// boundary
void test4_bs()
{
     i32 W = game->buffer_xy.x;
     i32 H = game->buffer_xy.y;


     line2i line_bs[NUM_BS_LINES] = 
     {
          {0, 0, W - 1, 0},         // Top Edge
          {W - 1, 0, W - 1, H - 1}, // Right Edge
          {0, H - 1, W - 1, H - 1}, // Bottom Edge
          {0, 0, W - 1, H - 1},     // Full Diagonal Sniper
          {0, 0, W - 1, 1},         // Ultra-Shallow Sniper
          {10, 10, 11, 11}          // 2-pixel micro-line
     };

     // Generate mirrored pairs 
     for (size_t i = 0; i < NUM_BS_LINES; i++)
     {
          line2i forward = line_bs[i];
          line2i backward = { forward.x2, forward.y2, forward.x1, forward.y1 };
          
          // Save result in res_test4
          ptrs1->res_test4[i] = symmetry_runner(forward, backward);
          
     }

     ptrs1->res_ttl_test[3] = 
          ptrs1->res_test4[0] &&
          ptrs1->res_test4[1] &&
          ptrs1->res_test4[2] &&
          ptrs1->res_test4[3] &&
          ptrs1->res_test4[4] &&
          ptrs1->res_test4[5] ? TRUE : FALSE; 





}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// dispay func
void test11_ui()
{

     //
     u32 col1 = 0x254733;
     u32 col2 = 0xffc000;

     u32 col_pass = 0x4c9e00;
     u32 col_fail = 0xFF0000;

     //
     plot_text(
          (v2i){game->buffer_xy.x / 2 - 128, 10}, 
          "-=Adventure 11: The Yield Midpoint=-", 
          col2
     );


     //
     plot_text(
          (v2i){20,50}, 
          "FUNC:",
          col2
     );

     char buf_func_name[NUM_LINE_FUNCS][32] =
     {
          "GTR", "SUT_BR"
     };

     static b32 state_line_func[NUM_LINE_FUNCS] = {0};


     for (i32 i = 0; i < NUM_LINE_FUNCS; i++)
     {
          rect2i btn_func = 
          {
               70 + i * 56, 45, 50, 16 
          };

          // over + clicked
          if(is_cursor_over(btn_func) && !state_line_func[i] && ptr_imouse->btn_left)
          {

               // 1. Force THIS button to TRUE (don't toggle it)
               ptrs1->tgl_line_func[i] = TRUE; 
               
               // 2. Force the OTHER button to FALSE
               ptrs1->tgl_line_func[!i] = FALSE; 
               
               // Lock the click state
               state_line_func[i] = TRUE;

               // res?
               ptrs1->current_run_func = i;

          }

          if (!ptr_imouse->btn_left)
          {
               // RELEASE THE LOCK: This allows the button to be pressed again next time
               state_line_func[i] = FALSE; 
          }


          // btn on color
          u32 r_col = 
               ptrs1->tgl_line_func[i] ? col2 : col1;

          u32 t_col =
               ptrs1->tgl_line_func[i] ? col1 : col2;

          plot_rect2i(btn_func, r_col);

          v2i pos_btn_name = {
               btn_func.left + 
                    (i == 0 ? 10 : 2),
               btn_func.top + 4
          };

          plot_text(pos_btn_name, buf_func_name[i], t_col);

          

     }

     char buf_tnames[NUM_TEST_NAMES][32] = 
     {
          "SYMMETRY",
          "OCTANT WALK",
          "CARDINAL & TB",
          "BOUNDARY & ST"
     };

     for (i32 i = 0; i < NUM_TEST_NAMES; i++)
     {
          v2i pos_tnames =
          {
               30, 70 + (i32)i * 16
          };
          
          plot_text(
               pos_tnames, 
               buf_tnames[i], 
               col2
          );

          u32 col_res_test = 
               ptrs1->res_ttl_test[i] == TRUE ?
                    col_pass : col_fail; 

          rect2i r_res_test = 
          {
               150, 
               70 + i * 16,
               32,
               8
          };

          plot_rect2i(r_res_test, col_res_test);

          // runs

          //
          for (i32 i1 = 0; i1 < NUM_SYMMETRY_LINES; i1++)
          {
               u32 col_res_run =
                    ptrs1->res_test1[i1] == TRUE ?
                         col_pass : col_fail;

               rect2i r_res_run =
               {
                    250 + i1 * 20,
                    70,
                    16,
                    8



               };

               plot_rect2i(r_res_run, col_res_run);
          } // 

          //
          for (i32 i2 = 0; i2 < NUM_OCTANT_LINES; i2++)
          {
               u32 col_res_run =
                    ptrs1->res_test2[i2] == TRUE ?
                         col_pass : col_fail;

               rect2i r_res_run =
               {
                    250 + i2 * 20,
                    70 + 16 * 1,
                    16,
                    8



               };

               plot_rect2i(r_res_run, col_res_run);
          } // 

          //
          for (i32 i3 = 0; i3 < NUM_CARDINALTB_LINES; i3++)
          {
               u32 col_res_run =
                    ptrs1->res_test3[i3] == TRUE ?
                         col_pass : col_fail;

               rect2i r_res_run =
               {
                    250 + i3 * 20,
                    70 + 16 * 2,
                    16,
                    8



               };

               plot_rect2i(r_res_run, col_res_run);
          } // 
          
          //
          for (i32 i4 = 0; i4 < NUM_BS_LINES; i4++)
          {
               u32 col_res_run =
                    ptrs1->res_test4[i4] == TRUE ?
                         col_pass : col_fail;

               rect2i r_res_run =
               {
                    250 + i4 * 20,
                    70 + 16 * 3,
                    16,
                    8



               };

               plot_rect2i(r_res_run, col_res_run);

          } // 
          
     }
     
     //size_t num_test_runs = NUM_SYMMETRY_LINES + NUM     


     //
     plot_text(
          (v2i){250,50}, 
          "RUNS:", 
          col2
     );

     // legend

     rect2i r_pass = { 30, 150, 16, 8 };
     rect2i r_fail = { 120, 150, 16, 8 };

     plot_rect2i(r_pass, col_pass);
     plot_rect2i(r_fail, col_fail);
     

     plot_text(
          (v2i){20,150}, 
          "     = PASS     = FAIL", 
          col2
     );

     plot_text(
          (v2i){20,190}, 
          "FUNC COMPLAINCE:", 
          col2
     );


     plot_text(
          (v2i){160,190}, 
          ptrs1->is_gold[ptrs1->current_run_func] ? "GOLD" : "FAIL",
          0xFFFFFF
     );

     plot_text(
          (v2i){20,210}, 
          "FUNC TIME, MS:",
          col2
     );

     //
     plot_text(
          (v2i){160, 210},
          ptrs1->buf_stat,
          0xFFFFFF
     );


     plot_text(
          (v2i){20,370}, 
          "PRESS ESC TO TERMINATE PROGRAMM", 
          col2
     );
     

} //



void draw_dragon(void *buf, int width, int height, v2i a, v2i b, int detail, int side) 
{


    u32 col = ptrs1->fractal_col;

    // reject left half
    i32 x_min = width / 4;

    if(a.x < x_min || b.x < x_min) { return; }

    
    if (detail == 0) 
    {
          b32 use_gtr = !ptrs1->current_run_func; // invert, so we run gtr as 0'th current func
          wrap_plot_line(use_gtr ? LINE_GTR : LINE_SUT_BROKEN, buf, (line2i){a.x, a.y, b.x, b.y}, col);
          return;
    }

    v2i mid;
    int dx = b.x - a.x;
    int dy = b.y - a.y;

    // The "magic" of the dragon curve: 
    // New point is (dx/2 - dy/2, dx/2 + dy/2) relative to point A
    // 'side' (1 or -1) determines if the fold goes left or right
    mid.x = a.x + (dx - (side * dy)) / 2;
    mid.y = a.y + (dy + (side * dx)) / 2;

    // Recurse: Note how the 'side' flips for the second call
    draw_dragon(buf, width, height, a, mid, detail - 1, 1);
    draw_dragon(buf, width, height, b, mid, detail - 1, -1);
}



void run_test_suit()
{
     test1_symmetry();
     test2_octant();
     test3_cardinaltb();
     test4_bs();

     //
     ptrs1->is_gold[ptrs1->current_run_func] =
               ptrs1->res_ttl_test[0] &&
               ptrs1->res_ttl_test[1] &&
               ptrs1->res_ttl_test[2] &&
               ptrs1->res_ttl_test[3]
          ? TRUE : FALSE;

    u32 col_comp_true = 0xffc000;
    u32 col_comp_false = 0xc0ff00;

     ptrs1->fractal_col =
          (ptrs1->is_gold[ptrs1->current_run_func] ? col_comp_true : col_comp_false);

}



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

void test11()
{
     static int last_active_index = -1; // -1 means nothing was active before

     int current_active_index = ptrs1->current_run_func;

     // 1. Find which button is currently ON
     for (int i = 0; i < NUM_LINE_FUNCS; i++) {
     if (ptrs1->tgl_line_func[i]) 
     {
          current_active_index = i;
          break; // Only care about the first one we find
     }
     }

     // 2. Logic: If a button is pressed AND it's a DIFFERENT button than last time
     if (current_active_index != -1 && current_active_index != last_active_index) 
     {
     
          run_test_suit(); // Run ONCE
          
          // 3. Update the state so it won't run again until the button index changes
          last_active_index = current_active_index;
     }

     // 4. Optional: Reset if the user turns all buttons off
     if (current_active_index == -1) {
     last_active_index = -1;
     }

     void *buffer = game->ptr_main;
     i32 width = game->buffer_xy.x;
     i32 height = game->buffer_xy.y;




//
SWSTART(&ptrs1->time_func);

     v2i p1 = {width + 200,     40};     
     v2i p2 = {width/10  + 150, 3 *height/4 + 100};  
     draw_dragon(buffer, width, height, p1, p2, 17, -1);
     
SWSTOP(&ptrs1->time_func);
     
 
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


//
b32 game_start()
{
     game->is_game_running = TRUE;

     // TODO: buffer size is static for now
     game->buffer_xy = (v2i)
     {
          ptf->clientSize.x,
          ptf->clientSize.y
     };
     game->buffer_size = (size_t)( game->buffer_xy.x * game->buffer_xy.y * (i32)sizeof(UINT32) );
     game->ptr_main = malloc(game->buffer_size);
     if (game->ptr_main == NULL) { puts("FAIL: malloc ptr_main!"); return FALSE; }

     //
     suit_prepare();
          
#if 0
          printf(
               "SYMMETRY TEST:\nst1[%d]\nst2[%d]\nst3[%d]\nst4[%d]\nst5[%d]\n",
               ptrs1->res_test1[0],
               ptrs1->res_test1[1],
               ptrs1->res_test1[2],
               ptrs1->res_test1[3],
               ptrs1->res_test1[4]
          );

          test2_octant();
          printf(
               "OCTANT WALK TEST:\nst1[%d]\nst2[%d]\nst3[%d]\nst4[%d]\nst5[%d]\nst6[%d]\nst7[%d]\nst8[%d]\n",
               ptrs1->res_test2[0],
               ptrs1->res_test2[1],
               ptrs1->res_test2[2],
               ptrs1->res_test2[3],
               ptrs1->res_test2[4],
               ptrs1->res_test2[5],
               ptrs1->res_test2[6],
               ptrs1->res_test2[7]
          );

          test3_cardinaltb();
          printf(
               "CARDINAL TIE BREAK TEST:\nst1[%d]\nst2[%d]\nst3[%d]\nst4[%d]\nst5[%d]\nst6[%d]\nst7[%d]\nst8[%d]\n",
               ptrs1->res_test3[0],
               ptrs1->res_test3[1],
               ptrs1->res_test3[2],
               ptrs1->res_test3[3],
               ptrs1->res_test3[4],
               ptrs1->res_test3[5],
               ptrs1->res_test3[6],
               ptrs1->res_test3[7]
          );

          test4_bs();
          printf(
               "BOUNDARY/SHORT TEST:\nst1[%d]\nst2[%d]\nst3[%d]\nst4[%d]\nst5[%d]\nst6[%d]\n",
               ptrs1->res_test4[0],
               ptrs1->res_test4[1],
               ptrs1->res_test4[2],
               ptrs1->res_test4[3],
               ptrs1->res_test4[4],
               ptrs1->res_test4[5]
          );
     }

    return TRUE;
#endif

     return TRUE;
}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/




void clock_that_time()
{
     #define CHECK_INTERVAL 30
     if( game->logtick % CHECK_INTERVAL == 0)
     {
          snprintf(ptrs1->buf_stat, STRING_LENGTH_SAFETY, "%.2f", ptrs1->time_func.ms);
     }

}


void logic_update()
{
     // simplest cursor pos polling
     POINT cursor_pos = {0};
    
    // 1. Get position in screen coordinates
    GetCursorPos(&cursor_pos); 
    
    // 2. Convert to client coordinates (relative to the window)
    ScreenToClient(ptf->hwndMain, &cursor_pos);

     // 3. Store the final position (it's already clamped by the screen edge)
     ptr_imouse->curr_pos.x = cursor_pos.x;
     ptr_imouse->curr_pos.y = cursor_pos.y;

     // Manual Clamping
     const int MAX_X = ptf->clientSize.x - 1;
     const int MAX_Y = ptf->clientSize.y - 1;

     if (ptr_imouse->curr_pos.x > MAX_X) ptr_imouse->curr_pos.x = MAX_X;
     if (ptr_imouse->curr_pos.x < 0) ptr_imouse->curr_pos.x = 0;

     if (ptr_imouse->curr_pos.y > MAX_Y) ptr_imouse->curr_pos.y = MAX_Y;
     if (ptr_imouse->curr_pos.y < 0) ptr_imouse->curr_pos.y = 0;

     // CURSOR BTNs
     ptr_imouse->btn_left  = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
     //ptr_imouse->btn_right = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

     // KBD
     ptr_kbd->key_w = (GetAsyncKeyState(0x57) & 0x8000) != 0;
     ptr_kbd->key_s = (GetAsyncKeyState(0x53) & 0x8000) != 0;

     ptr_kbd->key_r = (GetAsyncKeyState(0x52) & 0x8000) != 0;
     static b32 prev_key_r = FALSE;

     ptr_kbd->key_escape = (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;

     
     // prev/toggle
     if(ptr_kbd->key_r && !prev_key_r)
     {
          ptr_kbd->tgl_key_r = !ptr_kbd->tgl_key_r;
     }

     // update prev for next frame
     prev_key_r = ptr_kbd->key_r;

     //
     clock_that_time();

}
     

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/








/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


     


void render_update()
{

     cgi_fill_u32(0x254733);


     //
     test11();

     //
     test11_ui();

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


//
void game_stop()
{
     //
     game->is_game_running = FALSE;

     // free buffer, null pointer
     if (game->ptr_main) { free(game->ptr_main);   game->ptr_main = NULL;   }

     //
     for (size_t i = 0; i < NUM_BUFFERS; i++)
     {
          if(ptrs1->buf[i]) { free(ptrs1->buf[i]); ptrs1->buf[i] = NULL; }
          
     }

     
     puts("game stopped!");

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/