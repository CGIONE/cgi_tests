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
     void *ptr_main;
     void *ptr_fractal;

} gameproto;

gameproto gameOne; gameproto *game = &gameOne;


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




inline float fast_truncf_portable(float x)
{
    // 1. Cast the float to an integer (this performs the truncation).
    // Use long or int, assuming a 32-bit system where they are 32-bit.
    // We use (int) for safety across standard platforms.
    int i = (int)x; 
    
    // 2. Cast the truncated integer back to a float.
    return (float)i;
}




inline float fast_fabsf_portable(float x)
{
    return (x < 0.0f) ? -x : x;
}

#define FLOAT_EPSILON 0.000001f

inline float fast_floorf_safe(float x)
{
    // 1. Truncate toward zero (t = truncf(x))
    // We use the fastest, compiler-optimized method for truncation:
    float t = (float)((int)x);
    
    // 2. Check for non-integer using safe epsilon comparison:
    // This checks if the absolute difference between the original value (x)
    // and the truncated value (t) is greater than our small tolerance (epsilon).
    // The expression: fabsf(x - t) > FLOAT_EPSILON
    
    if (x < 0.0f && fast_fabsf_portable(x - t) > FLOAT_EPSILON) {
        // If x is negative and is NOT within epsilon of being an integer, 
        // subtract 1.0f to floor correctly (e.g., -2.5 -> -3.0).
        t = t - 1.0f;
    }
    return t;
}


float fast_fmod_general(float x, float y)
{
    // Calculate quotient: q = floorf(x / y)
    float quotient = fast_floorf_safe(x / y);
    
    // Remainder: r = x - (q * y)
    return x - (quotient * y);
}

inline float fast_fminf_portable(float a, float b)
{
    // If a is less than b, return a, otherwise return b.
    return (a < b) ? a : b;
}

inline float fast_fmaxf_portable(float a, float b)
{
    // If a is greater than b, return a, otherwise return b.
    return (a > b) ? a : b;
}



/**
 * @brief Calculates 1.0/sqrt(x) using the Fast Inverse Square Root algorithm.
 * WARNING: Requires IEEE 754 floats and relies on a magic number.
 * Accuracy is very high but not perfect.
 */

/**
 * @brief Fast Inverse Square Root using the magic number 0x5f3759df.
 * * This version uses a union for type punning to avoid the 
 * -Wstrict-aliasing compiler warning.
 * * @param x The input number (float)
 * @return 1.0/sqrt(x)
 */
float fast_inv_sqrt_safe(float x)
{
    // Union ensures that 'f' and 'i' share the same memory location.
    union {
        float f;
        uint32_t i; // Use uint32_t to explicitly match the size of float (32 bits)
    } u;

    float x2 = x * 0.5F;
    
    // 1. Safe Type Punning: Store float, access bits as integer
    u.f = x; 

    // 2. The "Magic Number" calculation
    // 0x5f3759df is designed for 32-bit floats, so uint32_t is the correct type.
    u.i = 0x5f3759df - ( u.i >> 1 );       
    
    float y = u.f;
    
    // 3. Newton's method for accuracy improvement
    y = y * ( 1.5F - ( x2 * y * y ) ); 
    
    return y; // Returns 1/sqrt(x)
}

/**
 * @brief Uses the fast inverse sqrt to get sqrt(x)
 */
float fast_sqrt(float x)
{
    // sqrt(x) = x * (1/sqrt(x))
    return x * fast_inv_sqrt_safe(x);
}



#define NUM_ITERATIONS 16     // detail level
#define REPEAT_FREQ    4.0f // How many times the pattern repeats horizontally/vertically

//
float hyper_geometric_pattern(float X_norm, float Y_norm)
{
    float X = X_norm * REPEAT_FREQ;
    float Y = Y_norm * REPEAT_FREQ;

    // --- Rotation (Time-based linear skew, assuming c and s are constants for one frame) ---
    // If c and s were calculated outside the pixel loop:
    // float c = FRAME_C_PRIME;
    // float s = 0.1f;
    
    float c = 0.003f;
    // + time * 0.01f;
    const float s = 0.001f; // Use const for s since it doesn't change
    
    // Rotation matrix multiply
    float X_rot = X * c - Y * s;
    float Y_rot = X * s + Y * c;
    
    X = X_rot;
    Y = Y_rot;
    
    float f = 0.0f;

    // --- Iterative Folding ---
    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        // 1. Domain Repetition (fast fmod for divisor 2.0f, then shift)
        // Original: X = fast_fmod_general(X, 2.0f) - 1.0f;
        float qX = fast_floorf_safe(X * 0.5f);
        X = X - (qX * 2.0f) - 1.0f;
        
        float qY = fast_floorf_safe(Y * 0.5f);
        Y = Y - (qY * 2.0f) - 1.0f;

        // 2. Folding (Faster fabsf)
        X = fast_fabsf_portable(X);
        Y = fast_fabsf_portable(Y);
        
        // 3. Accumulate and Scale
        // f += (X + Y) * 0.5f; --> f += (X + Y) / 2.0f;
        f += (X + Y) / 2.0f; 
        
        // Scale: X *= 2.0f; --> X += X;
        X += X; 
        Y += Y;
    }
    
    // Normalize result: fast_fmod_general(f * 0.25f, 1.0f)
    return fast_fmod_general(f / 4.0f, 1.0f);
}

