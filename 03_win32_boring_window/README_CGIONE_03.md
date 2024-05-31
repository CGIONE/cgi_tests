
//   CGIONE
//   31.05.2024
//   CGI test 03

wi32 the boring window

1.
In the future, the C programming language and its compiler will be so powerful, 
that the language itself will request and dictate any OS the necessary commands 
by some abstract "platform" syntax.

The programmer will be set free of any dull APIs with their rules,
as all hardware possibilities will be open and avaliable at once.

But, until that bright future comes, we are stuck with "the win32 way" to:
1) request OS to start our programm as a process, create window and
2) construct "Windows message pump" and finally see our own code on screen.

2.
The following design is set on this examples:

2.1 There is a 01base.h file, a base layer for common use
2.2 The 02platform.h file, a layer dedicated for win32 platform functions
2.3 The 03game.h, a layer of game engine prototype with knowlage of platfom.h layer
2.4 The main.c, the kernel of whole application.
2.5 .vscode folder, IDE settings files

3. 
To understand this example, lets look at main function:

3.1 Two functions are needed to prepare a) platform layer (query monitor size, make window) and b) game layer (allocate linear buffer in memory)
3.2 Once we enter the "endless" cycle of programm running loop, 
we a) query messages from platform (win32 message pump), b) update game layer (ex. produce pattern) and finally, "blit" or transfer image to screen.

At last, we have a "soft real time" programm, with a window, linear buffer and a blitter. 

More advantures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests


