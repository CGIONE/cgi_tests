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

typedef struct _game {

     b32 is_game_running;
     void *ptr_buffer;
     v2i buffer_xy;
     size_t buffer_size;

} gameproto;

gameproto gameOne; gameproto *game = &gameOne;

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// grahic funcs
//
void screen_plot_rect2i(rect2i src, u32 color)
{
    //
    u32 *ptr = (u32 *)game->ptr_buffer;

    // bound to buffer size
     if ( src.left   < 0 )                        { src.left   = 0; }
     if ( src.top    < 0 )                        { src.top    = 0; }
     if (src.right   < 0 )                        { src.right = 1;  }
     if (src.bottom  < 0 )                        { src.bottom = 1; }
     if (src.left + src.right > game->buffer_xy.x  ) { src.right  = game->buffer_xy.x - src.left; }
     if (src.top  + src.bottom > game->buffer_xy.y ) { src.bottom = game->buffer_xy.y - src.top;  }

    // fwd to rect start
    ptr += src.top * game->buffer_xy.x + src.left;

    for (i32 y = 0; y < src.bottom; ++y)
    {
        // fill line
        for (i32 x = 0; x < src.right; x++) { *ptr++ = color; }

        // next line of rect
        ptr += game->buffer_xy.x - src.right;
    }
    
}


// bard gemini generated // cpu intensive
u32 pattern1(i32 x, i32 y, float rad)
{
     i32 WIDTH = game->buffer_xy.x;
     i32 HEIGHT = game->buffer_xy.y;
     float max_radius = 10;
     // Calculate distance from image center
     float distance = sqrtf(powf(x - WIDTH / 2.0f, 2.0f) + powf(y - HEIGHT / 2.0f, 2.0f));

     // Calculate hue based on angle and distance (0-360 degrees)
     float angle = atan2f(y - HEIGHT / 2.0f, x - WIDTH / 2.0f) * 180.0f / M_PI;
     float hue = (angle + distance / max_radius * 360.0f) / 360.0f;

     // Convert hue to RGB using a basic algorithm (adjust for better results)
     uint8_t red = (uint8_t)(255.0f * (1.0f - cosf(hue * 6.0f)));
     uint8_t green = (uint8_t)(255.0f * (1.0f - cosf(hue * 6.0f + (2.0f * M_PI / 3.0f))));
     uint8_t blue = (uint8_t)(255.0f * (1.0f - cosf(hue * 6.0f + (4.0f * M_PI / 3.0f))));

     return (red << 16) | (green << 8) | blue;
     
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

void game_update()
{

     // TODO: buffer size is static for now


     // pattern
     u32 *ptr = (u32 *)game->ptr_buffer;
     for (i32 y = 0; y < game->buffer_xy.y; y++)
     {
               for (i32 x = 0; x < game->buffer_xy.x; x++)
               {
                    ptr[y * game->buffer_xy.x + x] = pattern1(x,y, 1.f);
               }

     }



     
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