#define MAX_DIST_RADIUS 575.0f // Distance in pixels where the mask fades to zero


void render_pattern_blend()
{
    // --- LOAD AND CALCULATE CONSTANTS (Outside the loops) ---
    // Frame buffer access
    u32 *frame_buffer = game->ptr_fractal; 
    i32 stride = game->buffer_xy.x;
    
    // Time and Bounds
    i32 xres = game->buffer_xy.x;
    i32 yres = game->buffer_xy.y;
     
    // Reciprocal for normalization
    float center_x = (float)xres / 2.0f;
    float center_y = (float)yres / 2.0f;
    const float INV_CENTER_X = 1.0f / center_x;
    const float INV_CENTER_Y = 1.0f / center_y;
    const float INV_MAX_DIST = 1.0f / MAX_DIST_RADIUS; 
    
    // Set initial pointer and loop bounds
    i32 start_x = 0;
    i32 start_y = 0;
    u32 *row_ptr = frame_buffer + (start_y * stride); 

    // --- MAIN LOOP ---
    for (i32 y = start_y; y < yres; y++) 
    {
        // Reset X pointer for the start of the drawing area on this row
        u32 *current_pixel_ptr = row_ptr + start_x;
        
        for (i32 x = start_x; x < xres; x++) {
            
            // 1. Normalize Coordinates (Use Reciprocal Multiply)
            float X_norm = ((float)x * INV_CENTER_X) - 1.0f;
            float Y_norm = ((float)y * INV_CENTER_Y) - 1.0f;
            
            // 2. Get Geometric Pattern Intensity (A)
            float pattern_intensity = hyper_geometric_pattern(X_norm, Y_norm);
            
            // 3. Get Blending Mask (B)
            float dx = (float)x - center_x;
            float dy = (float)y - center_y;
            
            // The Fast Inverse Square Root approach (fast_sqrt) is used here.
            float dist_from_center = fast_sqrt(dx*dx + dy*dy);
            
            // Mask fades from 1.0 to 0.0 (Use Reciprocal Multiply)
            float mask = fast_fmaxf_portable(0.0f, 1.0f - (dist_from_center * INV_MAX_DIST));

            // 4. Final Blending and Coloring
            float final_intensity = fast_fmod_general(pattern_intensity + mask * 0.5f, 1.0f);
            float effective_intensity = final_intensity * mask;

            // Clamping using the fast functions
            effective_intensity = fast_fmaxf_portable(0.05f, fast_fminf_portable(0.95f, effective_intensity));

            u32 final_color;
            if (final_intensity < 0.1f) {
                final_color = 0x254733; 
            } else {
                // Color conversion (Can be SIMD optimized, but sticking to scalar here)
                u32 R = (u32)(effective_intensity * (float)0xFF);
                u32 G = (u32)(effective_intensity * (float)0xC0);
                u32 B = (u32)(effective_intensity * (float)0x00);
                final_color = (R << 16) | (G << 8) | B;
            }
            
            // 5. Direct Write (Fastest possible pixel update)
            if (final_color > 0x254733) {
                *current_pixel_ptr = final_color;
            }
            
            // Advance the pointer to the next pixel
            current_pixel_ptr++; 
        }
        
        // Advance row_ptr by the full stride for the next row
        row_ptr += stride;
    }
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
     game->ptr_fractal = malloc(game->buffer_size);
     if (game->ptr_fractal == NULL) { puts("FAIL: malloc ptr_fractal!"); return FALSE; }

     // generate to off screen buffer
     render_pattern_blend();


    return TRUE;

}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// very long buffer size to shush compiler warning
#define STRING_LENGTH_SAFETY 512

#define NUM_ITEMS 5

static char rval[NUM_ITEMS][STRING_LENGTH_SAFETY] = {0};

void clock_that_time()
{
     if( game->logtick % 15 == 0)
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

// global switches to set between funcs
#define NUM_BTN 3
static b32 tgl_sw[NUM_BTN] = {0};



// dispay func
void test9()
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

     char btn_desc[NUM_BTN][64] = 
     {
          "Subsystems call times, in ms",
          "Show generated fractal image",
          "Terminate current programm"
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

          // show description if no btn is pressed
          if(!tgl_sw[i])
          {
               v2i posds = 
               {
                    (rbt[i].left + rbt[i].width) + 16,
                    rbt[i].top + ind1 / 5
               };

               plot_text(posds, btn_desc[i], 0xAAAAAA);
          }

          //
          
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
          "-=Adventure 9: Window Dressing=-", 
          tgl_sw[1] == TRUE ? 0xFFFFFF : col2
     );

 
     




} //






/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


     


void render_update()
{

    // second [1] : toggle load
     if(tgl_sw[1])
     {
          u32 *dst = (u32 *)game->ptr_main;
          u32 *src = (u32 *)game->ptr_fractal;
          for (size_t i = 0; i < game->buffer_size / sizeof(u32); i++)
          {
               
               *dst++ = *src++; 
          }

     }
     else // no load, redraw bg
     { cgi_fill_u32(0x254733); }


     //
     test9();

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
     if (game->ptr_fractal) { free(game->ptr_fractal);   game->ptr_fractal = NULL;   }
     puts("game stopped!");

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/