
//   CGIONE
//   17.12.2025
//   CGI test 10

The Yield Midpoint

1.
In previous adventure we established the base of our climb to the peak of the Yield Mountain.
Yet byte comparing buffer sends us in the tail spin of guesswork based on failed coordinates,
essentially chasing pixel perfection.

So, lets switch gears to run a set of tests for any line function,
that should correctly handle screen line approximation.

I proudly present behemoth of The Line Rasterization Compliance Test Suite.

2.
Firstly, we establish The Reference Standard: Bresenham's algorithm base, inclusive end points, integer coordinates, screen space.

Start with a Symmetry Test, meaning a line from point A to point B should be pixel-by-pixel equal to the line from point B to point A.
It is valuable to check how correct coordinates rounding are as well as error accumulation inside function.
We then compare our GTR function against itself with reverse coordinates.

3.
Next is The Octant Walk Test, where we check all octanes where the the flipping of major axes and signs happens. This covers all possible configurations of shallow and steep line directions.
As before we run this dataset both forward and backward with our symmetry runner.

4. 
Next is a set called Cardinal Tie-breaker and aim to test minimal line slopes, integer and sub-pixel precisions.
Just as before, feed the data, run forward and backward, evaluate results.

5.
The last dataset is a combination of boundary points and short line test.
This test is designed toward the optimized versions of line plotting to check skips of pixels.

Once we get a "Pass" on all runs, our GTR line function consider to be Gold Standard.

6.
To test how easily a line function can pass compiler's check and fail on some of test runs,
we have an SUT_BROKEN function, a saboteur entity that avoids normalization of coordinates in it's algorithm.

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests






