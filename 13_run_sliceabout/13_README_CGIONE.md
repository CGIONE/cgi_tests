
//   CGIONE
//   22.12.2025
//   CGI test 12

Run-Sliceabout

1.
After several iteration of tests we achieved a mathematically correct line plotting function with performance at its peak.
And while being an interesting adventure, we cant simply skip investigation of the Run-Length Slice Algorithm, from mr. Michael Abrash.

The premise of this testing is the same: run through dataset forward and backward, achieve the Gold status. Simple.

But, since the underlaying mechanism of error accounting is different from Bresenham's logic, meaning searching for "horizontal runs",
the aliased line will always be constructed slightly different, and the aliased "humps" will always "pixel drift" between functions.

This means that byte comparison of filled buffers is no longer the "ultimate judge" of correctness. We will change the compliance of functions approach.

2. 
Our new version of test suit expects the line function to pass all test as before. 
Then pass the new "Endpoints and Mass" Test, meaning that as long as the line starts where it should, ends where it should, and is "monotonically moving" (no gaps or weird zig-zags), it is a "correct" line.

3.
Once new run length slice function passes all tests along with reference versions,
we successfully fail the buffer comparison and moving into performance territory.

4.
To observe a noticeable performance gain we can increase stress load with buttons ui.
Initially drawing only 500 lines, the gain from Run Length Slice version is a negligible .2 to .3 milliseconds. 
Should we increase the load to about 5000 lines, the gap from references functions increase to about .5 milliseconds.
And once we max out the load to draw about 50000 lines, the Run Length Slice version becomes supersonic, flying over 5 milliseconds faster!

Voila!

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests






