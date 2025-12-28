
//   CGIONE
//   25.12.2025
//   CGI test 14

Pixel Drift Zen

1.
In the previous adventure, a new line plotting routing was composed to battle our line rasterization test suit.
New function achieve Gold status with ease and significant performance gain.
There is one more piece of the puzzle, the expected fail of buffer's memcmp.

Today we are cathcing the elusive Pixel Drift, both in numbers and visually.

2.
Before the main loop starts the test subject should achieve the gold status from all 5 symmetric runs.
On main loop, we do comparison of the buffers in following manner:
- total pixel mass of prepared random lines tells us if function is underplotting or overplotting;
- total number of drifted pixel, meaning the disagreement between function on final color at same coordinates;
- two counters, one cyan color, meaning new buffer has a color pixel where the old buffer has a background, and red color, meaning the opposite.
- the counter of "ghost" pixel, meaning mismatch in line's random colors.

3.
For understanding the scope of drifting we construct a Diff-Map, where all drifted pixel are placed with enchanced neighbour pixels.

By comparing the numbers we get the full picture: the two buffers are quite close in terms of pixel mass. 

We check pixel mass difference against error colors difference to get the perfect mass balance of both geometrical parity as well as color parity.

The total number of drifted pixels gives us a disagreement less then 1% between functions. Or, in reverse, the Run-Length slice version is 99% on par with the GTR representation.

The Parity Zen is here!

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests






