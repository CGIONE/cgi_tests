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
#include "03platform.h"

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// GAME DEFINEs

// --- Time Constants (Define your fixed logic rate) ---
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

// graphic funcs
void screen_plot_rect2i(rect2i src, u32 color)
{
    // 1. Define readable local vars for clarity
    // Assuming: src.left=X, src.top=Y, src.right=WIDTH, src.bottom=HEIGHT
    i32 x = src.left;
    i32 y = src.top;
    i32 w = src.right;
    i32 h = src.bottom;

    // 2. Early Exit: Valid Dimensions?
    if (w <= 0 || h <= 0) return;

    // 3. Early Exit: Completely Off-screen?
    if (x >= game->buffer_xy.x || y >= game->buffer_xy.y) return;
    if (x + w <= 0 || y + h <= 0) return;

    // 4. Clip Left (Crop, don't slide)
    if (x < 0) 
    {
        w += x; // Reduce width by the amount we are off-screen (x is negative)
        x = 0;  // Clamp start to 0
    }

    // 5. Clip Top (Crop, don't slide)
    if (y < 0) 
    {
        h += y; // Reduce height
        y = 0;  // Clamp start to 0
    }

    // 6. Clip Right
    if (x + w > game->buffer_xy.x) 
    {
        w = game->buffer_xy.x - x;
    }

    // 7. Clip Bottom
    if (y + h > game->buffer_xy.y) 
    {
        h = game->buffer_xy.y - y;
    }
    
    // Safety check after clipping (in case clipping resulted in 0 width/height)
    if (w <= 0 || h <= 0) return;

    // ---------------------------------------------------------
    // DRAWING
    // ---------------------------------------------------------

    u32 *ptr = (u32 *)game->ptr_buffer;
    u32 screen_w = (u32)game->buffer_xy.x;

    // Advance pointer to the starting Top-Left pixel
    ptr += (y * screen_w) + x;

    // Pre-calculate the jump to get from end of row to start of next row
    // (Screen Width - Rect Width)
    i32 stride = screen_w - w;

    for (i32 row = 0; row < h; ++row)
    {
        // Fill the row
        // Optimization: For very wide rects, memset can be faster, 
        // but for general UI/Game logic, this loop is perfectly fine.
        for (i32 col = 0; col < w; ++col) 
        { 
            *ptr++ = color; 
        }

        // Jump to next line
        ptr += stride;
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
            u32 row_start_offset = target_y * buffer_width;
            
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
                        main_buffer[row_start_offset + target_x] = text_color;
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

//
b32 game_start()
{
     game->is_game_running = TRUE;

     // TODO: buffer size is static for now
     game->buffer_xy = (v2i){
          ptf->rectMain.right,
          ptf->rectMain.bottom
     };
     game->buffer_size = game->buffer_xy.x * game->buffer_xy.y * sizeof(UINT32);
     game->ptr_buffer = malloc(game->buffer_size);
    if (game->ptr_buffer == NULL) { puts("FAIL: malloc ptr_buffer!"); return FALSE; }

    return TRUE;

}

void logic_update()
{

     // TODO: buffer size is static for now // with dynamic buffer need update buffer size

    // simple xor pattern + logtick offset

     u32 *ptr = (u32 *)game->ptr_buffer;

     //
     u32 timeN = game->logtick / 3;

     u32 width = game->buffer_xy.x;
     u32 height = game->buffer_xy.y;

     //
     u32 report_theta = 0;

     // 1. OUTER LOOP (Rows / Y)
     for (u32 y = 0; y < height; y++)
     {
          // 2. INNER LOOP (Columns / X)
          for (u32 x = 0; x < width / 2; x++)
          {
                    u32 mix1 = x ^ y;
                    u32 mix2 = x * y;
                    u32 premix1 = mix1 - timeN;
                    u32 premix2 = mix2 / (timeN + 1); 
                    report_theta = premix1 - premix2;

                    u32 fcol = make_u32ch
                    (
                         0,
                         255 - (u8)report_theta,
                         255 - (u8)report_theta,
                         255
                    );

                    *ptr++ = fcol;

          }

          // stride
          ptr += (width - width / 2);

          
     }

     //
     u32 rcol = 0x001414;
     rect2i rr1 = { width / 2, 0, width, height};
     screen_plot_rect2i(rr1, rcol);

     //
     v2i shift = {width / 6, 32};
     v2i pos2 = (v2i){ width / 2 + shift.x, shift.y};
     u32 col2 = 0xFFFFFF;
     plot_text(pos2, "-=CGI1 Flow Debug=-", col2);


     #define REPORT16 16

     char rnam[REPORT16][REPORT16] =
     {
          "System Info",
          "CPU:",
          "RAM:",
          "DISPLAY:",
          "LOGIC_FPS_US:",
          "-",
          "Pattern Info",
          "BUFFER_W:",
          "BUFFER_H:",
          "LOGTICK:",
          "THETA:",
          "-"
     };

     char rval[REPORT16][REPORT16] = {};

     sprintf(rval[1], "%d", ptf->num_log_cores);
     sprintf(rval[2], "%u", ptf->total_mem_mb);
     sprintf(rval[3], "%d x %d", ptf->monitorSize.x, ptf->monitorSize.y);
     sprintf(rval[4], "%d", TARGET_FPS_US);

     sprintf(rval[7], "%d", width);
     sprintf(rval[8], "%d", height);
     sprintf(rval[9], "%llu", game->logtick);
     sprintf(rval[10], "%u", report_theta);

     for (size_t i = 0; i < REPORT16; i++)
     {
          v2i posn = {width / 2 + shift.x / 4, shift.y * 2 + i * shift.y / 2};
          u32 coln = 0xAAAA00;
          plot_text(posn, rnam[i], coln);

          v2i posv = {width / 2 + shift.x + shift.x / 4, shift.y * 2 + i * shift.y / 2};
          u32 colv = 0xCCCCCC;
          plot_text(posv, rval[i], colv);
     }
     




}
     



     


void render_update()
{

}

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