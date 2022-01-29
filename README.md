# Photomosaic Generator

A photomosaic is a picture that has been divided into tiles sections, each of which is replaced with another photograph that matches the target photo, and this is what this program does, It creates a photomosaic from images in PPM format.

The program works with both types of PPM format (ASCII and binary) and if you want to convert imeges to this format, a good option is to use the gimp program.

To use the program, you need an image (the bigger the better) and a directory with a set of tiles (the same size).

To compile, use:

```bash
make
or
make mosaic
```

To run the program in terminal, use:

```bash
./mosaic -i input.ppm -o output.ppm
or
./mosaic < input.ppm > output.ppm -p ./tiles20x20/
```

By default, the program searches for the tiles in a directory called `./tiles`, if you want to change to a directory with another name, use the ``-p`` option.
