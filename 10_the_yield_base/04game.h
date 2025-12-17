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

//

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

#define MAX_VIZ_LINES 5999
#define FILL_PAT 8

// very long buffer size to shush compiler warning
#define STRING_LENGTH_SAFETY 384



//
typedef struct
{
     i32 result; // 0 pass, 1 fail
    u32 fail_color_bres; // New: Color of the first differing pixel in BUF0 (Bresenham)
    u32 fail_color_abr;  // Color of the first differing pixel in BUF1 (Abrash DDA)
     v2i fail_pix_coords; //
} memcmp_result;

//
typedef struct 
{
     i32 x1;
     i32 y1;
     i32 x2;
     i32 y2;

} line2i;

//
typedef struct
{
     i32 arr_index;
     u32 color;
     line2i fline;
} fail_line;




// test suite
typedef struct 
{
     i32 num_lines_to_reject;
     i32 num_gtr_rejected;
     i32 num_rejected;
     memcmp_result res_fill; // gtr/buf0 
     fail_line prob_line; // gtr/buf0 

     u32 col_lrand[MAX_VIZ_LINES];
     v2i pos_static_start[MAX_VIZ_LINES];
     v2i pos_static_stop[MAX_VIZ_LINES];

     line2i static_lines[MAX_VIZ_LINES];

     void *buf_gtr;
     void *buf_stest;


} tsuit;

tsuit testOne = {0};
tsuit *tst1 = &testOne; 


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

// LINE

//
line2i makeline_2v2i(v2i A, v2i B) { return (line2i) { A.x, A.y, B.x, B.y }; }


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// Bresenham

