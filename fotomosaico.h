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
#define NUMGRANDE 1123581321

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
	double mediaR;
	double mediaG;
	double mediaB;
};


struct pixelP3 *alocaVetorImagemP3(int largura, int altura);

struct pixelP6 *alocaVetorImagemP6(int largura, int altura);

struct Timagem *alocaTimagem();

struct Timagem *alocaVetorPastilhas(int qtdPastilhas);

void desalocaImagem(struct Timagem *imagem);

// faz a leitura da imagem e a retorna um ponteiro para ela
struct Timagem *leImagem(FILE* arq);

// escreve a imagem no arquivo de saida
void escreveImagem(struct Timagem *pastilha, FILE* output);

// Funcao de filtro para a funcao scandir()
int filtro(const struct dirent *dir);

// calcula a media dos quadrados dos pixels
void calculaMediaPixels(struct Timagem *pastilha);

// faz o mosaico acontecer
void constroiMosaico(struct Timagem *imagem, struct Timagem **pastilhas, 
int qtdPastilhas);

// pega bloco da imagem do tamanho da pastilha e guarda em um auxiliar
void copiaBlocoDaImagem(struct Timagem *img, struct Timagem *imgAuxiliar, 
int larguraP, int alturaP, int i, int j);

// acha a pastilha com a menor distancia da imagem
int menorDistancia(struct Timagem *imagem, struct Timagem **pastilhas,
int qtdPastilhas);

// calcula a distancia entre as duas imagens usando red mean
double calculaDistancia(struct Timagem *imagem1, struct Timagem *imagem2);

// 
void substituiPastilha(struct Timagem *imagem, struct Timagem *pastilha, 
int larguraP, int alturaP, int i, int j);
