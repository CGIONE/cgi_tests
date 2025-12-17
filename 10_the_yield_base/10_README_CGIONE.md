
//   CGIONE
//   13.12.2025
//   CGI test 10

The Yield Base

1.
Its only logical that most of the frame time in the main loop is spend during render phase.
The logic phase orchestrate the flow of tick and perform calculations,
but the heavy lifting is left to the render.

2.
The same graphic functions can operate on many objects, draining resources.
Thus their performance demand attention, or better, testing.

3.
We start yield climbing from constructing two additional buffers.
We fill buffers with random lines using two function, 
the vanilla Bresenham's algorithm and Symmetric Double Step version.

As those buffers are static, 
before our main loop we initiate the "correctness" test,
comparing every pixel on a byte level.

If our memcmp passed with 0, 
two buffers has same data and our functions generate mathematically identical lines.

4.
Once we started the main loop,
our test suite has a selection between static and dynamic pattern,
as well as radio buttons to select the algorithm.

5.
With the good old timestop functions,
the most important observation is clear:
Our functions, beeing identical as proven earlier, 
are almost identical in terms of performance.

So in order to climb the Yield mountain higher,
the next step should be to switch from mathematical correctness to
"visually identical" functions! 

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests






