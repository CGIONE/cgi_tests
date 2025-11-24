
//   CGIONE
//   04.11.2025
//   CGI test 04

Tick and flow

1. From the first time I saw the epic "Handmade Hero" series,
the famous "UpdateAndRender" function puzzles me.

Progressing forward, with the ease of AI chat help,
I present the "fixed step logic" - "variable step render" main loop.

2. The fixed timestamp is a design choice that guarantees that 
logic update will happends once a frame, every frame,
regardless of hardware performance,
manifesting as a "game time".

3. Basicly the "accumulator pattern" conditionally clamps
logic updates within target step (target fps)

By constantly measuring time with win32 function QueryPerformanceCounter() and
simple arithmetic, we get the excact moment when logic updates should perform 
as well as some edge cases.


4. Its still not the full solution,
as we dont handle pacifying time correctly and 
not yet implemented rendering interpolation (thus its a stutter move).
But the point of this example to simply tick!

5.
So, once have tick, we can use it for, duh, flow!
A simple xor like pattern will now move indefinately,
based on modified logtick, resulting in game-time based value.

More advantures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests


