/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
//
//   CGIONE
//   File: 03game.h
//
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

#pragma once

//
#include "02platform.h"

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

// data

// Define the dimensions based on your pattern
#define PAT_ROWS 7
// Ensure this string length matches your longest string + 1 for null terminator
#define PAT_COLS 128 

// Your pattern data
static const char pat_cgi1[PAT_ROWS][PAT_COLS] = {
    "  _______   _______   ___   _______   __    _   _______  ",
    " |       | |       | |   | |       | |  |  | | |       | ",
    " |       | |    ___| |   | |   _   | |   |_| | |    ___| ",
    " |       | |   | __  |   | |  | |  | |       | |   |___  ",
    " |      _| |   ||  | |   | |  |_|  | |   _   | |    ___| ",
    " |     |_  |   |_| | |   | |       | |  | |  | |   |___  ",
    " |_______| |_______| |___| |_______| |__| |__| |_______| "
};

// 5 Rows, Fixed 128 Columns
const char pat_cgi2[5][PAT_COLS] = 
{
    "   __________________ __    _   __   ________    ____ _       ____  ",
    "  /_  __/  _/ ____/ //_/   / | / /  / ____/ /   / __ \\ |     / / /  ",
    "   / /  / // /   / ,<     /  |/ /  / /_  / /   / / / / | /| / / /   ",
    "  / / _/ // /___/ /| |   / /|  /  / __/ / /___/ /_/ /| |/ |/ /_/    ",
    " /_/ /___/\\____/_/ |_|  /_/ |_/  /_/   /_____/\\____/ |__/|__(_)     "
};


// bard ascii glyph plot
void draw_ascii_glyphs_2d(const char pattern[][PAT_COLS], i32 rows, i32 start_x, i32 start_y, i32 cell_w, i32 cell_h, i32 thick, u32 color)
{
    for (i32 r = 0; r < rows; r++)
    {
        // Iterate columns up to PAT_COLS or null terminator
        for (i32 c = 0; c < PAT_COLS && pattern[r][c] != '\0'; c++)
        {
            char glyph = pattern[r][c];
            
            // Calculate pixel position
            i32 px = start_x + (c * cell_w);
            i32 py = start_y + (r * cell_h);
            
            rect2i rect;

            // --- 1. Horizontal '_' (Bottom) ---
            if (glyph == '_')
            {
                rect.left   = px;
                rect.top    = py + cell_h - thick; 
                rect.right  = cell_w; 
                rect.bottom = thick;
                screen_plot_rect2i(rect, color);
            }
            // --- 2. Vertical '|' (Left) ---
            else if (glyph == '|')
            {
                rect.left   = px;
                rect.top    = py;
                rect.right  = thick;
                rect.bottom = cell_h;
                screen_plot_rect2i(rect, color);
            }
            // --- 3. Forward Slash '/' (Slope Up-Right) ---
            else if (glyph == '/')
            {
                // Interpolate small blocks to form diagonal
                for(i32 i = 0; i < cell_w; i+=2) // Step by 2 for optimization
                {
                    // Map X (0..W) -> Y (H..0)
                    i32 y_offset = cell_h - ((i * cell_h) / cell_w);
                    
                    rect.left   = px + i;
                    rect.top    = py + y_offset - thick;
                    rect.right  = 2 + thick; // slightly wider to fill gaps
                    rect.bottom = thick;
                    screen_plot_rect2i(rect, color);
                }
            }
            // --- 4. Back Slash '\' (Slope Down-Right) ---
            else if (glyph == '\\')
            {
                // Interpolate small blocks
                for(i32 i = 0; i < cell_w; i+=2)
                {
                    // Map X (0..W) -> Y (0..H)
                    i32 y_offset = (i * cell_h) / cell_w;

                    rect.left   = px + i;
                    rect.top    = py + y_offset;
                    rect.right  = 2 + thick; 
                    rect.bottom = thick;
                    screen_plot_rect2i(rect, color);
                }
            }
            // --- 5. Less Than / Comma '<' or ',' ---
            else if (glyph == '<' || glyph == ',')
            {
                // Simple small slant at bottom
                rect.left   = px;
                rect.top    = py + cell_h - (cell_h/2);
                rect.right  = thick;
                rect.bottom = cell_h/2;
                screen_plot_rect2i(rect, color);
            }
        }
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
     u32 time_offset = game->logtick / 8;

     u32 width = game->buffer_xy.x;
     u32 height = game->buffer_xy.y;
     u32 scale_factor = 6; // Adjust for pattern brightness/contrast

     // 1. OUTER LOOP (Rows / Y)
     for (u32 y = 0; y < height; y++)
     {
          // 2. INNER LOOP (Columns / X)
          for (u32 x = 0; x < width; x++)
          {
               if(x > y)
               {
                    *ptr++ = 0x131d38;
               }
               else 
               {
                    u32 pattern_val = ((x + time_offset) ^ y) * scale_factor;
                    *ptr++ = make_color_rgb(
                         0,
                         (u8)(pattern_val),
                         (u8)(pattern_val)
                    );

               }

          }
     }

     //
     i32 OFFSET128 = 128;
     v2i post = (v2i){ width - OFFSET128 * 4, OFFSET128};


     draw_ascii_glyphs_2d(pat_cgi1, PAT_ROWS, post.x, post.y, 8, 12, 4, 0xFFFFFF);
     draw_ascii_glyphs_2d(pat_cgi2, 5, post.x + post.y / 4, post.y + 96, 6, 9, 2, 0xAAAAAA);


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