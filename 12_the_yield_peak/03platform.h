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
#define PTF_NAME "12_THE_YIELD_PEAK"

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

// platform 
typedef struct t_platform
{
     //
     BOOL isAppRunning;

     UINT64 PerfFreq;
     UINT64 TIME_LAST_US;
     UINT64 TIME_ACCUM;

     LPCSTR MainWndClass;
     HWND hwndMain;
     HDC hdcMain;
     RECT rectMain;

     POINT monitorSize;
     POINT startSize;

     //
     POINT clientSize;

     //
     HMODULE hLibGdi;



} t_platform_proto;

t_platform_proto platform;
t_platform_proto *ptf = &platform;


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// TIMESTAMP
uint64_t ptf_timestamp_us()
{
     LARGE_INTEGER counter;
     QueryPerformanceCounter(&counter);

     return (uint64_t)(
         (long double)counter.QuadPart * 1000000.0L /
         (long double)ptf->PerfFreq);
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// STOPWATCH

typedef struct _stopwatch
{
     u64 begin;
     u64 end;
     u64 diff;
     r64 ms;
} stw;

#define debug_stw(src)                                                                    \
     printf("\nswatch [%s]\nbegin  [%llu]\nend    [%llu]\ndiff   [%llu]\nms     [%.3f]\n\n", \
            #src, src.begin, src.end, src.diff, src.ms);

void SWSTART(stw *src) { src->begin = ptf_timestamp_us(); }

void SWSTOP(stw *src)
{
     src->end = ptf_timestamp_us();
     src->diff = src->end - src->begin;
     src->ms = (r64)src->diff / (r64)1000.f;
}

// actual structs
stw stopw_full_loop;
stw stopw_platform;
stw stopw_logic;
stw stopw_render;


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// win proc forward decl
LRESULT CALLBACK MainWProc(HWND, UINT, WPARAM, LPARAM);


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"

typedef INT_PTR(WINAPI *GDI_StretchDIBits)(HDC hdc,int xDest,int yDest,int DestWidth,int DestHeight,int xSrc,int ySrc,int SrcWidth,int SrcHeight,CONST VOID *lpBits,CONST BITMAPINFO *lpbmi,UINT iUsage,DWORD rop);
GDI_StretchDIBits FN_StretchDIBits;

typedef INT_PTR(WINAPI *GDI_CreateRectRgn)(int x1,int y1,int x2,int y2);
GDI_CreateRectRgn FN_CreateRectRgn;




#pragma GCC diagnostic pop

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

BOOL platformWindow()
{
     //
     ptf->isAppRunning = TRUE;

     // load libs before window creating, so upon wm_create we already have region func

     // load lib funcs
     ptf->hLibGdi = LoadLibraryW(L"GDI32.dll");
          if (!ptf->hLibGdi) { puts("fail LoadLib: GDI32.dll!"); return FALSE; }


//// load gdi funcs // insanity or false positive or whatever, supress

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"

     FN_StretchDIBits = (GDI_StretchDIBits)GetProcAddress(ptf->hLibGdi, "StretchDIBits");
     if (FN_StretchDIBits == NULL)
     {
          w32terminal_log(TERR, "FN_StretchDIBits");
          return FALSE;
     }

     //
     FN_CreateRectRgn = (GDI_CreateRectRgn)GetProcAddress(ptf->hLibGdi, "CreateRectRgn");
     if (FN_CreateRectRgn == NULL)
     {
          w32terminal_log(TERR, "FN_CreateRectRgn");
          return FALSE;
     }



#pragma GCC diagnostic pop




     // register Main Window Class
     WNDCLASSEXA mainWC = {0};
     memset(&mainWC, 0, sizeof(WNDCLASSEXA));

     ptf->MainWndClass        = PTF_NAME;
     mainWC.cbSize            = sizeof(WNDCLASSEXA);
     mainWC.lpfnWndProc       = MainWProc;
     mainWC.lpszClassName     = ptf->MainWndClass;
     mainWC.hCursor           = LoadCursorA(NULL, (LPCSTR)IDC_ARROW);

     if(!RegisterClassExA(&mainWC)) { puts("fail window reg"); return FALSE; }

    // get primary monitor size and start window size, position rect at center
     ptf->monitorSize   = (POINT){GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
     ptf->startSize     = (POINT){ ptf->monitorSize.x - ptf->monitorSize.x / 3, ptf->monitorSize.y - ptf->monitorSize.y / 3};  

     ptf->rectMain = (RECT){
          (ptf->monitorSize.x - ptf->startSize.x) / 2,
          (ptf->monitorSize.y - ptf->startSize.y) / 2,
          ptf->startSize.x, 
          ptf->startSize.y 
     };

     // Create Main window
     ptf->hwndMain = CreateWindowExA(0, 
          ptf->MainWndClass, 
          ptf->MainWndClass,
          WS_OVERLAPPEDWINDOW | WS_VISIBLE,
          ptf->rectMain.left, 
          ptf->rectMain.top, 
          ptf->rectMain.right, 
          ptf->rectMain.bottom, 
          NULL, NULL, GetModuleHandle(NULL), NULL);
     //
     if (!ptf->hwndMain) { puts("Fail Main Window Create"); return FALSE; }

     //
     ptf->hdcMain = GetDC(ptf->hwndMain);
     if (ptf->hdcMain == NULL) {puts("failed to get dc!"); return FALSE; }



     // frequency is req for swatch, duh
     LARGE_INTEGER frequency;
     QueryPerformanceFrequency(&frequency);
     ptf->PerfFreq = (UINT64)frequency.QuadPart;

     // 
     ptf->TIME_LAST_US = ptf_timestamp_us();




     return TRUE;

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



void platformMSG()
{
     MSG msg = {0}; 
     if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
     { 
          if (msg.message == WM_QUIT) { return;  } 
          TranslateMessage(&msg); DispatchMessage(&msg); 
     }       
}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


//
void platformBlit(void *srcBuffer, i32 bufferX, i32 bufferY)
{
     //   int rWidth = ptf->rectMain.right;
     //   int rHeight = ptf->rectMain.bottom;

     int Width     = ptf->clientSize.x;
     int Height    = ptf->clientSize.y;
     
     BITMAPINFO structBMP;
     structBMP.bmiHeader.biSize      = sizeof(BITMAPINFOHEADER);
     structBMP.bmiHeader.biHeight    = -Height;
     structBMP.bmiHeader.biWidth     = Width;

     structBMP.bmiHeader.biPlanes         = 1;
     structBMP.bmiHeader.biBitCount       = 32;
     structBMP.bmiHeader.biCompression    = BI_RGB;



     i32 res __attribute__((unused)) = 
          (i32)FN_StretchDIBits(
              ptf->hdcMain,
              0, 0, Width, Height,
              0, 0, bufferX, bufferY,
              srcBuffer, &structBMP, DIB_RGB_COLORS, SRCCOPY
          );

}



/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


BOOL platformClose()
{
     //
     ptf->isAppRunning = FALSE;

     // release hdc
     if (ptf->hdcMain)
     {
          if (ReleaseDC(ptf->hwndMain, ptf->hdcMain) == 0)
          { puts("DID not release hdcViewer!"); return FALSE; }
     }

     // Main Window
     if (ptf->hwndMain != 0)
     {
          DestroyWindow(ptf->hwndMain);
          if (UnregisterClassA(ptf->MainWndClass, GetModuleHandle(NULL)) == 0)
          { puts("DID not Unreg Main WClass!"); return FALSE; }
     }
     else
     {
          if (UnregisterClassA(ptf->MainWndClass, GetModuleHandle(NULL)) == 0)
          { puts("DID not Unreg Main WClass!"); return FALSE; }
     }

     // gdi lib
     if (ptf->hLibGdi) { if (!FreeLibrary(ptf->hLibGdi)) { puts("FreeLib: hLibGdi!"); return FALSE; } }


     //
     puts("platform stopped!");

     return TRUE;

}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// WIN PROC
LRESULT CALLBACK MainWProc(HWND hwnd, UINT msgMain, WPARAM wParam, LPARAM lParam)
{
     switch (msgMain)
     {
          default: return DefWindowProc(hwnd, msgMain, wParam, lParam);

          // get rid of ugly dwm aliased rounded top corners
          case WM_CREATE:
          {
               // Get the window's current dimensions
               RECT rect;
               GetClientRect(hwnd, &rect);
               
               int width = rect.right - rect.left;
               int height = rect.bottom - rect.top;

               // Create a new rectangular region (no rounding)
               HRGN hRgn = (HRGN)FN_CreateRectRgn(0, 0, width, height);

               // Set this new region on the window. 
               // This tells the DWM/OS to stop clipping the window with rounded corners.
               SetWindowRgn(hwnd, hRgn, TRUE);
               
               // Note: The system takes ownership of the HRGN now, 
               // so you do NOT call DeleteObject(hRgn).

               
          } break; //




          case WM_NCCALCSIZE:
          {
               // def proc is respos for final sizing // remove non-client area
               if (wParam == TRUE) { return 0;}

               break; // break here
          } //

          case WM_NCPAINT:
          {
               // dont paint non-client area
               return 0;

          } break; //

          // TODO: case WM_NCHITTEST: 
          // we do static buffer, thus NO need to handle resize window
          // and window is not draggable, duh

          case WM_SIZE:
          { 
               // client size query upon change
               ptf->clientSize.x = (INT)LOWORD(lParam);
               ptf->clientSize.y = (INT)HIWORD(lParam);


          } break; // 

          //
          case WM_CLOSE:           
          { 
               platformClose();

          } break; // wm_close 

          case WM_DESTROY:         { w32terminal_log(TLOG, "WM_DESTROY!"); PostQuitMessage(0); } break;

          // 
          case WM_ERASEBKGND:      { return TRUE; } break;

          // restrict min window
          case WM_GETMINMAXINFO:
          {
               LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam; 
               lpMMI->ptMinTrackSize = ptf->startSize;
          } break;



     } // switch

     return 0;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/



