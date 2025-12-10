
//   CGIONE
//   11.12.2025
//   CGI test 08

Clock that Time

1.
By architectural choice, our adventures lies in the realm of "potato" computers, 
thus our target machine has somewhat decent cpu and dont use any explicit gpu calls for now.

Our objective today is to obtain a clear view of current program performance,
given our "budget" of 60 FPS or 16.67 milliseconds.

2. 
Using our platform timestamp function we derive a stopwatch,
two functions that measure time elapsed between two calls.

3.
We place stopwatches all around our main loop to estimate our total time (of full loop) and logic and render phases times.

4. 
There is one more part that needs attention, the pacing of cpu when no active load is applied.
Without the load our full loop will be executed as fast as possible, yet internal platform mechanism will pacify the cpu on its own. It is noticable in Task Manager as a high cpu and power usage.

For now the easiest solution is adding win32 function Sleep with 1 millisecond argument.
This will drastically reduce the cpu usage as observed in Task Manager.
As to the precision of the function, in reality it will probably be 10 or 15 milliseconds,
which is coarse yet appliable for current needs.

5.
With the help of input interface from previous adventure, we can apply computational load to our loop and observe the changes: our pacing time decreases and our render time increases. Voila!

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests





















