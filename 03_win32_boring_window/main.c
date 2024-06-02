/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
//
//   CGIONE
//   File: main.c
//   31.05.2024
//   CGI test 03, win32 boring window
//   Win32 app, static linear buffer
//   little endian, win32, gcc, c[99] 
//   extra flags "-O3"
//   moar info in README
//
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// recursively include everything
#include "03game.h"

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

int main()
{
     // prepare
     puts("hello from main!");
     if (!platformWindow())  { puts("fail platformWindow!"); };
     game_start();
    
     // enter cycle
     while (ptf->isAppRunning)
     {
          // conditions 
          if(game->is_game_running)
          {
               //
               platformMSG();

               //
               game_update();

               //blit
               platformBlit(game->ptr_buffer, game->buffer_xy.x, game->buffer_xy.y);

               Sleep(1);
          }


     } // da loop

     //
     game_stop();


     puts("goodbye from main!");
     return 0;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/