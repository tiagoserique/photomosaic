
CFLAGS = -Wall
LIBS = -lm

objects = main.o photomosaic.o

all: mosaic

mosaic: $(objects)
	gcc $(objects) -o mosaic $(LIBS)

main.o: main.c
	gcc -c main.c $(CFLAGS)

photomosaic.o:	photomosaic.c photomosaic.h
	gcc -c photomosaic.c $(CFLAGS)

clean:
	rm -f *.o

purge:	clean
	rm -f mosaic
