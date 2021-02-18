
#include "fotomosaico.h"


struct pixelP3 *alocaVetorImagemP3(int largura, int altura){
	return (struct pixelP3 *)malloc(sizeof(struct pixelP3) * largura * altura);
}


struct pixelP6 *alocaVetorImagemP6(int largura, int altura){
	return (struct pixelP6 *)malloc(sizeof(struct pixelP6) * (largura * altura + 1));
}


struct Timagem *alocaTimagem(){
	return (struct Timagem *)malloc(sizeof(struct Timagem));
}


void *alocaVetorPastilhas(int qtdPastilhas){
	return malloc(sizeof(struct Timagem*) * qtdPastilhas);
}


void desalocaImagem(struct Timagem *imagem){
	free(imagem->imagem);
	free(imagem);
}


// faz a leitura da imagem e a retorna um ponteiro para ela
struct Timagem *leImagem(FILE* arq){
	char line[SIZE+1];
	
	int ret, i, tamanho;
	
	// aloca memoria para os dados da imagem
	struct Timagem *pastilha = alocaTimagem();
	if (!pastilha){
		fprintf(stderr, "Nao foi possivel alocar memoria\n");
		fprintf(stderr, "Leitura dos dados da imagem nao pode ser feita\n");
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
		fprintf(stderr, "Formato errado: %s, diferente de P3 e P6\n", line);
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
		fprintf(stderr, "Erro ao ler dimensoes da imagem\n");
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
		fprintf(stderr, "Erro ao ler o valor maximo dos pixels\n");
		exit(1);
	}
	// se for diferente de 255, retorna um erro
	if ( pastilha->valorMax != MAXRGB){
		fprintf(stderr, "Valor maximo diferente de 255\n");
		exit(1);
	}

	// Aviso de possivel erro: 
	// Caso tenha comentarios apos o valor maximo havera um erro na
	// leitura da imagem

	// aloca o vetor de pixels para armazenar a imagem no formato P3
	pastilha->imagem = alocaVetorImagemP3(pastilha->largura, pastilha->altura);
	if ( !pastilha->imagem ){
		fprintf(stderr, "Nao foi possivel alocar memoria\n");
		fprintf(stderr, "Leitura dos pixels da imagem nao pode ser feita\n");
		exit(1);
	}


	// verifica o formato da imagem para prosseguir com a leitura dos pixels
	if (!strcmp(pastilha->formato, "P3")){
		tamanho = pastilha->largura * pastilha->altura;
		
		// faz leitura da imagem em formato P3
		for (i = 0; i < tamanho; i++){
			fscanf(arq, "%d ", &pastilha->imagem[i].r);
			fscanf(arq, "%d ", &pastilha->imagem[i].g);
			fscanf(arq, "%d ", &pastilha->imagem[i].b);
		}
	}
	else if (!strcmp(pastilha->formato, "P6")){
		// aloca vetor de pixels para armazenar a imagem no formato P6
		struct pixelP6 *temp = alocaVetorImagemP6(pastilha->largura, pastilha->altura);
		if (!temp){
			fprintf(stderr, "Nao foi possivel alocar memoria\n");
			fprintf(stderr, "Leitura dos pixels da imagem nao pode ser feita\n");
			exit(1);
		}
		
		tamanho = pastilha->largura * pastilha->altura;
		
		// faz leitura da imagem P6
		ret = fread(temp, sizeof(struct pixelP6), tamanho + 1, arq);
		if ( ret != tamanho ){
			fprintf(stderr, "Erro ao fazer leitura dos dados\n");
			fprintf(stderr, "funcao fread() retornou valor diferente do esperado\n");
			exit(1);
		}

		// converte para imagem P3
		for (i = 0; i < tamanho; i++){
			pastilha->imagem[i].r = (int)temp[i].charR;
			pastilha->imagem[i].g = (int)temp[i].charG;
			pastilha->imagem[i].b = (int)temp[i].charB;
		}

		free(temp);
	}
	
	// faz o calculo da media dos componentes RGB
	calculaMediaPixels(pastilha);

	return pastilha;
}


