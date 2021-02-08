
CFLAGS = -Wall

objects = main.o fotomosaico.o

mosaico: $(objects)
	gcc $(objects) -o mosaico

main.o: main.c
	gcc -c main.c $(CFLAGS)

fotomosaico.o:	fotomosaico.c fotomosaico.h
	gcc -c fotomosaico.c $(CFLAGS)

clean:
	rm -f *.o

purge:	clean
	rm -f mosaico
