
//   CGIONE
//   25.11.2025
//   CGI test 05

The digital dread of debugging the flow

1. Once we started the main loop,
it will run perpertual until either conditional exit or an error.

The constant change in values (beeing 60 times a second) makes it
sometimes difficult to track and trace values as any project inevitably grows in complexity.

The digital dread creeps in...

So, to rephrase one of the colossal C programmer, once we have the result (the game flow),
we want the control.

2. Control starts with understanding of what exactly happening under the hood, where and why. 

First tool is a good lad, compiler's debugger.
We add the "-g3" compiler flag to bloat the executable with debug information.
We write macro that can be placed anywhere in code, place breakpoint in our IDE and
have a snapshot of the system once execution meets breakpoint. Easy.

3. Second set is "debug any value", terminal based.
We write macro that can take variable and printf its value.
Simple, yet clogs the terminal output.

4. Third set of tools is condition based. 
IF condtion is met (ex. 1 second elapsed), we can print debug info in terminal.
Useful, yet limited.

5. Forth set is screen based, prepared.
We fill char arrays with names of variables we want to trace.
Some values can be queried once, even before entering main loop.
Every frame, or every N-th frame we query the values and display on screen.

6. Fifth set is screen based, any value debug.
A function places value of a variable at screen coordinates.
Very easy and informative.

7. now, with all necessary tools avaliable,
we can query hardware information, os properties and game stats.

More advantures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests


