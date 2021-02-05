
#include "fotomosaico.h"


struct pixelP3 *alocaImagem(int largura, int altura){
	struct pixelP3 *temp;

	temp = malloc(sizeof(struct pixelP3) * largura * altura);
	
	return temp;						
}


struct Timagem *leImagem(FILE* arq){
	int ret, i;
	char line[LINESIZE+1];
	struct Timagem *pastilha = malloc(sizeof(struct Timagem));;
	
/*============================================================================*/

	// ignora as linhas em branco ou comentadas
	fgets(line, LINESIZE, arq);
	while(line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#'){
		fgets(line, LINESIZE, arq);
	};

	// le o formato do arquivo
	ret = sscanf(line, "%s", pastilha->formato);
	if (ret != 1){
		perror("Formato errado");
		fclose(arq);
		exit(1);
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
		perror("Erro ao pegar tamanho da imagem");
		fclose(arq);
		exit(1);
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
		perror("Erro ao pegar valor maximo dos componentes");
		exit(1);
	}
	printf("Valor maximo cor: %d\n", pastilha->valorMax);


/*============================================================================*/

	// Warning: Caso tenha comentarios apos o valor maximo havera um erro na
	// leitura da imagem

	pastilha->imagem = alocaImagem(pastilha->largura, pastilha->altura);
	if ( !pastilha->imagem ){
		perror("Erro ao alocar memoria");
		fclose(arq);
		exit(1);
	}

	if (!strcmp(pastilha->formato, "P3")){

		for (i = 0; i < (pastilha->largura * pastilha->altura); i++){
			fscanf(arq, "%d ", &pastilha->imagem[i].r);
			fscanf(arq, "%d ", &pastilha->imagem[i].g);
			fscanf(arq, "%d ", &pastilha->imagem[i].b);
		}
		calculaMediaPixels(pastilha);

		printf("Media R: %d, G: %d, B: %d\n", pastilha->mediaR, pastilha->mediaG, 
			pastilha->mediaB);
	}
	

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




