#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

#define LINESIZE 1024
#define MAXVALUERGB 255

struct pixelP3 {
	int r;
	int g;
	int b;
};

struct pixelP6 {
	unsigned char charR;
	unsigned char charG;
	unsigned char charB;
};

struct Timagem {
	char formato[3];
	int largura;
	int altura;
	int valorMax;
	struct pixelP3 *imagem;
	int mediaR;
	int mediaG;
	int mediaB;
};


struct pixelP3 *alocaImagemP3(int largura, int altura);

struct pixelP6 *alocaImagemP6(int largura, int altura);

struct Timagem *leImagem();

void calculaMediaPixels(struct Timagem *pastilha);
