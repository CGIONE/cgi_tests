
//   CGIONE
//   04.12.2025
//   CGI test 07

Mastering Control

1.
It is essential to have the power to change the input values during programm execution in real time.
From buffer settings to user input to pattern generation, its time to master the controls. 

2. 
Since the platform is orchestrating the usage of the hardware, 
we are entering the bleak desert of the win32 api once again.

Considering that this is not fullscreen FPS in development,
we can query cursor coordinates within logic update phase and clamp against window size. Simple!

3.
The platform has a list of "virtual key codes" for every keystroke,
so for now we simply query several buttons and store their values as booleans in a separate structure.

For a larger project it is possible to query a snapshot of the whole key set a store it as a key map.

4. 
Key query will reset every frame, 
we can check "previous key pressed" instance, creating toggle logic.

5. 
Now, in this graphic example we can control some parameters of the pattern.
By switching 'R' key we toggle pattern color.
Pressing 'W' and 'S' keys we change number of stripes in the pattern.
Holding left mouse key disables updates of background fill.

More adventures in linear buffer are coming!

Src: github.com/CGIONE/cgi_tests


