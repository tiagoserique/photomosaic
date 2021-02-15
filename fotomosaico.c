
#include "fotomosaico.h"


struct pixelP3 *alocaImagemP3(int largura, int altura){
	return malloc(sizeof(struct pixelP3) * largura * altura);
}


struct pixelP6 *alocaImagemP6(int largura, int altura){
	return malloc(sizeof(struct pixelP6) * largura * altura);
}


struct Timagem *alocaVetorPastilhas(int qtdImagens){
	return malloc(sizeof(struct Timagem*) * qtdImagens);
}


void desalocaImagem(struct Timagem *imagem){
	free(imagem->imagem);
	free(imagem);
}


struct Timagem *leImagem(FILE* arq){
	char line[SIZE+1];
	
	int ret, i;
	
	struct Timagem *pastilha = malloc(sizeof(struct Timagem));
	if (!pastilha){
		fprintf(stderr, "Nao foi possivel alocar memoria\n");
		exit(1);
	}


	// ignora as linhas em branco ou comentadas
	fgets(line, SIZE, arq);
	while(line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#'){
		fgets(line, SIZE, arq);
	};


	// le o formato do arquivo
	ret = sscanf(line, "%s", pastilha->formato);
	if (ret != 1 
	|| (strcmp(pastilha->formato, "P3") && strcmp(pastilha->formato, "P6"))){
		fprintf(stderr, "Formato errado: %s\n", line);
		fclose(arq);
		return NULL;
	}


	// ignora as linhas em branco ou comentadas
	fgets(line, SIZE, arq);
	while(line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#'){
		fgets(line, SIZE, arq);
	};


	// le as dimensoes da imagem
	ret = sscanf(line, "%d %d", &pastilha->largura, &pastilha->altura);
	if (ret != 2){
		printf("%d", ret);
		fprintf(stderr, "Erro ao pegar tamanho da imagem\n");
		fclose(arq);
		return NULL;
	}

	
	// ignora as linhas em branco ou comentadas
	fgets(line, SIZE, arq);
	while( line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#'){
		fgets(line, SIZE, arq);
	};

	// le o valor maximo dos pixels
	ret = sscanf(line, "%d", &pastilha->valorMax);
	if (ret != 1){
		fprintf(stderr, "Erro ao pegar valor maximo dos componentes\n");
		exit(1);
	}


	// Warning: Caso tenha comentarios apos o valor maximo havera um erro na
	// leitura da imagem

	pastilha->imagem = alocaImagemP3(pastilha->largura, pastilha->altura);
	if ( !pastilha->imagem ){
		fprintf(stderr, "Erro ao alocar memoria\n");
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
		if (!temp){
			fprintf(stderr, "Nao foi possivel alocar memoria\n");
			exit(1);
		}

		ret = fread(temp, 3 * pastilha->largura, pastilha->altura, arq);
		
		if ( ret != pastilha->altura ){
			fprintf(stderr, "Erro fazer leitura dos dados\n");
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


void escreveImagem(struct Timagem *pastilha, FILE* output){
	int ret, tamanho, i, j;

	fputs(pastilha->formato, output);
	fputs("\n", output);
	fprintf(output, "%d %d\n", pastilha->largura, pastilha->altura);
	fprintf(output, "%d\n", pastilha->valorMax);

	if (!strcmp(pastilha->formato, "P3")){
		tamanho = pastilha->altura * pastilha->largura * 3;

		// escreve os valores gerados no final do arquivo
		for (i = 0; i < (pastilha->altura); i++){
			int indice 	= i * pastilha->altura; 
			int largura = pastilha->largura + indice;
			
			for (j = indice; j < largura; j++){
				fprintf(output, "%d ", pastilha->imagem[j].r);
				fprintf(output, "%d ", pastilha->imagem[j].g);
				fprintf(output, "%d ", pastilha->imagem[j].b);
			}

			fputs("\n", output);
		}
	}
	else if (!strcmp(pastilha->formato, "P6")){
		tamanho = pastilha->altura * pastilha->largura * 3;

		// escreve os valores gerados no final do arquivo
		ret = fwrite(pastilha->imagem, sizeof(int), tamanho, output);
		if (!ret){
			fprintf(stderr, "Erro ao gravar...\n");
			exit(1);
		}
	}
}


float calculaDistancia(struct Timagem *imagem1, struct Timagem *imagem2){
	float mediaRed, red, green, blue;
	
	mediaRed = (imagem1->mediaR + imagem2->mediaR)/2;

	red = (2 + mediaRed/256) * pow((imagem1->mediaR - imagem2->mediaR), 2);

	green = 4 * pow((imagem1->mediaG - imagem2->mediaG), 2);

	blue = (2 + (255 - mediaRed)/256) * pow((imagem1->mediaB - imagem2->mediaB), 2);

	return sqrt(red + green + blue);
}


int filtro(const struct dirent *dir){
	int tamanho = strlen(dir->d_name);

	if ( tamanho < 1 )
		return 0;

	if ( dir->d_type != DT_REG )
		return 0;
	
	if ( dir->d_name[tamanho - 4] == '.' && dir->d_name[tamanho - 3] == 'p'
	  && dir->d_name[tamanho - 2] == 'p' && dir->d_name[tamanho - 1] == 'm' ){
		// fprintf(stderr, "%s\n", dir->d_name);
		return 1;
	}

	return 0;
}


