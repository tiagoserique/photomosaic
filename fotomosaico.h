#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>


struct pixelP3 {
	int r, g, b;
};

struct pixelP6 {
	unsigned char charR, charG, charB;
};

struct imagem {
	char formato[3];
	int largura;
	int altura;
	int valorMax;
	struct pixelP3 *pixels;
	int mediaR;
	int mediaG;
	int mediaB;
};


struct imagem leImagem(FILE* arq);

void retiraComentarios(FILE* arq);

void mostraArquivo(FILE* arq);

void ignoraLinhaEmBranco(FILE* arq);