// escreve a imagem no arquivo de saida
void escreveImagem(struct Timagem *pastilha, FILE* output){
	int tamanho, i;

	// adiciona o formato da imagem 
	fputs(pastilha->formato, output);
	fputs("\n", output);

	// adiciona as dimensoes
	fprintf(output, "%d %d\n", pastilha->largura, pastilha->altura);
	
	// adiciona valor maximo
	fprintf(output, "%d\n", pastilha->valorMax);

	// verifica o formato da imagem para prosseguir com a escrita dos pixels
	if (!strcmp(pastilha->formato, "P3")){
		tamanho = pastilha->altura * pastilha->largura;

		// escreve os valores da imagem no arquivo
		for (i = 0; i < tamanho; i++){
			fprintf(output, "%d ", pastilha->imagem[i].r);
			fprintf(output, "%d ", pastilha->imagem[i].g);
			fprintf(output, "%d ", pastilha->imagem[i].b);
		}
	}
	else if (!strcmp(pastilha->formato, "P6")){
		// aloca vetor de pixels para armazenar a imagem no formato P6
		struct pixelP6 *temp = alocaVetorImagemP6(pastilha->largura, pastilha->altura);
		int ret;
		
		tamanho = pastilha->altura * pastilha->largura;

		// converte os valores da imagem de volta ao original
		for (i = 0; i < tamanho; i++){
			temp[i].charR = (unsigned char)pastilha->imagem[i].r;
			temp[i].charG = (unsigned char)pastilha->imagem[i].g;
			temp[i].charB = (unsigned char)pastilha->imagem[i].b;
		}

		// escreve os valores da imagem no arquivo
		ret = fwrite(temp, sizeof(struct pixelP6), tamanho, output);
		if ( ret != tamanho ){
			fprintf(stderr, "Erro fazer escrita dos dados\n");
			fprintf(stderr, "funcao fwrite() retornou valor diferente do esperado\n");
			exit(1);
		}

		free(temp);
	}
}


// Funcao de filtro para a funcao scandir()
int filtro(const struct dirent *dir){
	int tamanho = strlen(dir->d_name);

	// se o tamanho for invalido ou nao for um arquivo
	if (tamanho < 1 || dir->d_type != DT_REG)
		return 0;
	
	// se tiver .ppm no final do arquivo
	if ( dir->d_name[tamanho - 4] == '.' && dir->d_name[tamanho - 3] == 'p'
	  && dir->d_name[tamanho - 2] == 'p' && dir->d_name[tamanho - 1] == 'm' ){
		return 1;
	}

	return 0;
}


// calcula a media dos quadrados dos pixels
void calculaMediaPixels(struct Timagem *pastilha){
	double tamanho;
	int i;

	tamanho = pastilha->altura * pastilha->largura;

	// soma de quadrados
	for (i = 0; i < tamanho; i++){
		pastilha->mediaR += pastilha->imagem[i].r * pastilha->imagem[i].r;
		pastilha->mediaG += pastilha->imagem[i].g * pastilha->imagem[i].g;
		pastilha->mediaB += pastilha->imagem[i].b * pastilha->imagem[i].b;
	}
	
	// raiz quadrada da media da soma de quadrados
	pastilha->mediaR = sqrt(pastilha->mediaR / tamanho);
	pastilha->mediaG = sqrt(pastilha->mediaG / tamanho);
	pastilha->mediaB = sqrt(pastilha->mediaB / tamanho);
}


// faz o mosaico acontecer
void constroiMosaico(struct Timagem *imagem, struct Timagem **pastilhas, 
int qtdPastilhas){
	int larguraI		= imagem->largura;
	int alturaI			= imagem->altura;
	int larguraP		= pastilhas[0]->largura; 
	int alturaP			= pastilhas[0]->altura;
	int aMenor			= 0;	// armazena o indice da pastilha 

	// aloca memoria para os dados da imagem
	struct Timagem *imagemAuxiliar = alocaTimagem();
	// aloca o vetor de pixels para armazenar a imagem no formato P3
	imagemAuxiliar->imagem = alocaVetorImagemP3(larguraP, alturaP);
	if (!imagemAuxiliar){
		fprintf(stderr, "Erro ao alocar memoria para construcao do mosaico\n");
		exit(1);
	}

	// divide a imagem original em blocos do tamanho das pastilhas
	for (int i = 0; i < ceil(alturaI/(double)alturaP); i++){
		for (int j = 0; j < ceil(larguraI/(double)larguraP); j++){
			// copia o bloco da imagem principal para a imagem auxiliar
			copiaBlocoDaImagem(imagem, imagemAuxiliar, larguraP, alturaP, i, j);

			// zera as medias para nao influenciar nos calculos das proximas
			// imagens 
			imagemAuxiliar->mediaR = 0;
			imagemAuxiliar->mediaG = 0;
			imagemAuxiliar->mediaB = 0;

			// calcula a media dos quadrados dos componentes RGB 
			calculaMediaPixels(imagemAuxiliar);

			// encontra a pastilha com menor distancia entre as medias
			aMenor = menorDistancia(imagemAuxiliar, pastilhas, qtdPastilhas);

			// faz a substituicao do bloco da imagem original pela pastilha 
			substituiPastilha(imagem, pastilhas[aMenor], larguraP, alturaP, i, j);
		}
	}
	free(imagemAuxiliar);
}


