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
     void *ptr_buffer;

} gameproto;

gameproto gameOne; gameproto *game = &gameOne;


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// CGI

//
void cgi_fill_u32(u32 color)
{
     u32 *buf = (u32 *)game->ptr_buffer;
     for (size_t i = 0; i < game->buffer_size / sizeof(u32); i++)
     {
          *buf++ = color;
     }
     
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


void screen_plot_rect2i(rect2i src, u32 color)
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

     u32 *ptrS = (u32 *)game->ptr_buffer;

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


// --- Main plot_text function ---
void plot_text(v2i pos, const char* text, u32 text_color)
{
    // Screen dimensions (for clipping)
    const int buffer_width = game->buffer_xy.x;
    const int buffer_height = game->buffer_xy.y;
    
    // Pointer to the start of the buffer
    u32 *main_buffer = (u32 *)game->ptr_buffer;
    
    int current_x = pos.x;

    // 1. Iterate through each character in the string
    for (const char* p = text; *p != '\0'; p++) 
    {
        unsigned char character = (unsigned char)*p;
        
        // --- Character Lookup and Handling ---
        
        // Handle unprintable characters or replace with a fallback
        if (character < 32 || character > 126) 
        {
            character = 'X'; 
        }
        int font_index = character - 32;
        
        const uint8_t* glyph = ASCII_GLYPH_SET_8X8[font_index];
        
        // --- 3. Draw the 8x8 glyph (Row by Row) ---
        for (int row = 0; row < 8; row++) 
        {
            int target_y = pos.y + row;
            
            // OPTIMIZED Y-CLIPPING: Skip the whole row if outside vertical bounds
            if (target_y < 0 || target_y >= buffer_height) continue;

            // OPTIMIZED OFFSET: Calculate the base offset for this entire row only once
            u32 row_start_offset = (u32)(target_y * buffer_width);
            
            uint8_t row_data = glyph[row];
            
            // --- Pixel Loop (Column by Column) ---
            for (int col = 0; col < 8; col++) 
            {
                // Check if the current pixel bit is set (bit 7 is the leftmost pixel)
                if ((row_data >> (7 - col)) & 0x01) 
                {
                    int target_x = current_x + col;

                    // X-CLIPPING: Only plot if inside horizontal bounds
                    if (target_x >= 0 && target_x < buffer_width)
                    {
                        // Calculate final 1D index using pre-calculated row offset
                        main_buffer[row_start_offset + (u32)target_x] = text_color;
                    }
                }
            }
        }
        
        // 4. Advance the X position for the next character (8 pixels wide)
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
     game->ptr_buffer = malloc(game->buffer_size);
     if (game->ptr_buffer == NULL) { puts("FAIL: malloc ptr_buffer!"); return FALSE; }

    return TRUE;

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

     
     // prev/toggle
     if(ptr_kbd->key_r && !prev_key_r)
     {
          ptr_kbd->tgl_key_r = !ptr_kbd->tgl_key_r;
     }

     // update prev for next frame
     prev_key_r = ptr_kbd->key_r;

}
     

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// dispay func
void test8()
{
     ////
     i32 ind = 64;
     v2i posrrr = (v2i){game->buffer_xy.x - game->buffer_xy.x / 3 - ind, ind};

     plot_text(posrrr, "-=Adventure 08: Clock That Time=-", 0xFFFFFF);

     #define NUM_LINES 9
     static char bnames[NUM_LINES][64] = 
     {
          "LOGICAL TICK:",
          "-=all in ms=-",
          "FULL LOOP TIME:",
          "time PLATF:",
          "time LOGIC:",
          "time RENDR:",
          "Pacing time [derived]:",
          "-",
          "Toggle LOAD (key R):"
     };

     // names
     for (i32 i = 0; i < NUM_LINES; i++)
     {
          v2i pos = {posrrr.x, posrrr.y + ind / 4 + ind / 6 * i};
          plot_text(pos, bnames[i], 0x99dae0);
     }
     
     // very long buffer size to shush compiler warning
     #define STRING_LENGTH_SAFETY 512
     static char bvals[NUM_LINES][STRING_LENGTH_SAFETY] = {0};

     // update queries every xxx sec
     u64 quarter_second = TARGET_FPS / 4;
     b32 is_half_sec_elapsed = game->logtick % quarter_second == 0 ? TRUE : FALSE;

     // derived pacing time
     r64 pacing_ms = stopw_full_loop.ms - stopw_platform.ms - stopw_logic.ms - stopw_render.ms;

     if(is_half_sec_elapsed)
     {
          snprintf(bvals[0], STRING_LENGTH_SAFETY, "%llu", game->logtick);
          snprintf(bvals[2], STRING_LENGTH_SAFETY, "%.2f", stopw_full_loop.ms);
          snprintf(bvals[3], STRING_LENGTH_SAFETY, "%.2f", stopw_platform.ms);
          snprintf(bvals[4], STRING_LENGTH_SAFETY, "%.2f", stopw_logic.ms);
          snprintf(bvals[5], STRING_LENGTH_SAFETY, "%.2f", stopw_render.ms);
          snprintf(bvals[6], STRING_LENGTH_SAFETY, "%.2f", pacing_ms);
          snprintf(bvals[8], STRING_LENGTH_SAFETY, "%d", ptr_kbd->tgl_key_r);
     }

     //
     for (i32 i = 0; i < NUM_LINES; i++)
     {
          v2i pos = {posrrr.x + ind * 3, posrrr.y + ind / 4 + ind / 6 * i};
          plot_text(pos, bvals[i], 0xFFFFFFF);
     }

     // toggle load
     if(ptr_kbd->tgl_key_r)
     {
          // Bounding box 
          i32 shift1 = 16;
          const i32 X_START = shift1 * 2;
          const i32 Y_START = shift1 * 2;

          i32 X_END = game->buffer_xy.x / 2;  
          i32 Y_END = game->buffer_xy.y - shift1; 

          // The actual stride (full width of the 1D pixel array)
          u32 *ptr = (u32 *)game->ptr_buffer;
          const i32 BUFFER_STRIDE = game->buffer_xy.x; 
          
          u32 ADIV = 8;
          u32 time_offset = (u32)game->logtick / ADIV;

          // --- Pattern Constants ---

          // MAX_FRACTAL_LEVELS controls the final detail of the pattern
          #define MAX_FRACTAL_LEVELS 10

          // LEVEL_DIVISOR controls how many logticks pass before the next detail level appears
          #define LEVEL_DIVISOR 15

          // Calculate the current detail level based on logtick
          // The max_check_level goes from 0 up to MAX_FRACTAL_LEVELS
          u32 max_check_level = (u32)game->logtick / LEVEL_DIVISOR;
          if (max_check_level > MAX_FRACTAL_LEVELS) { max_check_level = MAX_FRACTAL_LEVELS; }


          // 1. Calculate the VIEW dimensions
          const u32 VIEW_WIDTH =   (u32)(X_END - X_START);
          const u32 VIEW_HEIGHT =  (u32)(Y_END - Y_START);

          // 2. Find the largest power of 2 that fits the view (to define the scaling factor)
          // Example: If min dimension is 300, LARGEST_SIZE will be 256 (2^8).
          // We initialize to 1 and double it MAX_FRACTAL_LEVELS times.
          uint32_t LARGEST_SIZE = 1;
          for (int i = 0; i < MAX_FRACTAL_LEVELS; i++) 
          {
               if (LARGEST_SIZE * 2 <= VIEW_WIDTH && LARGEST_SIZE * 2 <= VIEW_HEIGHT) { LARGEST_SIZE *= 2; } 
               else { break; }
          }
          
          // If the view is smaller than 2^7, this will be the size of the largest triangle

          for (i32 y = Y_START; y < Y_END; y++) 
          {
               u32 *row_ptr = ptr + (y * BUFFER_STRIDE); 

               for (i32 x = X_START; x < X_END; x++) 
               {
                    // 1. Normalize Coordinates & Apply Animation
                    uint32_t x_local = (u32)(x - X_START);
                    uint32_t y_local = (u32)(y - Y_START);

                    // Moving Horizontally Right
                    uint32_t x_shift = x_local - time_offset;
                    uint32_t y_shift = y_local; // No change in Y
               
                    // --- KEY FIX: Force the coordinates to wrap/tile based on the largest single pattern size ---
                    // This ensures the pattern calculation doesn't repeat several times across the frame.
                    x_shift %= LARGEST_SIZE; 
                    y_shift %= LARGEST_SIZE;
                    
                    // --- Sierpinski Iteration Logic (Capped by Time) ---

                    i32 is_hole = 0; // 0 = IN the Set (Solid), 1 = Hole (Background)
                    
                    // The loop is capped by the time-based level (max_check_level)
                    for (u32 i = 0; i < max_check_level; i++)
                    {
                         // Core fractal check uses the scaled coordinates
                         if ((x_shift & (1 << i)) && (y_shift & (1 << i)))
                         {
                              is_hole = 1; 
                              break;
                         }
                    }
                    
                    // --- Color Mapping (Structured Color Bands) ---
                    
                    uint32_t color = 0;
                    
                    if (is_hole == 0) 
                    {
                         // Set Color (Now based on the whole pattern size)
                         uint8_t b = 255;
                         uint8_t r = (y_local * 5) % 256; // Use y_local for smooth vertical gradient
                         uint8_t g = (x_local * 3) % 256; // Use x_local for smooth horizontal gradient

                         color = (0xFF000000 | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b);
                    } 
                    else {  color = 0; }
                    
                    // add alpha check
                    if(color != 0) { row_ptr[x] = color; }

               } // x

          } // y

     } // key r
     
}






/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


     


void render_update()
{
     // bg
     cgi_fill_u32(0x254733);



     //
     test8();

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


//
void game_stop()
{
     //
     game->is_game_running = FALSE;

     // free buffer, null pointer
     if (game->ptr_buffer) { free(game->ptr_buffer);   game->ptr_buffer = NULL;   }
     puts("game stopped!");

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/