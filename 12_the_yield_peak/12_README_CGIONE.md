
//   CGIONE
//   22.12.2025
//   CGI test 12

The Yield Peak

1.
In this example we combine the line drawing tests with compliance of buffers across several implementations of plotting algorithm.

2.
Just as before, our line functions are to be integer coordinates, endpoints inclusive. Functions uses normalization of coordinates, that is to always draw line left to right.

3. 
Functions are executed before the main loop starts. 
After a test suit run, we compare the buffers on a byte level,
if function passed all test runs and has a Gold status.
This confirms that all implementations are mathematically identical.

4.
Next we have the option to plot random lines in real time. The only useful relative metric is the same, milliseconds of execution, using high precision platform timer.

5.
By observing the relative time performance, the difference is around .2 milliseconds, which is a noise value, a margin of error.
That is considered to be the yield peak of scalar execution, bottlenecked by memory throughput.

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests






