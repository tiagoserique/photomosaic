
#include "fotomosaico.h"


struct pixelP3 *alocaImagemP3(int largura, int altura){
	struct pixelP3 *temp;

	temp = malloc(sizeof(struct pixelP3) * largura * altura);
	
	return temp;						
}


struct pixelP6 *alocaImagemP6(int largura, int altura){
	struct pixelP6 *temp;

	temp = malloc(sizeof(struct pixelP6) * largura * altura);
	
	return temp;						
}


struct Timagem *leImagem(){
	int ret, i;
	char line[LINESIZE+1];
	struct Timagem *pastilha = malloc(sizeof(struct Timagem));;
	FILE* arq;

/*============================================================================*/

	// 
	arq = fopen("flower.ppm", "r");
	if ( !arq ){
		fprintf(stderr, "Erro ao abrir arquivo");
		exit(1);
	}

/*============================================================================*/

	// ignora as linhas em branco ou comentadas
	fgets(line, LINESIZE, arq);
	while(line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#'){
		fgets(line, LINESIZE, arq);
	};

	// le o formato do arquivo
	ret = sscanf(line, "%s", pastilha->formato);
	if (ret != 1){
		fprintf(stderr, "Formato errado");
		fclose(arq);
		return NULL;
	}
	printf("Formato: %s\n", pastilha->formato);

/*============================================================================*/

	// ignora as linhas em branco ou comentadas
	fgets(line, LINESIZE, arq);
	while(line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#'){
		fgets(line, LINESIZE, arq);
	};

	// le as dimensoes da imagem
	ret = sscanf(line, "%d %d", &pastilha->largura, &pastilha->altura);
	if (ret != 2){
		printf("%d", ret);
		fprintf(stderr, "Erro ao pegar tamanho da imagem");
		fclose(arq);
		return NULL;
	}
	printf("Largura: %d, Altura: %d\n", pastilha->largura, pastilha->altura);

/*============================================================================*/
	
	// ignora as linhas em branco ou comentadas
	fgets(line, LINESIZE, arq);
	while( line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#'){
		fgets(line, LINESIZE, arq);
	};

	// le o valor maximo dos pixels
	ret = sscanf(line, "%d", &pastilha->valorMax);
	if ( ret != 1 ){
		fprintf(stderr, "Erro ao pegar valor maximo dos componentes");
		exit(1);
	}
	printf("Valor maximo cor: %d\n", pastilha->valorMax);

 /*============================================================================*/

	// Warning: Caso tenha comentarios apos o valor maximo havera um erro na
	// leitura da imagem

	pastilha->imagem = alocaImagemP3(pastilha->largura, pastilha->altura);
	if ( !pastilha->imagem ){
		fprintf(stderr, "Erro ao alocar memoria");
		fclose(arq);
		exit(1);
	}

	if (!strcmp(pastilha->formato, "P3")){
		for (i = 0; i < (pastilha->largura * pastilha->altura); i++){
			fscanf(arq, "%d ", &pastilha->imagem[i].r);
			fscanf(arq, "%d ", &pastilha->imagem[i].g);
			fscanf(arq, "%d ", &pastilha->imagem[i].b);
		}
	}
	else if (!strcmp(pastilha->formato, "P6")){
		struct pixelP6 *temp = alocaImagemP6(pastilha->largura, pastilha->altura);

		ret = fread(temp, 3 * pastilha->largura, pastilha->altura, arq);
		
		if ( ret != pastilha->altura ){
			fprintf(stderr, "Erro ao alocar memoria");
			fclose(arq);
			exit(1);
		}

		for (i = 0; i < (pastilha->largura * pastilha->altura); i++){
			pastilha->imagem[i].r = (int)temp->charR;
			pastilha->imagem[i].g = (int)temp->charG;
			pastilha->imagem[i].b = (int)temp->charB;
		}

		free(temp);
	}
	
	calculaMediaPixels(pastilha);

	printf("Media R: %d, G: %d, B: %d\n", pastilha->mediaR, pastilha->mediaG, 
		pastilha->mediaB);

	fclose(arq);

	return pastilha;
}


void calculaMediaPixels(struct Timagem *pastilha){
	int i, tamVetor;

	tamVetor = pastilha->altura * pastilha->largura;

	for (i = 0; i < tamVetor; i++){
		pastilha->mediaR += pastilha->imagem[i].r;
		pastilha->mediaG += pastilha->imagem[i].g;
		pastilha->mediaB += pastilha->imagem[i].b;
	}

	pastilha->mediaR /= tamVetor;
	pastilha->mediaG /= tamVetor;
	pastilha->mediaB /= tamVetor;
}


void escreveImagem(struct Timagem *pastilha, char *saida){
	FILE* arq;
	int ret, tamanho, i, j;

	arq = fopen(saida, "w+");
	if (!arq){
		fprintf(stderr, "Erro ao salvar mosaico");
		exit(0);
	}

	fputs(pastilha->formato, arq);
	fputs("\n", arq);
	fprintf(arq, "%d %d\n", pastilha->largura, pastilha->altura);
	fprintf(arq, "%d\n", pastilha->valorMax);

	if (!strcmp(pastilha->formato, "P3")){
		tamanho = pastilha->altura * pastilha->largura * 3;

		// escreve os valores gerados no final do arquivo
		for (i = 0; i < (pastilha->altura); i++){
			int indice = i * pastilha->altura; 
			int largura = pastilha->largura + indice;
			for (j = indice; j < largura; j++){
				fprintf(arq, "%d ", pastilha->imagem[j].r);
				fprintf(arq, "%d ", pastilha->imagem[j].g);
				fprintf(arq, "%d ", pastilha->imagem[j].b);
			}
			fputs("\n", arq);
		}
	}
	else if (!strcmp(pastilha->formato, "P6")){
		tamanho = pastilha->altura * pastilha->largura * 3;

		// escreve os valores gerados no final do arquivo
		ret = fwrite(pastilha->imagem, sizeof(int), tamanho, arq);
		if (ret)
			printf ("Gravou %d valores com sucesso!\n", ret) ;
		else
			printf ("Erro ao gravar...\n") ;
	}

	fclose(arq);
}


