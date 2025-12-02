/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
//
//   CGIONE
//   File: 03platform.h
//
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

#pragma once

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

//
#include "01base.h"
#include <windows.h>

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

//
#define PTF_NAME "06_FOOLPROOF_REFLECTION"

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

#define TCOLOR_ERR (FOREGROUND_RED)
#define TCOLOR_LOG (FOREGROUND_GREEN)
#define TCOLOR_DEF (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED)

enum _ptypes
{
     TERR = 100,
     TLOG = 300,
} ptypes;

void w32terminal_log(INT type, char *msg)
{
     HANDLE hSt = GetStdHandle(STD_OUTPUT_HANDLE);

     switch (type)
     {
          default:
               break;


          case TLOG:
          {
               SetConsoleTextAttribute(hSt, TCOLOR_LOG);
               WriteConsole(hSt, msg, (DWORD)base_strlen(msg), NULL, NULL);
               WriteConsole(hSt, "\n", 1, NULL, NULL);
               SetConsoleTextAttribute(hSt, TCOLOR_DEF);
          }
          break; // tlog

          case TERR:
          {
               SetConsoleTextAttribute(hSt, TCOLOR_ERR);
               WriteConsole(hSt, "FAIL: ", 6, NULL, NULL);
               WriteConsole(hSt, msg, (DWORD)base_strlen(msg), NULL, NULL);
               WriteConsole(hSt, "\n", 1, NULL, NULL);
               SetConsoleTextAttribute(hSt, TCOLOR_DEF);
          }
          break; // err

     } // switch

} //



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/