// pega bloco da imagem do tamanho da pastilha e guarda em um auxiliar
void copiaBlocoDaImagem(struct Timagem *img, struct Timagem *imgAuxiliar, 
int larguraP, int alturaP, int i, int j){
	int alturaI		= img->altura;
	int larguraI	= img->largura;

	// calcula o inicio do for com base no tamanho da pastilha e nas coordenadas
	// do bloco atual da imagem original
	int inicioLin = alturaP		* i;
	int inicioCol = larguraP 	* j;

	// calcula o final do for com base no inicio do for + a dimensao da pastilha
	int limiteLin = alturaP		+ inicioLin;
	int limiteCol = larguraP 	+ inicioCol;

	int lin, col, cont = 0;
	for (lin = inicioLin; lin < limiteLin && lin < alturaI; lin++){
		for (col = inicioCol; col < limiteCol && col < larguraI; col++){
			imgAuxiliar->imagem[cont].r = img->imagem[larguraI * lin + col].r;
			imgAuxiliar->imagem[cont].g = img->imagem[larguraI * lin + col].g;
			imgAuxiliar->imagem[cont].b = img->imagem[larguraI * lin + col].b;
			cont++;
		}
	}
	
	// adiciona as dimensoes a imagem auxiliar
	imgAuxiliar->altura		= alturaP;
	imgAuxiliar->largura	= larguraP;
}


// acha a pastilha com a menor distancia da imagem
int menorDistancia(struct Timagem *imagem, struct Timagem **pastilhas, 
int qtdPastilhas){
	double distancia, distanciaAnterior = NUMGRANDE;
	int menor = 0;

	for (int i = 0; i < qtdPastilhas; i++){
		distancia = calculaDistancia(pastilhas[i], imagem);
		if (distancia < distanciaAnterior){
			menor = i;
			distanciaAnterior = distancia;
		}
	}

	return menor;
}


// calcula a distancia entre as duas imagens usando red mean
double calculaDistancia(struct Timagem *imagem1, struct Timagem *imagem2){
	double mRed, red, green, blue;
	
	mRed = (imagem1->mediaR + imagem2->mediaR) / 2.0;

	red = (2.0 + mRed / 256.0) * pow((imagem1->mediaR - imagem2->mediaR), 2.0);

	green = 4 * pow((imagem1->mediaG - imagem2->mediaG), 2);

	blue = (2 + (255 - mRed)/256.0) * pow((imagem1->mediaB - imagem2->mediaB), 2.0);

	return sqrt(red + green + blue);
}


// substitui pastilha na imagem principal
void substituiPastilha(struct Timagem *imagem, struct Timagem *pastilha, 
int larguraP, int alturaP, int i, int j){
	int alturaI		= imagem->altura;
	int larguraI	= imagem->largura;

	// calcula o inicio do for com base no tamanho da pastilha e nas coordenadas
	// do bloco atual da imagem original
	int inicioLin = alturaP	* i;
	int inicioCol = larguraP * j;

	// calcula o final do for com base no inicio do for + a dimensao da pastilha
	int limiteLin = alturaP	+ alturaP	* i;
	int limiteCol = larguraP + larguraP * j;

	int cont = 0;
	for (int lin = inicioLin; lin < limiteLin && lin < alturaI; lin++){
		for (int col = inicioCol; col < limiteCol && col < larguraI; col++){
			imagem->imagem[larguraI * lin + col].r = pastilha->imagem[cont].r;
			imagem->imagem[larguraI * lin + col].g = pastilha->imagem[cont].g;
			imagem->imagem[larguraI * lin + col].b = pastilha->imagem[cont].b;
			cont++;
		}
	}
}

