/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
//
//   CGIONE
//   File: 02platform.h
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
#define PTF_NAME "03_WIN32_BORING_WINDOW"

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// load gdi funcs
typedef INT_PTR(WINAPI *GDI_StretchDIBits)(HDC hdc,int xDest,int yDest,int DestWidth,int DestHeight,int xSrc,int ySrc,int SrcWidth,int SrcHeight,CONST VOID *lpBits,CONST BITMAPINFO *lpbmi,UINT iUsage,DWORD rop);

// win proc forward decl
LRESULT CALLBACK MainWProc(HWND, UINT, WPARAM, LPARAM);

// struct proto to hold platform vars
typedef struct _platform {

     //
     BOOL isAppRunning;

     //
     LPCSTR MainWndClass;
     HWND hwndMain;
     HDC hdcMain;
     RECT rectMain;

     POINT monitorSize;
     POINT startSize;


     //
     HMODULE hLibGdi;

} platform;

// actual struct and pointer to struct
platform ptfOne; platform *ptf = &ptfOne;

//
BOOL platformWindow()
{
     //
     ptf->isAppRunning = TRUE;

     // register Main Window Class
     WNDCLASSEXA mainWC = {0}; ATOM mainWCReturn; 
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

     // load lib funcs
     ptf->hLibGdi = LoadLibraryW(L"GDI32.dll");
          if (!ptf->hLibGdi) { puts("fail LoadLib: GDI32.dll!"); return FALSE; }

     return TRUE;

}

//
void platformMSG()
{
     MSG msg = {0}; 
     if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
     { 
          if (msg.message == WM_QUIT) { return;  } 
          TranslateMessage(&msg); DispatchMessage(&msg); 
     }       
}

//
void platformBlit(void *srcBuffer, i32 bufferX, i32 bufferY)
{
     //GetClientRect(ptf->hwndMain, &ptf->rectMain);

     int rWidth = ptf->rectMain.right;
     int rHeight = ptf->rectMain.bottom;
     
     BITMAPINFO structBMP;
     structBMP.bmiHeader.biSize      = sizeof(BITMAPINFOHEADER);
     structBMP.bmiHeader.biHeight    = -rHeight;
     structBMP.bmiHeader.biWidth     = rWidth;

     structBMP.bmiHeader.biPlanes         = 1;
     structBMP.bmiHeader.biBitCount       = 32;
     structBMP.bmiHeader.biCompression    = BI_RGB;


     GDI_StretchDIBits FN_StretchDIBits = (GDI_StretchDIBits)GetProcAddress(ptf->hLibGdi, "StretchDIBits");

     i32 res = FN_StretchDIBits(
              ptf->hdcMain,
              0, 0, rWidth, rHeight,
              0, 0, bufferX, bufferY,
              srcBuffer, &structBMP, DIB_RGB_COLORS, SRCCOPY
          );

}

//
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

}

// WIN PROCS
LRESULT CALLBACK MainWProc(HWND hwnd, UINT msgMain, WPARAM wParam, LPARAM lParam)
{
     switch (msgMain)
     {
          default: return DefWindowProc(hwnd, msgMain, wParam, lParam);

/* 
          case WM_PAINT:           
          {
               PAINTSTRUCT psMain = {0};
               BeginPaint(ptf->hwndMain, &psMain);
               EndPaint(ptf->hwndMain, &psMain);
          } break; // wm_paint

 */
          case WM_SIZE:            { } break;

          //
          case WM_CLOSE:           
          { 
               platformClose();

          } break; // wm_close 

          case WM_DESTROY:         { PostQuitMessage(0); } break;

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