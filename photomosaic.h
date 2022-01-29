/* CI-1002 (ERE 2)
Prof. Carlos Maziero

Author:
Tiago Serique Valadares (GRR20195138)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

#define SIZE 1024
#define MAXRGB 255

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

struct Timage {
	char formato[3];
	int width;
	int height;
	int maxValue;
	struct pixelP3 *image;
	double averageR;
	double averageG;
	double averageB;
};


struct pixelP3 *p3ImageArrayAlloc(int width, int height);

struct pixelP6 *p6ImageArrayAlloc(int width, int height);

struct Timage *TimageAlloc();

void *tilesArrayAlloc(int number_of_tiles);

void imageDealloc(struct Timage *image);

// faz a leitura da imagem e a retorna um ponteiro para ela
struct Timage *readImage(FILE* arq);

// escreve a imagem no arquivo de saida
void writeImage(struct Timage *image, FILE* output);

// Funcao de filtro para a funcao scandir()
int filter(const struct dirent *dir);

// calcula a media dos quadrados dos pixels
void calculateAveragePixels(struct Timage *tile);

// faz o mosaico acontecer
void buildMosaic(struct Timage *mosaic_base, struct Timage **tiles, 
int number_of_tiles);

// pega bloco da imagem do tamanho da pastilha e guarda em um auxiliar
void copyImageBlock(struct Timage *mosaic_base, struct Timage *tile_copy, 
int width_tile, int height_tile, int i, int j);

// acha a pastilha com a menor distancia da imagem
int shortestDistance(struct Timage *image, struct Timage **tiles,
int number_of_tiles);

// calcula a distancia entre as duas imagens usando red mean
double calculateDistance(struct Timage *tile, struct Timage *image);

// substitui pastilha na imagem de input
void replaceTile(struct Timage *mosaic_base, struct Timage *tile, 
int width_tile, int height_tile, int i, int j);
