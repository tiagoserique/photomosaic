#include "fotomosaico.h"

#define LINESIZE 1024

int main(int argc, char *argv[]){
	FILE* arq;

	arq = fopen("flower.ppm", "r");
	if ( !arq ){
		perror("Erro ao abrir arquivo");
		exit(1);
	}

	leImagem(arq);

	
	// ret = fread(vetaux, 3 * largura, altura, arq);

	// printf("Primeiros pixels: %s\n", vetaux->r);
	// printf("Primeiros pixels: %s\n", vetaux->g);
	// printf("Primeiros pixels: %s\n", vetaux->b);


	exit(0);
}