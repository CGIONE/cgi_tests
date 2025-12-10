/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
//
//   CGIONE
//   File: main.c
//   CGI test 07, Mastering Controls
//   Win32 app, static linear buffer
//   little endian, win32, gcc, c[99] 
//   extra flags "-O3"
//   moar info in README
//
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// recursively include everything
#include "04game.h"

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

int main()
{
     
w32terminal_log(TLOG, "started");

     if (!platformWindow())  { puts("fail platformWindow!"); };
     
     //
     game_start();


w32terminal_log(TLOG, "enter loop");


     // enter cycle
     while (ptf->isAppRunning)
     {

SWSTART(&stopw_full_loop);

SWSTART(&stopw_platform);

          //
          platformMSG();

SWSTOP(&stopw_platform);

          // conditions 
          if(game->is_game_running)
          {


               //
               uint64_t current_time_us = ptf_timestamp_us();
               uint64_t elapsed_time_us = current_time_us - ptf->TIME_LAST_US;
               ptf->TIME_LAST_US = current_time_us;

               // CLAMP: Prevents a massive time jump on resuming from a pause.
               const uint64_t MAX_ELAPSED_TIME_US = MAX_LOGIC_TICKS_PER_FRAME * TARGET_FPS_US;
               if (elapsed_time_us > MAX_ELAPSED_TIME_US) { elapsed_time_us = MAX_ELAPSED_TIME_US; }
               
               //
               ptf->TIME_ACCUM += elapsed_time_us;


               //
               u32 logic_ticks_executed = 0;
               // The Accumulator Loop: Drains time, running one time per TARGET_FPS_US
               while (
                    ptf->TIME_ACCUM >= TARGET_FPS_US
                    && logic_ticks_executed < MAX_LOGIC_TICKS_PER_FRAME
               ) 
               {
SWSTART(&stopw_logic);
                    //
                    logic_update();


                    // 
                    game->logtick++;
                    ptf->TIME_ACCUM -= TARGET_FPS_US;
                    logic_ticks_executed++;

SWSTOP(&stopw_logic);
               } //


SWSTART(&stopw_render);

               //
               render_update();

               //blit
               platformBlit(game->ptr_buffer, game->buffer_xy.x, game->buffer_xy.y);

SWSTOP(&stopw_render);

               // pacing
               Sleep(1);

          }

          // out of window context
          else 
          {
               ptf->TIME_LAST_US = ptf_timestamp_us();
               ptf->TIME_ACCUM = 0;
               Sleep(100);
               

          }

SWSTOP(&stopw_full_loop);

     } // da loop

     //
     game_stop();


     puts("goodbye from main!");
     return 0;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/