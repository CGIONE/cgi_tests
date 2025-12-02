
//   CGIONE
//   01.12.2025
//   CGI test 06

Foolproof reflection

1. 
Its time to slow down, reflect and meditate on the correctness of the whole thing.

2. 
Once again, the compiler has an extensive set of flags 
that can be used to set a warning upon compilation.

3.
The only piece that stands out is the famous "segmentation fault",
a write to out-of-bounds memory segment.
Most times it happens on allocated buffers, those size are generated upon execution or resizing.

The common cure is to breathe in, go for a walk, come back 
and recheck buffer boundaries plus printf value that is written in real time in terminal.

4. 
Why not make graphics features, pleasant to the eye.
After all, development and debugging is about doing similar things over and over again keeping head clear.

5.
Performance goes hand in hand with complexity. 
Any architectural choice (vector instruction, multithreading etc.) is a good choice once it is clear in development and proven by testing and time logging.

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests


