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

//
b32 game_start()
{
     game->is_game_running = TRUE;

     // TODO: buffer size is static for now
     i32 WWIDTH  = ptf->rectMain.right;
     i32 WHEIGHT = ptf->rectMain.bottom;
     game->buffer_xy = (v2i){WWIDTH, WHEIGHT};
     game->buffer_size = game->buffer_xy.x * game->buffer_xy.y * sizeof(UINT32);
     game->ptr_buffer = malloc(game->buffer_size);
    if (game->ptr_buffer == NULL) { puts("FAIL: malloc ptr_buffer!"); return FALSE; }

    return TRUE;

}

void game_update()
{

     // TODO: buffer size is static for now

     //memset(game->ptr_buffer, 0xFF, game->buffer_size);
/* 
     // color
     u32 *ptr = (u32 *)game->ptr_buffer; 

     for (i32 y = 0; y < game->buffer_xy.y; y++)
     {
          for (i32 x = 0; x < game->buffer_xy.x; x++)
          {
               ptr[y * game->buffer_xy.x + x] = 0xFF0000;

          }
     }
    */

    //
     u32 *pattern = (u32 *) game->ptr_buffer;
    for (i32 i = 0; i < game->buffer_xy.x; ++i)
    {
        for (i32 j = 0; j < game->buffer_xy.y; ++j)
        {
            u32 first_c  = 0x122563;
            u32 second_c = 0x481263;
            
            u32 final_c = 
            ( tan(i) > tan(j) ) && i < j*7 && j < i*3
            ? first_c >> 2 : second_c;

            *pattern = final_c;
            ++pattern;
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