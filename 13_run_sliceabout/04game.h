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

#define MAX_VIZ_LINES         59999
#define FILL_PAT              0x88888888

#define NUM_LINE_FUNCS        4
#define NUM_BUFFERS           2 // forward and reverse buffers

//
#define NUM_RADIO3            2


#define NUM_TEST_NAMES        5

#define NUM_SYMMETRY_LINES    5
#define NUM_OCTANT_LINES      8
#define NUM_CARDINALTB_LINES  8
#define NUM_BS_LINES          6
#define NUM_ENDMASS           9

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

// speed and slayer are somewhat equal of ms res

b32 line_bres_speed(void *buf, line2i line, u32 color) 
{
    u32 *base_ptr = (u32 *)buf;
    i32 width  = game->buffer_xy.x;
    i32 height = game->buffer_xy.y;

    // 1. TRIVIAL REJECT
    if (line.x1 < 0 || line.y1 < 0 || line.x1 >= width || line.y1 >= height ||
        line.x2 < 0 || line.y2 < 0 || line.x2 >= width || line.y2 >= height) {
        return 0;
    }

    // 2. NORMALIZATION (Critical for Symmetry Tests)
    if (line.x1 > line.x2) {
        i32 tx = line.x1; line.x1 = line.x2; line.x2 = tx;
        i32 ty = line.y1; line.y1 = line.y2; line.y2 = ty;
    }

    // 3. DELTAS (Using i32 to match GTR logic flow and avoid sign warnings)
    i32 dx = line.x2 - line.x1; 
    i32 dy_raw = line.y2 - line.y1;

    /// unsigned negation
    ///////i32 dy_abs = (dy_raw < 0) ? -dy_raw : dy_raw;
     i32 dy_abs = (dy_raw < 0) ? (i32)(0u - (u32)dy_raw) : dy_raw;


    i32 stride_y = (dy_raw >= 0) ? width : -width;
    u32 *ptr = base_ptr + (line.y1 * width + line.x1);

    // 4. X-MAJOR
    if (dx >= dy_abs) {
        // err = 2*dy - dx
        i32 err = (dy_abs << 1) - dx;
        i32 d_err_diag = (dy_abs - dx) << 1;
        i32 d_err_straight = dy_abs << 1;

        for (i32 i = 0; i <= dx; i++) {
            *ptr = color;
            i32 mask = -(err >= 0); // 0xFFFFFFFF if true, 0 if false
            
            // Branchless pointer and error update
            ptr += 1 + (stride_y & mask);
            err += d_err_straight + ((d_err_diag - d_err_straight) & mask);
        }
    } 
    // 5. Y-MAJOR
    else {
        i32 err = (dx << 1) - dy_abs;
        i32 d_err_diag = (dx - dy_abs) << 1;
        i32 d_err_straight = dx << 1;

        for (i32 i = 0; i <= dy_abs; i++) {
            *ptr = color;
            i32 mask = -(err >= 0);

            ptr += stride_y + (1 & mask);
            err += d_err_straight + ((d_err_diag - d_err_straight) & mask);
        }
    }
    return 1;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

b32 line_sut_slayer(void *buf, line2i line, u32 color) 
{
    u32 *base_ptr = (u32 *)buf;
    i32 width  = game->buffer_xy.x;
    i32 height = game->buffer_xy.y;

    if (line.x1 < 0 || line.y1 < 0 || line.x1 >= width || line.y1 >= height ||
        line.x2 < 0 || line.y2 < 0 || line.x2 >= width || line.y2 >= height) {
        return 0;
    }

    if (line.x1 > line.x2) {
        i32 tx = line.x1; line.x1 = line.x2; line.x2 = tx;
        i32 ty = line.y1; line.y1 = line.y2; line.y2 = ty;
    }

    i32 dx = line.x2 - line.x1; 
    i32 dy_raw = line.y2 - line.y1;
    i32 dy_abs = (dy_raw < 0) ? (i32)(0u - (u32)dy_raw) : dy_raw;
    i32 stride_y = (dy_raw >= 0) ? width : -width;
    u32 *ptr = base_ptr + (line.y1 * width + line.x1);

    if (dx >= dy_abs) {
        i32 err = (dy_abs << 1) - dx;
        i32 d_err_diag = (dy_abs - dx) << 1;
        i32 d_err_straight = dy_abs << 1;

        i32 count = dx + 1;
        // UNROLLING BY 4
        while (count >= 4) {
            // Pixel 1
            *ptr = color;
            i32 m = -(err >= 0);
            ptr += 1 + (stride_y & m);
            err += d_err_straight + ((d_err_diag - d_err_straight) & m);
            // Pixel 2
            *ptr = color;
            m = -(err >= 0);
            ptr += 1 + (stride_y & m);
            err += d_err_straight + ((d_err_diag - d_err_straight) & m);
            // Pixel 3
            *ptr = color;
            m = -(err >= 0);
            ptr += 1 + (stride_y & m);
            err += d_err_straight + ((d_err_diag - d_err_straight) & m);
            // Pixel 4
            *ptr = color;
            m = -(err >= 0);
            ptr += 1 + (stride_y & m);
            err += d_err_straight + ((d_err_diag - d_err_straight) & m);
            
            count -= 4;
        }
        // Cleanup remaining
        while (count--) {
            *ptr = color;
            i32 m = -(err >= 0);
            ptr += 1 + (stride_y & m);
            err += d_err_straight + ((d_err_diag - d_err_straight) & m);
        }
    } 
    else {
        // Y-Major also gets the unroll
        i32 err = (dx << 1) - dy_abs;
        i32 d_err_diag = (dx - dy_abs) << 1;
        i32 d_err_straight = dx << 1;

        i32 count = dy_abs + 1;
        while (count >= 4) {
            for(int k=0; k<4; k++) {
                *ptr = color;
                i32 m = -(err >= 0);
                ptr += stride_y + (1 & m);
                err += d_err_straight + ((d_err_diag - d_err_straight) & m);
            }
            count -= 4;
        }
        while (count--) {
            *ptr = color;
            i32 m = -(err >= 0);
            ptr += stride_y + (1 & m);
            err += d_err_straight + ((d_err_diag - d_err_straight) & m);
        }
    }
    return 1;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

//
b32 line_runslice(void *buf, line2i line, u32 color) {
    u32 *base_ptr = (u32 *)buf;
    i32 width = game->buffer_xy.x;
    i32 height = game->buffer_xy.y;

    // 1. Bounds check 
    if (line.x1 < 0 || line.y1 < 0 || line.x1 >= width || line.y1 >= height ||
        line.x2 < 0 || line.y2 < 0 || line.x2 >= width || line.y2 >= height) return 0;

    // 2. Normalize for Symmetry
    if (line.x1 > line.x2) {
        i32 tx = line.x1; line.x1 = line.x2; line.x2 = tx;
        i32 ty = line.y1; line.y1 = line.y2; line.y2 = ty;
    }

    i32 dx = line.x2 - line.x1;
    i32 dy_raw = line.y2 - line.y1;
    
    // The "Trick": Cast to u32 before negating to bypass strict-overflow checks
    u32 dy_abs = (dy_raw < 0) ? -(u32)dy_raw : (u32)dy_raw;

    // 3. Fast-Path for Cardinals
    if (dy_abs == 0) {
        u32 *p = base_ptr + ((i64)line.y1 * width + line.x1);
        for (i32 i = 0; i <= dx; i++) p[i] = color;
        return 1;
    }

    const i32 FP_SHIFT = 16;
    const i32 FP_HALF  = 0x8000;

    if ((u32)dx >= dy_abs) {
        // --- X-MAJOR ---
        i32 step_y_fp = (i32)(((i64)dy_raw << FP_SHIFT) / dx);
        i32 y_fp = (line.y1 << FP_SHIFT) + FP_HALF;

        for (i32 i = 0; i <= dx; i++) {
            base_ptr[(y_fp >> FP_SHIFT) * width + (line.x1 + i)] = color;
            y_fp += step_y_fp;
        }
    } else {
        // --- Y-MAJOR ---
        i32 step_x_fp = (i32)(((i64)dx << FP_SHIFT) / dy_abs);
        i32 x_fp = (line.x1 << FP_SHIFT) + FP_HALF;
        
        i32 y = line.y1;
        i32 y_inc = (dy_raw > 0) ? 1 : -1;

        for (u32 i = 0; i <= dy_abs; i++) {
            base_ptr[y * width + (x_fp >> FP_SHIFT)] = color;
            x_fp += step_x_fp;
            y += y_inc;
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

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

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
     LINE_GTR  = 0,
     LINE_SUT_BROKEN,
     LINE_ABR,
     LINE_SUT2
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
          } break;

          case LINE_ABR:
          {
               //
               line_runslice(buf, line, lcolor);
          } break;

          case LINE_SUT2:
          {
               //
               //line_bres_speed(buf, line, lcolor);
               line_sut_slayer(buf, line, lcolor);
          } break;

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


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



// suit
typedef struct 
{
     u32 scol;
     u32 fractal_col;
     i32 current_run_func;

     u32 num_rejected_lines;
     
     i32 current_run_radio3;
     b32 tgl_dynamic_lines[2];
     b32 tgl_num_lines[3];
     b32 tgl_radio3[NUM_RADIO3];

     b32 is_gold[NUM_LINE_FUNCS];
     b32 tgl_line_func[NUM_LINE_FUNCS];

     b32 comp_matrix[NUM_LINE_FUNCS][NUM_LINE_FUNCS];

     b32 res_ttl_test[NUM_LINE_FUNCS][NUM_TEST_NAMES];
     b32 res_test1[NUM_LINE_FUNCS][NUM_SYMMETRY_LINES]; 
     b32 res_test2[NUM_LINE_FUNCS][NUM_OCTANT_LINES];
     b32 res_test3[NUM_LINE_FUNCS][NUM_CARDINALTB_LINES];
     b32 res_test4[NUM_LINE_FUNCS][NUM_BS_LINES];
     b32 res_test5[NUM_LINE_FUNCS][NUM_ENDMASS];

     stw time_func;

     u32 col_lrand[MAX_VIZ_LINES];
     v2i pos_static_start[MAX_VIZ_LINES];
     v2i pos_static_stop[MAX_VIZ_LINES];

     line2i line_full_static[MAX_VIZ_LINES];



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
     ptrs1->tgl_radio3[0] = TRUE;
     ptrs1->tgl_dynamic_lines[0] = TRUE;
     ptrs1->tgl_num_lines[0] = TRUE; // start with MAX_VIZ_LINES / 100

     //
     srand(0x601D);

     // random value in range

     u32 col_min = 0xAA0000;
     u32 col_max = 0xFF0000;


     i32 OFFSCREEN_SHIFT = 8;
     i32 W = game->buffer_xy.x;
     i32 H = game->buffer_xy.y;
     i32 MIN_VAL = - OFFSCREEN_SHIFT;
     i32 MAX_VALX =  W + OFFSCREEN_SHIFT;
     i32 MAX_VALY = game->buffer_xy.y + OFFSCREEN_SHIFT;

     // (rand() % (MAX_VAL - MIN_VAL + 1)) + MIN_VAL;

     for (size_t i = 0; i < MAX_VIZ_LINES; i++)
     {
          ptrs1->col_lrand[i] = (u32)rand() % (col_max - col_min + 1) + col_min;

          ptrs1->pos_static_start[i] = (v2i)
          {
               rand() % (MAX_VALX - MIN_VAL + 1) + MIN_VAL,
               rand() % (MAX_VALY - MIN_VAL + 1) + MIN_VAL
          };

          ptrs1->pos_static_stop[i] = (v2i)
          {
               rand() % (MAX_VALX - MIN_VAL + 1) + MIN_VAL,
               rand() % (MAX_VALY - MIN_VAL + 1) + MIN_VAL
          };


          //
          ptrs1->line_full_static[i] = makeline_2v2i(
               ptrs1->pos_static_start[i],
               ptrs1->pos_static_stop[i]
          );

          //
          if (
             ptrs1->line_full_static[i].x1 < 0 ||
             ptrs1->line_full_static[i].y1 < 0 || 
             ptrs1->line_full_static[i].x1 >=  W || 
             ptrs1->line_full_static[i].y1 >=  H || 
             ptrs1->line_full_static[i].x2 < 0 ||
             ptrs1->line_full_static[i].y2 < 0 ||
             ptrs1->line_full_static[i].x2 >= W ||
             ptrs1->line_full_static[i].y2 >= H
          )
          { ptrs1->num_rejected_lines++; }

     }

     //
     printf("TO REJECT: %u\n", ptrs1->num_rejected_lines);
     

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
b32 symmetry_runner(i32 func_idx, line2i forward, line2i backward)
{
     // Start as TRUE, prove it FALSE
     b32 res = TRUE;

     // 1. Ensure buffers are clean before starting
     if(!suit_reset()) return FALSE;

     // 2. Plot forward on buf 0, backward on buf 1
     wrap_plot_line(func_idx, ptrs1->buf[0], forward, ptrs1->scol);
     wrap_plot_line(func_idx, ptrs1->buf[1], backward, ptrs1->scol);
     
     // 3. Compare
     if(game_memcmp_debug(ptrs1->buf[0], ptrs1->buf[1]).result != 0) 
     { res = FALSE; }

     // 4. Reset for the next test immediately
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

     for (i32 f_idx = 0; f_idx < NUM_LINE_FUNCS; f_idx++)
     {
          b32 all_lines_passed = TRUE;

          for (size_t l_idx = 0; l_idx < NUM_SYMMETRY_LINES; l_idx++)
          {
               // Test specific function against specific line
               b32 current_res = symmetry_runner(f_idx, sym_forward[l_idx], sym_backward[l_idx]);
               
               ptrs1->res_test1[f_idx][l_idx] = current_res;

               if (!current_res) all_lines_passed = FALSE;
          }

          ptrs1->res_ttl_test[f_idx][0] = all_lines_passed;
     }

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

     for (i32 f_idx = 0; f_idx < NUM_LINE_FUNCS; f_idx++)
     {
          b32 all_lines_passed = TRUE;

          for (size_t l_idx = 0; l_idx < NUM_OCTANT_LINES; l_idx++)
          {

               line2i forward = octant_walk[l_idx];
               line2i backward = { forward.x2, forward.y2, forward.x1, forward.y1 };
               b32 current_res = symmetry_runner(f_idx, forward, backward);

               
               ptrs1->res_test2[f_idx][l_idx] = current_res;

               if (!current_res) all_lines_passed = FALSE;
          }

          ptrs1->res_ttl_test[f_idx][1] = all_lines_passed;
     }
     

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


     for (i32 f_idx = 0; f_idx < NUM_LINE_FUNCS; f_idx++)
     {
          b32 all_lines_passed = TRUE;

          for (size_t l_idx = 0; l_idx < NUM_CARDINALTB_LINES; l_idx++)
          {

               line2i forward = line_ctb[l_idx];
               line2i backward = { forward.x2, forward.y2, forward.x1, forward.y1 };
               b32 current_res = symmetry_runner(f_idx, forward, backward);

               
               ptrs1->res_test3[f_idx][l_idx] = current_res;

               if (!current_res) all_lines_passed = FALSE;
          }

          ptrs1->res_ttl_test[f_idx][2] = all_lines_passed;
     }





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


     for (i32 f_idx = 0; f_idx < NUM_LINE_FUNCS; f_idx++)
     {
          b32 all_lines_passed = TRUE;

          for (size_t l_idx = 0; l_idx < NUM_BS_LINES; l_idx++)
          {

               line2i forward = line_bs[l_idx];
               line2i backward = { forward.x2, forward.y2, forward.x1, forward.y1 };
               b32 current_res = symmetry_runner(f_idx, forward, backward);

               
               ptrs1->res_test4[f_idx][l_idx] = current_res;

               if (!current_res) all_lines_passed = FALSE;
          }

          ptrs1->res_ttl_test[f_idx][3] = all_lines_passed;
     }


}




/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


// TODO: lib abs() is compiler optimized, whatever
// Requires a 32-bit signed integer (i32)
i32 fast_abs(i32 x) {
    // Determine the number of bits in i32, usually 32
    // For 32-bit integers, N = 31 (for the shift)
    const i32 N = sizeof(i32) * 8 - 1; 

    // 1. Create a Sign Mask (mask)
    // If x >= 0, mask = 0x00000000 (all zeros)
    // If x < 0, mask = 0xFFFFFFFF (all ones)
    i32 mask = x >> N; 
    
    // 2. Compute (x XOR mask)
    // If x >= 0: x XOR 0 = x
    // If x < 0: x XOR 0xFFFFFFFF = ~x (bitwise NOT, or one's complement)
    i32 result = x ^ mask;
    
    // 3. Subtract the Mask (add 1 in Two's Complement)
    // If x >= 0: result - 0 = x
    // If x < 0: (~x) - (-1) is the same as (~x) + 1, which equals -x (Two's Complement)
    result = result - mask;

    return result;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

b32 endpoint_mass_runner(i32 func_idx, line2i line)
{
    if(!suit_reset()) return FALSE;
    i32 W = game->buffer_xy.x;
    i32 H = game->buffer_xy.y;
    u32 color = ptrs1->scol;

    // 1. Plot
    wrap_plot_line(func_idx, ptrs1->buf[0], line, color);
    u32* b = (u32*)ptrs1->buf[0];

    // 2. Check Endpoints
    b32 start_hit = (b[line.y1 * W + line.x1] == color);
    b32 stop_hit  = (b[line.y2 * W + line.x2] == color);

    // 3. Check Mass (Total Pixels)
    // A line should have exactly max(dx, dy) + 1 pixels.
    i32 dx = fast_abs(line.x2 - line.x1);
    i32 dy = fast_abs(line.y2 - line.y1);
    i32 expected_mass = (dx > dy ? dx : dy) + 1;
    
    i32 actual_mass = 0;
    for (i32 i = 0; i < W * H; i++) {
        if (b[i] == color) actual_mass++;
    }

    b32 mass_hit = (actual_mass == expected_mass);

    return (start_hit && stop_hit && mass_hit);
}



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// Stress test for endpoints and pixel mass
void test5_stress_endpoints()
{
    i32 W = game->buffer_xy.x;
    i32 H = game->buffer_xy.y;

    line2i stress_lines[NUM_ENDMASS] = 
    {
        {0, 0, W - 1, 1},           // Ultra-shallow (2 runs)
        {0, 0, 1, H - 1},           // Ultra-steep (2 runs)
        {0, 0, W - 1, 2},           // 3-run X-Major
        {0, 0, 2, H - 1},           // 3-run Y-Major
        {0, 0, W - 1, H - 2},       // Almost Diagonal (X-Major)
        {0, 0, W - 2, H - 1},       // Almost Diagonal (Y-Major)
        {W / 2, H / 2, W / 2 + 1, H / 2 + 1}, // Tiny Diagonal
        {0, H - 1, W - 1, 0},       // Full Inverse Diagonal
        {W - 1, H - 1, 0, 0}        // Full Backward Diagonal
    };

     for (i32 f_idx = 0; f_idx < NUM_LINE_FUNCS; f_idx++)
     {
          b32 all_passed = TRUE;

          for (size_t l_idx = 0; l_idx < NUM_ENDMASS; l_idx++)
          {
               line2i l = stress_lines[l_idx];

               b32 current_res = endpoint_mass_runner(f_idx, l);
               
               ptrs1->res_test5[f_idx][l_idx] = current_res;

               if (!current_res) all_passed = FALSE;
          }

          ptrs1->res_ttl_test[f_idx][4] = all_passed; // Test 5 slot
     }
}




/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



void run_test_suit()
{

     test1_symmetry();
     test2_octant();
     test3_cardinaltb();
     test4_bs();

     test5_stress_endpoints();

     // compliance // gold/fail check

     
     for (size_t i = 0; i < NUM_LINE_FUNCS; i++)
     {
          ptrs1->is_gold[i] =
                    ptrs1->res_ttl_test[i][0] &&
                    ptrs1->res_ttl_test[i][1] &&
                    ptrs1->res_ttl_test[i][2] &&
                    ptrs1->res_ttl_test[i][3] &&
                    ptrs1->res_ttl_test[i][4]
               ? TRUE : FALSE;

     } // 

// 1. Initial State: Only Gold functions are candidates
for(int a=0; a < NUM_LINE_FUNCS; a++) {
    for(int b=0; b < NUM_LINE_FUNCS; b++) {
        ptrs1->comp_matrix[a][b] = (ptrs1->is_gold[a] && ptrs1->is_gold[b]);
    }
}

// 2. Double For-Loop (The Pairs)
for (i32 f1 = 0; f1 < NUM_LINE_FUNCS; f1++) {
    for (i32 f2 = f1 + 1; f2 < NUM_LINE_FUNCS; f2++) {
        
        // Skip if either isn't marked as Gold
        if (!ptrs1->comp_matrix[f1][f2]) continue;

        suit_reset(); // Reset buffers ONCE per pair

        // 3. The "Mass Brick" of Lines
        for (size_t l_idx = 0; l_idx < MAX_VIZ_LINES; l_idx++) {
            line2i test_line = makeline_2v2i(ptrs1->pos_static_start[l_idx], 
                                            ptrs1->pos_static_stop[l_idx]);
            
            // Draw all 5,999 lines into the respective buffers
            wrap_plot_line(f1, ptrs1->buf[0], test_line, ptrs1->scol);
            wrap_plot_line(f2, ptrs1->buf[1], test_line, ptrs1->scol);
        }

        // 4. Final Comparison for the whole "Image"
        if (game_memcmp_debug(ptrs1->buf[0], ptrs1->buf[1]).result != 0) {
            ptrs1->comp_matrix[f1][f2] = FALSE;
            ptrs1->comp_matrix[f2][f1] = FALSE;
            printf("[FAIL] Pair (%d, %d) did not match after %d lines.\n", f1, f2, MAX_VIZ_LINES);
        } else {
            printf("[PASS] Pair (%d, %d) is bit-identical.\n", f1, f2);
        }
    }
}







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
          "-=Adventure 13: Run-Sliceabout=-", 
          col2
     );

     v2i ltpos1 = {0, 32};

     i32 W = game->buffer_xy.x;


     rect2i rfs =
     {
          ltpos1.x, ltpos1.y, W, 16
     };



     // radio3 // prep test // compliance // draw

     static b32 state_radio3[NUM_RADIO3] = {0};

     char rad3_name[NUM_RADIO3][32] = 
     {
          "COMPLIANCE",
          "DRAW"
     };

     for (i32 i = 0; i < NUM_RADIO3; i++)
     {
          
          rect2i r_btn_rad3 = 
          {
               rfs.left + i * rfs.width / NUM_RADIO3,
               rfs.top,
               rfs.width / NUM_RADIO3,
               rfs.height
          };

          // radio logic
          // over + clicked

          if(is_cursor_over(r_btn_rad3) && !state_radio3[i] && ptr_imouse->btn_left)
          {

               // 1. Iterate through ALL buttons to reset them
               for (i32 j = 0; j < NUM_RADIO3; j++)
               {
                    ptrs1->tgl_radio3[j] = FALSE;
               }

               // 2. Set the current one to TRUE
               ptrs1->tgl_radio3[i] = TRUE; 
               
               // 3. Update the functional state/index
               ptrs1->current_run_radio3 = i;

               // 4. Lock this specific button to prevent repeat triggers while held
               state_radio3[i] = TRUE;

          }

          if (!ptr_imouse->btn_left)
          {
               // RELEASE THE LOCK: This allows the button to be pressed again next time
               state_radio3[i] = FALSE; 
          } 

          // btn on color
          u32 r_col = 
               ptrs1->tgl_radio3[i] ? col2 : col1;

          u32 t_col =
               ptrs1->tgl_radio3[i] ? col1 : col2;

          plot_rect2i(r_btn_rad3, r_col);

          v2i pos_btn_name3 = {
               r_btn_rad3.left + W/5,
               r_btn_rad3.top + 4
          };

          plot_text(pos_btn_name3, rad3_name[i], t_col);

     } //

     static b32 state_line_func[NUM_LINE_FUNCS] = {0};

     char buf_func_name[NUM_LINE_FUNCS][32] =
     {
          "GTR", 
          "SUT_BROKEN",
          "ABR",
          "SUT2"
     };


     // func selection
     for (i32 i = 0; i < NUM_LINE_FUNCS; i++)
     {
          
          rect2i r_btn_func = 
          {
               rfs.left + i * rfs.width / NUM_LINE_FUNCS,
               rfs.top + 32,
               rfs.width / NUM_LINE_FUNCS,
               rfs.height
          };

          // radio logic
          // over + clicked

          if(is_cursor_over(r_btn_func) && !state_line_func[i] && ptr_imouse->btn_left)
          {

               // 1. Iterate through ALL buttons to reset them
               for (i32 j = 0; j < NUM_LINE_FUNCS; j++)
               {
                    ptrs1->tgl_line_func[j] = FALSE;
               }

               // 2. Set the current one to TRUE
               ptrs1->tgl_line_func[i] = TRUE; 
               
               // 3. Update the functional state/index
               ptrs1->current_run_func = i;

               // 4. Lock this specific button to prevent repeat triggers while held
               state_line_func[i] = TRUE;

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

          plot_rect2i(r_btn_func, r_col);

          v2i pos_btn_name = {
               r_btn_func.left + W/12,
               r_btn_func.top + 4
          };

          plot_text(pos_btn_name, buf_func_name[i], t_col);

     } //


     

     // current run from button, -> show res
     v2i pos_r3 = { 40, 120 };
     i32 lfunc = ptrs1->current_run_func;
     switch (ptrs1->current_run_radio3)
     {
          default: break;

          // prerun
          case 0: 
          {
               //

               char buf_desc3[4][32] =
               {
                    "TEST:",
                    "RUN:",
                    "STATUS:",
                    "COMPLIANCE:"
               };

               for (i32 i = 0; i < 4; i++)
               {
                    v2i pos_desc3 = { pos_r3.x + W / 5 * i, pos_r3.y};
                    
                    plot_text(pos_desc3, buf_desc3[i], col2);
               }
               

               char buf_tnames[NUM_TEST_NAMES][32] = 
               {
                    "SYMMETRY",
                    "OCTANT WALK",
                    "CARDINAL & TB",
                    "BOUNDARY & ST",
                    "ENDMASS"
               };

               for (i32 i = 0; i < NUM_TEST_NAMES; i++)
               {
                    v2i pos_tnames =
                    {
                         pos_r3.x, pos_r3.y + 32 + (i32)i * 16
                    };
                    
                    plot_text(
                         pos_tnames, 
                         buf_tnames[i], 
                         col2
                    );

                    u32 col_res_test = 
                         ptrs1->res_ttl_test[lfunc][i] == TRUE ?
                              col_pass : col_fail; 

                    rect2i r_res_test = 
                    {
                         160, 
                         pos_r3.y + 32 + i * 16,
                         32,
                         8
                    };

                    plot_rect2i(r_res_test, col_res_test);
               
               } // for

               // runs

               // sym
               for (i32 i1 = 0; i1 < NUM_SYMMETRY_LINES; i1++)
               {
                    u32 col_res_run =
                         ptrs1->res_test1[lfunc][i1] == TRUE ?
                              col_pass : col_fail;

                    rect2i r_res_run =
                    {
                         222 + i1 * 20,
                         pos_r3.y + 32,
                         16,
                         8
                    };

                    plot_rect2i(r_res_run, col_res_run);
               } // 


               for (i32 i2 = 0; i2 < NUM_OCTANT_LINES; i2++)
               {
                    u32 col_res_run =
                         ptrs1->res_test2[lfunc][i2] == TRUE ?
                              col_pass : col_fail;

                    rect2i r_res_run =
                    {
                         222 + i2 * 20,
                         pos_r3.y + 32 * 1 + 16 * 1,
                         16,
                         8
                    };

                    plot_rect2i(r_res_run, col_res_run);
               } // 

               //
               for (i32 i3 = 0; i3 < NUM_CARDINALTB_LINES; i3++)
               {
                    u32 col_res_run =
                         ptrs1->res_test3[lfunc][i3] == TRUE ?
                              col_pass : col_fail;

                    rect2i r_res_run =
                    {
                         222 + i3 * 20,
                         pos_r3.y + 32 + 16 * 2,
                         16,
                         8

                    };

                    plot_rect2i(r_res_run, col_res_run);
               } // 

               //
               for (i32 i4 = 0; i4 < NUM_BS_LINES; i4++)
               {
                    u32 col_res_run =
                         ptrs1->res_test4[lfunc][i4] == TRUE ?
                              col_pass : col_fail;

                    rect2i r_res_run =
                    {
                         222 + i4 * 20,
                         pos_r3.y + 32 + 16 * 3,
                         16,
                         8
                    };

                    plot_rect2i(r_res_run, col_res_run);


               } //    

               //
               for (i32 i5 = 0; i5 < NUM_ENDMASS; i5++)
               {
                    u32 col_res_run =
                         ptrs1->res_test5[lfunc][i5] == TRUE ?
                              col_pass : col_fail;

                    rect2i r_res_run =
                    {
                         222 + i5 * 20,
                         pos_r3.y + 32 + 16 * 4,
                         16,
                         8
                    };

                    plot_rect2i(r_res_run, col_res_run);


               } //    


               // legend

               rect2i r_pass = { 30, 250, 16, 8 };
               rect2i r_fail = { 120, 250, 16, 8 };

               plot_rect2i(r_pass, col_pass);
               plot_rect2i(r_fail, col_fail);
               

               plot_text(
                    (v2i){20,250}, 
                    "     = PASS     = FAIL", 
                    col2
               );


               char buf_status[2][16] = { "GOLD", "FAIL" };


               plot_text(
                    (v2i){460,160}, 
                    ptrs1->is_gold[ptrs1->current_run_func] ? buf_status[0] : buf_status[1],
                    0xFFFFFF
               );


               // compliance 4x4

               char buf_fname[NUM_LINE_FUNCS][32] = 
               {
                    "GTR0",
                    "BRK1",
                    "ABR",
                    "SUT2"
               };

               v2i ltc = { W / 2 + W / 5, 166};
               for (i32 y = 0; y < 4; y++)
               {
                    for (i32 x = 0; x < 4; x++)
                    {
                         b32 is_diag = x == y ? TRUE : FALSE;
                         rect2i c_rect =
                         {
                              ltc.x + x * 42,
                              ltc.y + y * 42,
                              40,
                              40
                         };

                         // plus check compl pass
                         u32 rcol = 
                              is_diag ? col1 : 
                                   ptrs1->comp_matrix[x][y] == TRUE ? 
                                        col_pass : col_fail;

                         plot_rect2i(c_rect, rcol);



                    } // x
                    
                    // y == iter == 4
                    i32 iter = y;
                    v2i posn = 
                    {
                         ltc.x + 4 +  iter * 42,
                         ltc.y - 12
                    };

                    // horz
                    plot_text(posn, buf_fname[iter], col2);

                    // vert
                    v2i posn2 = 
                    {
                         ltc.x - 46,
                         ltc.y + 16 + iter * 42,
                    };

                    // horz
                    plot_text(posn2, buf_fname[iter], col2);

               } // y
               

          } break; // case 0 // prerun

          // draw
          case 1:
          {
               // 
               static b32 state_dynamic[2] = {0};
               char buf_name2[2][32] = { "STATIC", "DYNAMIC" };
               for (i32 i = 0; i < 2; i++)
               {
                    rect2i r_btn_dyn = 
                    {
                         rfs.left + i * rfs.width / 2,
                         rfs.top + 64,
                         rfs.width / 2,
                         rfs.height
                    };

                    // radio logic
                    // over + clicked

                    if(is_cursor_over(r_btn_dyn) && !state_dynamic[i] && ptr_imouse->btn_left)
                    {

                         // 1. Iterate through ALL buttons to reset them
                         for (i32 j = 0; j < 2; j++)
                         {
                              ptrs1->tgl_dynamic_lines[j] = FALSE;
                         }

                         // 2. Set the current one to TRUE
                         ptrs1->tgl_dynamic_lines[i] = TRUE; 
                         

                         // 4. Lock this specific button to prevent repeat triggers while held
                         state_dynamic[i] = TRUE;

                    }

                    if (!ptr_imouse->btn_left)
                    {
                         // RELEASE THE LOCK: This allows the button to be pressed again next time
                         state_dynamic[i] = FALSE; 
                    } 

                    // btn on color
                    u32 r_col = 
                         ptrs1->tgl_dynamic_lines[i] ? col2 : col1;

                    u32 t_col =
                         ptrs1->tgl_dynamic_lines[i] ? col1 : col2;

                    plot_rect2i(r_btn_dyn, r_col);

                    v2i pos_btn_name2 = {
                         r_btn_dyn.left + W/12,
                         r_btn_dyn.top + 4
                    };

                    plot_text(pos_btn_name2, buf_name2[i], t_col);

                    
               } // dyn btn
               

               //
               plot_text(
                    (v2i){20, 210},
                    ptrs1->buf_stat,
                    0xFFFFFF
               );

               // 
               static b32 state_num_lines[3] = {0};
               char buf_name3[3][32] = { "MVL / 100", "MVL / 10", "MVL(60K)" };
               for (i32 i = 0; i < 3; i++)
               {
                    rect2i r_btn_numline = 
                    {
                         rfs.left + i * rfs.width / 3,
                         rfs.top + 96,
                         rfs.width / 3,
                         rfs.height
                    };

                    // radio logic
                    // over + clicked

                    if(is_cursor_over(r_btn_numline) && !state_num_lines[i] && ptr_imouse->btn_left)
                    {

                         // 1. Iterate through ALL buttons to reset them
                         for (i32 j = 0; j < 3; j++)
                         {
                              ptrs1->tgl_num_lines[j] = FALSE;
                         }

                         // 2. Set the current one to TRUE
                         ptrs1->tgl_num_lines[i] = TRUE; 
                         

                         // 4. Lock this specific button to prevent repeat triggers while held
                         state_num_lines[i] = TRUE;

                    }

                    if (!ptr_imouse->btn_left)
                    {
                         // RELEASE THE LOCK: This allows the button to be pressed again next time
                         state_num_lines[i] = FALSE; 
                    } 

                    // btn on color
                    u32 r_col = 
                         ptrs1->tgl_num_lines[i] ? col2 : col1;

                    u32 t_col =
                         ptrs1->tgl_num_lines[i] ? col1 : col2;

                    plot_rect2i(r_btn_numline, r_col);

                    v2i pos_btn_name3 = {
                         r_btn_numline.left + W/12,
                         r_btn_numline.top + 4
                    };

                    plot_text(pos_btn_name3, buf_name3[i], t_col);

                    
               } // dyn btn
               

               //
               plot_text(
                    (v2i){20, 210},
                    ptrs1->buf_stat,
                    0xFFFFFF
               );


          } break; // draw case 1

     } // switch

     plot_text(
          (v2i){20,370}, 
          "PRESS ESC TO TERMINATE PROGRAMM", 
          col2
     );


     

} //





/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

void test11()
{
     // 
     i32 lfunc = ptrs1->current_run_func;

     // Assumes only one toggle is active at a time
     size_t multipliers[] = { MAX_VIZ_LINES / 100, MAX_VIZ_LINES / 10, MAX_VIZ_LINES };

     // Find the first set bit (0, 1, or 2)
     // If no bits are set, you can default to index 1 or 2
     int idx = ptrs1->tgl_num_lines[0] ? 0 : 
               ptrs1->tgl_num_lines[1] ? 1 : 2;

     size_t line_to_viz = multipliers[idx];


SWSTART(&ptrs1->time_func);

     if(ptrs1->tgl_dynamic_lines[1])
     {
          for (size_t i = 0; i < line_to_viz; i++)
          {
               line2i lineR = makeline_2v2i(
                    ptrs1->pos_static_start[i],
                    ptr_imouse->curr_pos
               ); 

               // test pure func
               wrap_plot_line(lfunc, game->ptr_main, lineR, ptrs1->col_lrand[i]);

          } // for

     }
     else
     {

          for (size_t i = 0; i < line_to_viz; i++)
          {
               // test pure func
               wrap_plot_line(lfunc, game->ptr_main, ptrs1->line_full_static[i], ptrs1->col_lrand[i]);

          } // for


     } // static
     


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

     //
     run_test_suit();
          

     return TRUE;
}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/




void clock_that_time()
{
     #define CHECK_INTERVAL 30
     if( game->logtick % CHECK_INTERVAL == 0)
     {
          snprintf(ptrs1->buf_stat, STRING_LENGTH_SAFETY, "FUNC, MS: %.3f", ptrs1->time_func.ms);
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
     //
     if(ptrs1->tgl_radio3[1])
     {
          test11();
     }

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