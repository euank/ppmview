# ppmview - A simple ppm viewer (with OpenGL)

## Building

Standard CMake
```
cmake .; make
```

### Documentation:
```
cmake .; make docs; firefox docs/html/files.html
```

## Usage
```
./ppmview <in.ppm> [<out.ppm>]
```
### Controls
r, g, b, o - Control the visible colour channels. Red, green, blue, and all respectively.
i - Invert the image.
q or clicking - Quit

## Code

This code reads a ppm file, whose name is given as the first argument, and displays it using opengl textures. It will also write a similar ppm if a second argument is given indicating the output filename.

Masking features for Red, Green, and Blue channels are avaliable by pressing 'r', 'g', and 'b' respectively. 'o' reverts the image. The image can also be inverted by pressing 'i'. 'q' or clicking will exit the program.

Because OpenGL textures are used, resizing will automatically stretch the image.
Documentation is provided in the form of regular comments and doxygen formatted comments.

## Issues
For reasons that I could not discern, the image is inverted on my Samsung Chromebook, but not on any other computers I tried. If you know why, I'd be interested.

It also takes a large amount of ram for large images, but that's basically a given.

### Error handling
Errors are rarely actually fatal. The program will print messages to stderr on potential issues, and then attempt to continue anyways. It happily will display an incorrect image rather than nothing at all. This is intentional and in line with the recommendations of the ppm spec. The program tries to never crash in the process of going on into the unknown, however.