// Returns 1 if line was drawn, 0 if rejected by bounds check
b32 line_bres_buf(void *buf, line2i line, u32 lcolor)
{
     u32 *base_ptr = (u32 *)buf;
     i32 width = game->buffer_xy.x;
     i32 height = game->buffer_xy.y;

     // --- 1. PRE-CHECK Bounds (Trivial Reject) ---
     if (line.x1 < 0 || line.y1 < 0 || line.x2 < 0 || line.y2 < 0 ||
         line.x1 >= width || line.x2 >= width || line.y1 >= height || line.y2 >= height)
     {
          return 0; 
     }

     // --- 2. NORMALIZATION ---
     // Ensure P1 (x1, y1) is the left-most point (line.x1 <= line.x2)
     if (line.x1 > line.x2) {
          i32 tx = line.x1; line.x1 = line.x2; line.x2 = tx;
          i32 ty = line.y1; line.y1 = line.y2; line.y2 = ty;
     }

     // --- 3. SETUP POINTERS ---
     u32 *ptr1 = base_ptr + (line.y1 * width + line.x1); // Starts at P1
     u32 *ptr2 = base_ptr + (line.y2 * width + line.x2); // Starts at P2

     i32 dx = line.x2 - line.x1;
     i32 dy_abs = fast_abs(line.y2 - line.y1);
     i32 stride_y = (line.y1 < line.y2) ? width : -width;
     
     // --- 4. INITIATION VARIABLES ---
     i32 err1, err2; 
     i32 d_err_major, d_err_diag;
     i32 step_p1_straight, step_p2_straight; 
     i32 step_p1_diag, step_p2_diag;
     
     if (dy_abs <= dx) 
     {
          // --- X MAJOR ---
          err1 = 2 * dy_abs - dx;
          err2 = 2 * dy_abs - dx;
          
          d_err_major = 2 * dy_abs;
          d_err_diag  = 2 * (dy_abs - dx);

          // P1 steps: Always moves X+1 (Straight=X+1, Diag=X+1, Y+/-1)
          step_p1_straight = 1;
          step_p1_diag     = 1 + stride_y;
          
          // P2 steps: Always moves X-1 (Straight=X-1, Diag=X-1, Y-/+1)
          step_p2_straight = -1;
          step_p2_diag     = -1 - stride_y;
     } 
     else 
     {
          // --- Y MAJOR ---
          err1 = 2 * dx - dy_abs;
          err2 = 2 * dx - dy_abs;
          
          d_err_major = 2 * dx;
          d_err_diag  = 2 * (dx - dy_abs);
          
          dx = dy_abs; // Swap major axis for loop count

          // P1 steps: Always moves Y+/-1 (Straight=Y+/-1, Diag=Y+/-1, X+1)
          step_p1_straight = stride_y;
          step_p1_diag     = stride_y + 1;

          // P2 steps: Always moves Y-/+1 (Straight=Y-/+1, Diag=Y-/+1, X-1)
          step_p2_straight = -stride_y;
          step_p2_diag     = -stride_y - 1;
     }

     // --- 5. SYMMETRIC LOOP ---
     // Iterations is half the total number of segments.
     i32 iterations = (dx + 1) >> 1; 

     for (i32 i = 0; i < iterations; i++)
     {
          // Plot current pixels before moving
          *ptr1 = lcolor;
          *ptr2 = lcolor;

          // P1 Logic: Standard Canonical (>= 0 means Diagonal/Minor move)
          if (err1 >= 0) {
               ptr1 += step_p1_diag;
               err1 += d_err_diag;
          } else {
               ptr1 += step_p1_straight;
               err1 += d_err_major;
          }

          // P2 Logic: CORRECTED TIE-BREAKER
          // Use STRICT inequality for Diagonal step.
          // This forces P2 to take the STRAIGHT step when err2=0 (Tie),
          // preventing the under-plot caused by P2 skipping a required step.
          if (err2 > 0) { // <--- FIX IS HERE
               ptr2 += step_p2_diag;
               err2 += d_err_diag;
          } else {
               ptr2 += step_p2_straight;
               err2 += d_err_major;
          }
     }

     // --- 6. Handle Middle Pixel ---
     // If the total line length (dx+1) is odd, ptr1 is positioned correctly
     // to draw the final single center pixel.
     if ((dx + 1) & 1) {
          *ptr1 = lcolor;
     }
     
     return 1;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// BRESENHAM GROUND TRUTH REFERENCE (Single-Pointer, 8-Octant)
// FIX: Added normalization (Step 1.5) to ensure consistent drawing direction.
b32 line_bres_gtr(void *buf, line2i line, u32 color)
{
    u32 *base_ptr = (u32 *)buf;
    i32 width = game->buffer_xy.x;
    i32 height = game->buffer_xy.y;

    // --- 1. TRIVIAL REJECT BOUNDS CHECK ---
    // If ANY endpoint is outside, we reject.
    if (line.x1 < 0 || line.y1 < 0 || 
        line.x1 >= width || line.y1 >= height || 
        line.x2 < 0 || line.y2 < 0 || 
        line.x2 >= width || line.y2 >= height)
    {
        return 0;
    }

    // --- 1.5. CRITICAL FIX: NORMALIZATION ---
    // Ensure P1 is the left-most point (x1 <= x2). 
    // This simplifies the logic, guarantees a positive major step (step_x = 1), 
    // and fixes the center pixel miss for right-to-left lines.
    if (line.x1 > line.x2) {
        i32 tx = line.x1; line.x1 = line.x2; line.x2 = tx;
        i32 ty = line.y1; line.y1 = line.y2; line.y2 = ty;
    }

    // --- 2. SETUP DELTAS AND STEPS ---
    // Use the now-normalized local 'line' values
    i32 x1 = line.x1;
    i32 y1 = line.y1;
    i32 x2 = line.x2;
    i32 y2 = line.y2;
    
    // dx_abs is now just (x2 - x1) because x1 <= x2 is guaranteed.
    i32 dx_abs = x2 - x1; 
    i32 dy_abs = fast_abs(y2 - y1); // y-order is not guaranteed, still need abs.
    
    // Direction steps
    i32 step_x = 1; // Always 1 if normalized X (left-to-right)

    // Y-Stride (memory offset for a single Y step)
    i32 stride_y = (y1 < y2) ? width : -width;

    // --- 3. POINTER START ---
    u32 *ptr = base_ptr + (y1 * width + x1);

    // --- 4. X MAJOR (Octants 0, 7, 3, 4) ---
    if (dx_abs >= dy_abs)
    {
        // Initial Error
        i32 err = 2 * dy_abs - dx_abs;
        
        // Error constants
        i32 d_err_major = 2 * dy_abs;
        i32 d_err_diag = 2 * (dy_abs - dx_abs); // 2*dy - 2*dx

        // Plot dx_abs + 1 pixels
        for (i32 i = 0; i <= dx_abs; i++)
        {
            *ptr = color; // Plot the current pixel

            if (err >= 0)
            {
                // Diagonal step: Move Minor (Y)
                ptr += stride_y;
                err += d_err_diag; // Adds 2*dy - 2*dx
            }
            else
            {
                // Straight step: Update error for straight move
                err += d_err_major; // Adds 2*dy
            }
            
            // Always move Major (X)
            ptr += step_x; // Uses step_x = 1
        }
    }
    // --- 5. Y MAJOR (Octants 1, 2, 5, 6) ---
    else
    {
        // Initial Error (deltas swapped)
        i32 err = 2 * dx_abs - dy_abs; // 2*dx - dy
        
        // Error constants
        i32 d_err_major = 2 * dx_abs; // 2*dx
        i32 d_err_diag = 2 * (dx_abs - dy_abs); // 2*dx - 2*dy

        // Plot dy_abs + 1 pixels
        for (i32 i = 0; i <= dy_abs; i++)
        {
            *ptr = color; // Plot the current pixel

            if (err >= 0)
            {
                // Diagonal step: Move Minor (X)
                ptr += step_x; // Uses step_x = 1
                err += d_err_diag; // Adds 2*dx - 2*dy
            }
            else
            {
                // Straight step: Update error for straight move
                err += d_err_major; // Adds 2*dx
            }
            
            // Always move Major (Y)
            ptr += stride_y;
        }
    }

    return 1;
}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


typedef enum
{
     LINE_GTR  = 11,
     LINE_BRES
} line_type;

// test wrapper
void wrap_plot_line(i32 algo_type, line2i line, u32 lcolor)
{

     switch (algo_type)
     {
          default: break;

          case LINE_GTR:
          {
               //
               line_bres_buf(game->ptr_main, line, lcolor);

          } break; // gtr

          case LINE_BRES:
          {
               //
               line_bres_buf(game->ptr_main, line, lcolor);

          } break; // bres

     } //

}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

//
memcmp_result game_memcmp_debug(const void *ptr1, const void *ptr2, size_t size_bytes, int width)
{
    // Cast to u32 pointers for pixel-wise comparison
    const u32 *p1 = (const u32 *)ptr1;
    const u32 *p2 = (const u32 *)ptr2;

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

static inline i32 min2i32(i32 a, i32 b)
{
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

static inline i32 max2i32(i32 a, i32 b)
{
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

//
fail_line find_fail_line(memcmp_result failure_data, line2i *line_arr, u32 *color_arr, size_t max_lines)
{
    // Initialize the return structure to a default "failed" state
    fail_line result = { .arr_index = -1, .color = 0x0, .fline = {0, 0, 0, 0} };
    v2i failed_coords = failure_data.fail_pix_coords;
    
    u32 search_color;
    const char *failure_type;

    // --- SMART SEARCH COLOR SELECTION ---
    if (failure_data.fail_color_bres != FILL_PAT)
    {
        search_color = failure_data.fail_color_bres;
        failure_type = "Over-plot (1st buf mismatch)";
    }
    else if (failure_data.fail_color_abr != FILL_PAT)
    {
        search_color = failure_data.fail_color_abr; 
        failure_type = "Under-plot (1st buf miss)";
    }
    else
    {
        // Should not happen if MEMCMP RES is 1.
        printf("*** ERROR: MEMCMP failed, but both colors are background. ***\n");
        return result;
    }

    printf("\n*** Searching for Culprit Line (Color: 0x%08X) for %s ***\n", 
           search_color, failure_type);

    for (size_t i = 0; i < max_lines; i++)
    {
        // 1. Check if the line has the designated search color.
        if (color_arr[i] == search_color)
        {
            // 2. Check if the failed coordinate is within the line's bounding box.
            int min_x = min2i32(line_arr[i].x1, line_arr[i].x2);
            int max_x = max2i32(line_arr[i].x1, line_arr[i].x2);
            int min_y = min2i32(line_arr[i].y1, line_arr[i].y2);
            int max_y = max2i32(line_arr[i].y1, line_arr[i].y2);

            if (failed_coords.x >= min_x && failed_coords.x <= max_x &&
                failed_coords.y >= min_y && failed_coords.y <= max_y)
            {
                // Line L_i is the strong candidate. Populate the result struct.
                result.arr_index = (i32)i;
                result.color = search_color;
                result.fline = line_arr[i];
                
                printf("=> POSSIBLE CULPRIT LINE FOUND (Index %zu, Color/Bounding Box Match):\n", i);
                printf("   P1: (%d, %d) | P2: (%d, %d)\n", result.fline.x1, result.fline.y1, result.fline.x2, result.fline.y2);
                printf("   Failed Pixel (%d, %d) is inside bounds: (%d, %d) to (%d, %d)\n",
                       failed_coords.x, failed_coords.y, min_x, min_y, max_x, max_y);
                
                return result; 
            }
        }
    }

    printf("*** FAILED TO FIND CULPRIT LINE INDEX. The pixel may have been drawn by a line whose color was overwritten.\n");
    return result; 
}



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// 


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

// global switches to set between funcs
#define NUM_BTN 3
static b32 tgl_sw[NUM_BTN] = {0};

#define NUM_TESTS 2
#define NUM_LFUNCS NUM_TESTS

static b32 tgl_test[NUM_TESTS] = {0};
static b32 tgl_func[NUM_LFUNCS] = {0};

#define NUM_ITEMS 5
static char rval[NUM_ITEMS][STRING_LENGTH_SAFETY] = {0};

void preloop_test()
{
     // buf size should already be non zero
     game->buffer_size = (size_t)( game->buffer_xy.x * game->buffer_xy.y * (i32)sizeof(UINT32) );
     if(game->buffer_size <1) { puts("buf size fail"); }
     
     tst1->buf_stest = malloc(game->buffer_size);
     if (tst1->buf_stest == NULL) { puts("FAIL: malloc buf_stest!"); }

     // fill bufs!
     base_memset_u32(tst1->buf_stest, FILL_PAT, game->buffer_size / sizeof(u32));

     // gtr buffer
     tst1->buf_gtr = malloc(game->buffer_size);
     if (tst1->buf_gtr == NULL) { puts("FAIL: malloc buf_gtr!"); }

     // fill gtr
     base_memset_u32(tst1->buf_gtr, FILL_PAT, game->buffer_size / sizeof(u32));

     // cmp gtr to buf0
     if(game_memcmp_debug(tst1->buf_gtr, tst1->buf_stest, game->buffer_size, game->buffer_xy.x).result != 0)
     {
          puts("memcmp of gtr vs buf0 failed!");
     }

     // seed 
     u32 seed = 0xFA10AF;
     srand(seed);

     u32 col_min = 0xAA0000;
     u32 col_max = 0xFF0000;

     // random value in range
     i32 OFFSCREEN_SHIFT = 8;
     i32 MIN_VAL = - OFFSCREEN_SHIFT;
     i32 MAX_VALX = game->buffer_xy.x + OFFSCREEN_SHIFT;
     i32 MAX_VALY = game->buffer_xy.y + OFFSCREEN_SHIFT;

     // (rand() % (MAX_VAL - MIN_VAL + 1)) + MIN_VAL;
     for (size_t i = 0; i < MAX_VIZ_LINES; i++)
     {
          // pos
          tst1->pos_static_start[i] = (v2i)
          {
               rand() % (MAX_VALX - MIN_VAL + 1) + MIN_VAL,
               rand() % (MAX_VALY - MIN_VAL + 1) + MIN_VAL
          };

          tst1->pos_static_stop[i] = (v2i)
          {
               rand() % (MAX_VALX - MIN_VAL + 1) + MIN_VAL,
               rand() % (MAX_VALY - MIN_VAL + 1) + MIN_VAL
          };

          // counters
          if(
               tst1->pos_static_start[i].x < 0 ||
               tst1->pos_static_start[i].x >= game->buffer_xy.x ||
               tst1->pos_static_start[i].y < 0 ||
               tst1->pos_static_start[i].y >= game->buffer_xy.y ||

               tst1->pos_static_stop[i].x < 0 ||
               tst1->pos_static_stop[i].x >= game->buffer_xy.x ||
               tst1->pos_static_stop[i].y < 0 ||
               tst1->pos_static_stop[i].y >= game->buffer_xy.y
          )
          {
               tst1->num_lines_to_reject++;
          }

          //
          tst1->static_lines[i] = makeline_2v2i(tst1->pos_static_start[i], tst1->pos_static_stop[i]);

          // colors
          tst1->col_lrand[i] = (u32)rand() % (col_max - col_min + 1) + col_min;

          //
          if(line_bres_gtr(tst1->buf_gtr, tst1->static_lines[i], tst1->col_lrand[i]) == 0)
          {
               tst1->num_gtr_rejected++;
          }


          // buf0: Bresenham
          if(line_bres_buf(tst1->buf_stest, tst1->static_lines[i], tst1->col_lrand[i]) == 0)
          {
               tst1->num_rejected++;
          }



     }

     // stats
     printf("MAX_LINES %d\n", MAX_VIZ_LINES);
     printf("TO REJECT %d\n", tst1->num_lines_to_reject);
     printf("GTR REJECTED %d\n", tst1->num_gtr_rejected);
     printf("BUF0[B] REJECTED %d\n", tst1->num_rejected);

     // gtr vs buf0
     b32 do_single_line_test_buf0 = FALSE;
     if(tst1->num_gtr_rejected != tst1->num_rejected)
     { puts(" gtr vs buf0 mismatch!");}
     else
     {
          // cmp buffers
          tst1->res_fill = game_memcmp_debug(tst1->buf_gtr, tst1->buf_stest, game->buffer_size, game->buffer_xy.x);
          printf("MEMCMP[gtr/buf0]: %d\n", tst1->res_fill.result);

          // if we are NOT ok
          if(tst1->res_fill.result != 0)
          {
               // FIND FAILED LINE IN BUFFER 0
               tst1->prob_line = find_fail_line(
                    tst1->res_fill, tst1->static_lines, tst1->col_lrand, MAX_VIZ_LINES);

               if(tst1->prob_line.arr_index !=0)
               {
                    do_single_line_test_buf0 = TRUE;
               }


          } //

     } //

     // single line gtr/buf0
     if(do_single_line_test_buf0)
     {
          //refill
          base_memset_u32(tst1->buf_gtr, FILL_PAT, game->buffer_size / sizeof(u32));
          base_memset_u32(tst1->buf_stest, FILL_PAT, game->buffer_size / sizeof(u32));

          // cmp gtr to buf0 // fill
          if(game_memcmp_debug(tst1->buf_gtr, tst1->buf_stest, game->buffer_size, game->buffer_xy.x).result != 0)
          { puts("memcmp of gtr vs buf0, single line mode failed!"); }
          else
          {
               line_bres_gtr(tst1->buf_gtr, tst1->prob_line.fline, tst1->prob_line.color);
               line_bres_buf(tst1->buf_stest, tst1->prob_line.fline, tst1->prob_line.color);

               //
               puts("-=SINGLE LINE CMP=-");
               memcmp_result res_isolated_fail = 
                    game_memcmp_debug(
                         tst1->buf_gtr, 
                         tst1->buf_stest, 
                         game->buffer_size, 
                         game->buffer_xy.x
               );

               if (res_isolated_fail.result != 0) 
               {
                    v2i f = { res_isolated_fail.fail_pix_coords.x, res_isolated_fail.fail_pix_coords.y};
                    printf("FINAL MISMATCH PIXEL: (%d, %d)\n", f.x, f.y);
                    printf("GTR Color: 0x%X, BUF0 Color: 0x%X\n", 
                         res_isolated_fail.fail_color_bres,
                         res_isolated_fail.fail_color_abr);
               } 
               else { printf("ISOLATED TEST PASSED: MISMATCH IS EXTERNAL TO LINE\n"); }

          }
          

     } //


     // set default switches
     tgl_test[0] = TRUE; // static
     tgl_func[0] = TRUE; // gtr

}




// dispay func
void test10_ui()
{
 
     // btn
     i32 ind1 = 16;
     i32 ind2 = 32;
     rect2i rbt[NUM_BTN] = {0};

     char btn_names[NUM_BTN][32] = 
     {
          "Show Stats",
          "Toggle Load",
          "Exit"
     };

     // states of btns // switches
     static b32 state_sw[NUM_BTN] = {0};
     
     u32 col1 = 0x254733;
     u32 col2 = 0xffc000;



     for (i32 i = 0; i < NUM_BTN; i++)
     {
          rbt[i] = (rect2i){
               ind1, ind2 + ind1 + (ind2 - ind1 / 2) * i, ind2 * 3, ind1 
          };

          // over + clicked
          if(is_cursor_over(rbt[i]) && !state_sw[i] && ptr_imouse->btn_left)
          {
               tgl_sw[i] = !tgl_sw[i];
               state_sw[i] = TRUE;
          }

          if (!ptr_imouse->btn_left)
          {
               // RELEASE THE LOCK: This allows the button to be pressed again next time
               state_sw[i] = FALSE; 
          }


          u32 rcol = 
               tgl_sw[i] ? col2 : col1;

          plot_rect2i(rbt[i], rcol);

          v2i postt = 
          {
               rbt[i].left + 2,
               rbt[i].top + ind1 / 5
          };

          u32 tcol = 
               tgl_sw[i] ? col1 : col2;

          plot_text(postt, btn_names[i], tcol);

     } // for


     // buttons now control subcalls

     char rname[NUM_ITEMS][32] =
     {
          "TOTAL:",
          "Logic:",
          "Rendr:",
          "Pacing[der]:"
     };

     // btn0: Toggle stats
     if(tgl_sw[0])
     {
          for (i32 i = 0; i < NUM_ITEMS; i++)
          {
               v2i posn = {550, 64 + i * 10};
               plot_text(posn, rname[i], 0xffc000);

               v2i posv = {650, 64 + i * 10};
               plot_text(posv, rval[i], 0xffffff);
          }

     } // btn0


     //
     plot_text(
          (v2i){game->buffer_xy.x / 2 - 128, 10}, 
          "-=Adventure 10: The Yield Base=-", 
          tgl_sw[1] == TRUE ? 0xFFFFFF : col2
     );

 
     




} //


void test10()
{
     //
     rect2i tbtn[NUM_TESTS] = {0};
     rect2i fbtn[NUM_TESTS] = {0};
     static b32 state_test[NUM_TESTS] = {0};
     static b32 state_func[NUM_LFUNCS] = {0};

     char tname[NUM_TESTS][32] =
     {
          "STATIC", "DYNAMIC"
     };

     //
     char fname[NUM_LFUNCS][32] =
     {
          "GTR", "BRES"
     };

     // toggle load switch
     if(tgl_sw[1])
     {
          //
          line2i fline = {0};

          // load // try to check cond before for loop // loop unswitching
          if(tgl_test[0]) // static
          {

               // gtr
               if(tgl_func[0])
               {
                    for (size_t i = 0; i < MAX_VIZ_LINES; i++)
                    {
                         fline =  
                              makeline_2v2i(tst1->pos_static_start[i], tst1->pos_static_stop[i]);
                         wrap_plot_line(LINE_GTR, fline, tst1->col_lrand[i]);
                    }   

               }

               else // bres
               {
                    for (size_t i = 0; i < MAX_VIZ_LINES; i++)
                    {

                         fline =  
                              makeline_2v2i(tst1->pos_static_start[i], tst1->pos_static_stop[i]);
                         wrap_plot_line(LINE_BRES, fline, tst1->col_lrand[i]);
                    }

               } // else

          } // static

          else // dynamic
          {
               // gtr
               if(tgl_func[0])
               {
                    for (size_t i = 0; i < MAX_VIZ_LINES; i++)
                    {
                         fline =  
                              makeline_2v2i(tst1->pos_static_start[i], input_mouse.curr_pos);
                         wrap_plot_line(LINE_GTR, fline, tst1->col_lrand[i]);
                    }   

               }

               else // bres
               {
                    for (size_t i = 0; i < MAX_VIZ_LINES; i++)
                    {

                         fline =  
                              makeline_2v2i(tst1->pos_static_start[i], input_mouse.curr_pos);
                         wrap_plot_line(LINE_BRES, fline, tst1->col_lrand[i]);
                    }

               } // else
          } // dynamic
          

          i32 shift1 = 64;
          for (i32 i = 0; i < NUM_TESTS; i++)
          {

          // static/dynamic test btns
               tbtn[i] = (rect2i)
               {
                    shift1 * 2, 
                    shift1 - 8 + shift1 / 2 * i, 
                    shift1, 
                    shift1 / 3
               };


               //
               v2i pos_tnam =
               {
                    
                    tbtn[i].left + 4,
                    tbtn[i].top + shift1 / 8

               };

               // test radio btns
               // over + clicked
               if(is_cursor_over(tbtn[i]) && !state_test[i] && ptr_imouse->btn_left)
               {

                    // 1. Force THIS button to TRUE (don't toggle it)
                    tgl_test[i] = TRUE; 
                    
                    // 2. Force the OTHER button to FALSE
                    tgl_test[!i] = FALSE; 
                    
                    // Lock the click state
                    state_test[i] = TRUE;

               }

               if (!ptr_imouse->btn_left)
               {
                    // RELEASE THE LOCK: This allows the button to be pressed again next time
                    state_test[i] = FALSE; 
               }

          // func radio switches
               fbtn[i] = (rect2i)
               {
                    shift1 * 3 + shift1 / 3, 
                    shift1 - 8 + shift1 / 2 * i, 
                    shift1, 
                    shift1 / 3
               };

               //
               v2i pos_fnam =
               {
                    
                    fbtn[i].left + 16,
                    fbtn[i].top + shift1 / 8

               };

               // func radio btns
               // over + clicked
               if(is_cursor_over(fbtn[i]) && !state_func[i] && ptr_imouse->btn_left)
               {

                    // 1. Force THIS button to TRUE (don't toggle it)
                    tgl_func[i] = TRUE; 
                    
                    // 2. Force the OTHER button to FALSE
                    tgl_func[!i] = FALSE; 
                    
                    // Lock the click state
                    state_func[i] = TRUE;

               }

               if (!ptr_imouse->btn_left)
               {
                    // RELEASE THE LOCK: This allows the button to be pressed again next time
                    state_func[i] = FALSE; 
               }

               // 
               u32 colsw1 = 0xFFFFFF;
               u32 colsw2 = 0xAA0000;

               u32 rcol = 
                    tgl_test[i] == TRUE ? colsw1 : colsw2;

               u32 tcol = 
                    tgl_test[i] == TRUE ? colsw2 : colsw1;


               // same for func btns so dont bother with logic
               u32 rcol2 = 
                    tgl_func[i] == TRUE ? colsw1 : colsw2;

               u32 tcol2 = 
                    tgl_func[i] == TRUE ? colsw2 : colsw1;



               // test
               plot_rect2i(tbtn[i], rcol);
               plot_text(pos_tnam, tname[i], tcol);

               // func
               plot_rect2i(fbtn[i], rcol2);
               plot_text(pos_fnam, fname[i], tcol2);

          }




     } //

     

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
     preloop_test();

    return TRUE;

}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/




void clock_that_time()
{
     #define CHECK_INTERVAL 30
     if( game->logtick % CHECK_INTERVAL == 0)
     {
          snprintf(rval[0], STRING_LENGTH_SAFETY, "%.2f", stopw_full_loop.ms);
          snprintf(rval[1], STRING_LENGTH_SAFETY, "%.2f", stopw_logic.ms);
          snprintf(rval[2], STRING_LENGTH_SAFETY, "%.2f", stopw_render.ms);
          snprintf(rval[3], STRING_LENGTH_SAFETY, "%.2f", stopw_full_loop.ms - stopw_platform.ms - stopw_logic.ms - stopw_render.ms);
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
     //cgi_fill_u32(0xAAAAAA);


     //
     test10();

     //
     test10_ui();

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
     if(tst1->buf_stest) { free(tst1->buf_stest); tst1->buf_stest = NULL; }

     // free gtr
     if (tst1->buf_gtr) { free(tst1->buf_gtr);   tst1->buf_gtr = NULL;   }

     
     puts("game stopped!");

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/