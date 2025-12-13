
//   CGIONE
//   13.12.2025
//   CGI test 09

Nicer font
https://github.com/craigberry1983/Win32_software_renderer

Window Dressing

1.
This time our adventure revolves around prettifying our main window. 

2.
The first task is take our the main window in full, utilizing the so called "non-client area".
To simplify, we manually handle the win32 calls so the whole window is one big buffer.

Now, because our program uses fixed dimensions, we dont need to resize or drag window.
For more advanced version we obviously have to write corrensponding hit-testing routines.

3.
For several years the fixed font from Mr. Craigberry's project worked flawlessly,
so we can use it instead that ugly thing from previous adventures.
This 8-by-8, English only, capital letters only fontset, that looks fine for our purposes.

With a slight modification for character search and dimensions, 
a new text plotting routing is implemented.

4.
In previous adventures we use keyboard keys as a toggle switches.
The use interface toggle buttons works similar: 
check if cursor position is over button's rectangle;
check if mouse left button is clicked and switch's temporary state is off;
If all conditions are met, the button changes state.

Now, this example is "quazi" ui, meaning the "button" is not a single object,
but a collection of logic and drawing calls.

In larger project, all components of a "button" as object or ui element will be packed into struct,
incorporated into appropriate function calls then scattered across the codebase. Pure joy!

5.
Like with keyboard, toggle buttons can operate on additional functions.
We can show and hide the generated image, performance information and
duplicate keyboard functionality.

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests






