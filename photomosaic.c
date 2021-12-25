
#include "photomosaic.h"


struct pixelP3 *p3ImageArrayAlloc(int width, int height){
	return (struct pixelP3 *)malloc(sizeof(struct pixelP3) * width * height);
}


struct pixelP6 *p6ImageArrayAlloc(int width, int height){
	return (struct pixelP6 *)malloc(sizeof(struct pixelP6) * (width * height + 1));
}


struct Timage *TimageAlloc(){
	return (struct Timage *)malloc(sizeof(struct Timage));
}


void *tilesArrayAlloc(int number_of_tiles){
	return malloc(sizeof(struct Timage*) * number_of_tiles);
}


void imageDealloc(struct Timage *image){
	free(image->image);
	free(image);
}


// faz a leitura da imagem e a retorna um ponteiro para ela
struct Timage *readImage(FILE* arq){
	char line[SIZE + 1];
	
	int ret, i, size;
	
	// aloca memoria para os dados da imagem
	struct Timage *image = TimageAlloc();
	if ( !image ){
		fprintf(stderr, "Nao foi possivel alocar memoria\n");
		fprintf(stderr, "Leitura dos dados da imagem nao pode ser feita\n");
		exit(EXIT_FAILURE);
	}


	// ignora as linhas em branco ou comentadas
	fgets(line, SIZE, arq);
	while( line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#' ){
		fgets(line, SIZE, arq);
	};


	// le o formato do arquivo
	ret = sscanf(line, "%s", image->formato);
	if ( ret != 1 
	|| (strcmp(image->formato, "P3") && strcmp(image->formato, "P6")) ){
		fprintf(stderr, "Formato errado: %s, diferente de P3 e P6\n", line);
		return NULL;
	}


	// ignora as linhas em branco ou comentadas
	fgets(line, SIZE, arq);
	while( line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#' ){
		fgets(line, SIZE, arq);
	};


	// le as dimensoes da imagem
	ret = sscanf(line, "%d %d", &image->width, &image->height);
	if ( ret != 2 ){
		printf("%d", ret);
		fprintf(stderr, "Erro ao ler dimensoes da imagem\n");
		fclose(arq);
		return NULL;
	}

	
	// ignora as linhas em branco ou comentadas
	fgets(line, SIZE, arq);
	while( line[0] == '\0' || !strcmp(line,"\n") || line[0] == '#' ){
		fgets(line, SIZE, arq);
	};


	// le o valor maximo dos pixels
	ret = sscanf(line, "%d", &image->maxValue);
	if ( ret != 1 ){
		fprintf(stderr, "Erro ao ler o valor maximo dos pixels\n");
		exit(EXIT_FAILURE);
	}
	// se for diferente de 255, retorna um erro
	if ( image->maxValue != MAXRGB ){
		fprintf(stderr, "Valor maximo diferente de 255\n");
		exit(EXIT_FAILURE);
	}

	// Aviso de possivel erro: 
	// Caso tenha comentarios apos o valor maximo havera um erro na
	// leitura da imagem

	// aloca o vetor de pixels para armazenar a imagem no formato P3
	image->image = p3ImageArrayAlloc(image->width, image->height);
	if ( !image->image ){
		fprintf(stderr, "Nao foi possivel alocar memoria\n");
		fprintf(stderr, "Leitura dos pixels da imagem nao pode ser feita\n");
		exit(EXIT_FAILURE);
	}


	// verifica o formato da imagem para prosseguir com a leitura dos pixels
	if ( !strcmp(image->formato, "P3") ){
		size = image->width * image->height;
		
		// faz leitura da imagem em formato P3
		for (i = 0; i < size; i++){
			fscanf(arq, "%d ", &image->image[i].r);
			fscanf(arq, "%d ", &image->image[i].g);
			fscanf(arq, "%d ", &image->image[i].b);
		}
	}
	else if ( !strcmp(image->formato, "P6") ){
		// aloca vetor de pixels para armazenar a imagem no formato P6
		struct pixelP6 *temp = p6ImageArrayAlloc(image->width, image->height);
		if ( !temp ){
			fprintf(stderr, "Nao foi possivel alocar memoria\n");
			fprintf(stderr, "Leitura dos pixels da imagem nao pode ser feita\n");
			exit(EXIT_FAILURE);
		}
		
		size = image->width * image->height;
		
		// faz leitura da imagem P6
		ret = fread(temp, sizeof(struct pixelP6), size + 1, arq);
		if ( ret != size ){
			fprintf(stderr, "Erro ao fazer leitura dos dados\n");
			fprintf(stderr, "funcao fread() retornou valor diferente do esperado\n");
			exit(EXIT_FAILURE);
		}

		// converte para imagem P3
		for (i = 0; i < size; i++){
			image->image[i].r = (int)temp[i].charR;
			image->image[i].g = (int)temp[i].charG;
			image->image[i].b = (int)temp[i].charB;
		}

		free(temp);
	}
	
	// faz o calculo da media dos componentes RGB
	calculateAveragePixels(image);

	return image;
}


// escreve a imagem no arquivo de saida
void writeImage(struct Timage *image, FILE* output){
	int size, i;

	// adiciona o formato da imagem 
	fputs(image->formato, output);
	fputs("\n", output);

	// adiciona as dimensoes
	fprintf(output, "%d %d\n", image->width, image->height);
	
	// adiciona valor maximo
	fprintf(output, "%d\n", image->maxValue);

	// verifica o formato da imagem para prosseguir com a escrita dos pixels
	if ( !strcmp(image->formato, "P3") ){
		size = image->height * image->width;

		// escreve os valores da imagem no arquivo
		for (i = 0; i < size; i++){
			fprintf(output, "%d ", image->image[i].r);
			fprintf(output, "%d ", image->image[i].g);
			fprintf(output, "%d ", image->image[i].b);
		}
	}
	else if ( !strcmp(image->formato, "P6") ){
		int ret;

		// aloca vetor de pixels para armazenar a imagem no formato P6
		struct pixelP6 *temp = p6ImageArrayAlloc(image->width, image->height);
		
		size = image->height * image->width;

		// converte os valores da imagem de volta ao original
		for (i = 0; i < size; i++){
			temp[i].charR = (unsigned char)image->image[i].r;
			temp[i].charG = (unsigned char)image->image[i].g;
			temp[i].charB = (unsigned char)image->image[i].b;
		}

		// escreve os valores da imagem no arquivo
		ret = fwrite(temp, sizeof(struct pixelP6), size, output);
		if ( ret != size ){
			fprintf(stderr, "Erro fazer escrita dos dados\n");
			fprintf(stderr, "funcao fwrite() retornou valor diferente do esperado\n");
			exit(EXIT_FAILURE);
		}

		free(temp);
	}
}


// Funcao de filtro para a funcao scandir()
int filter(const struct dirent *dir){
	int size = strlen(dir->d_name);

	// se o tamanho for invalido ou nao for um arquivo
	if ( size < 1 || dir->d_type != DT_REG )
		return 0;
	
	// se tiver .ppm no final do arquivo
	if ( dir->d_name[size - 4] == '.' && dir->d_name[size - 3] == 'p'
	  && dir->d_name[size - 2] == 'p' && dir->d_name[size - 1] == 'm' ){
		return 1;
	}

	return 0;
}


// calcula a media dos quadrados dos pixels
void calculateAveragePixels(struct Timage *tile){
	int i;
	double size = tile->height * tile->width;

	// soma de quadrados
	for (i = 0; i < size; i++){
		tile->averageR += tile->image[i].r * tile->image[i].r;
		tile->averageG += tile->image[i].g * tile->image[i].g;
		tile->averageB += tile->image[i].b * tile->image[i].b;
	}
	
	// raiz quadrada da media da soma de quadrados
	tile->averageR = sqrt(tile->averageR / size);
	tile->averageG = sqrt(tile->averageG / size);
	tile->averageB = sqrt(tile->averageB / size);
}


// faz o mosaico acontecer
void buildMosaic(struct Timage *mosaic_base, struct Timage **tiles, 
int number_of_tiles){
	int shortest		= 0;					// armazena o indice da pastilha 
	int height_tile		= tiles[0]->height;
	int width_tile		= tiles[0]->width; 
	int height_mosaic	= mosaic_base->height;
	int width_mosaic	= mosaic_base->width;
	int width_block		= ceil(width_mosaic/(double)width_tile);
	int height_block	= ceil(height_mosaic/(double)height_tile);

	// aloca memoria para os dados da imagem
	struct Timage *tile_copy = TimageAlloc();

	// aloca o vetor de pixels para armazenar a imagem no formato P3
	tile_copy->image = p3ImageArrayAlloc(width_tile, height_tile);
	if ( !tile_copy ){
		fprintf(stderr, "Erro ao alocar memoria para construcao do mosaico\n");
		exit(EXIT_FAILURE);
	}

	// divide a imagem original em blocos do tamanho das pastilhas
	for (int i = 0; i < height_block; i++){
		for (int j = 0; j < width_block; j++){
			// copia o bloco da imagem de input para a imagem auxiliar
			copyImageBlock(mosaic_base, tile_copy, width_tile, height_tile, i, j);

			// zera as medias para nao influenciar nos calculos das proximas
			// imagens 
			tile_copy->averageR = 0;
			tile_copy->averageG = 0;
			tile_copy->averageB = 0;

			// calcula a media dos quadrados dos componentes RGB 
			calculateAveragePixels(tile_copy);

			// encontra a pastilha com menor distancia entre as medias
			shortest = shortestDistance(tile_copy, tiles, number_of_tiles);

			// faz a substituicao do bloco da imagem original pela pastilha 
			replaceTile(mosaic_base, tiles[shortest], width_tile, height_tile, i, j);
		}
	}

	free(tile_copy);
}


// pega bloco da imagem do tamanho da pastilha e guarda em um auxiliar
void copyImageBlock(struct Timage *mosaic_base, struct Timage *tile_copy, 
int width_tile, int height_tile, int i, int j){
	int height_mosaic	= mosaic_base->height;
	int width_mosaic	= mosaic_base->width;

	// calcula o inicio do for com base no tamanho da pastilha e nas coordenadas
	// do bloco atual da imagem original
	int row_start 		= height_tile	* i;
	int column_start 	= width_tile 	* j;

	// calcula o final do for com base no inicio do for + a dimensao da pastilha
	int row_end 	= height_tile	+ row_start;
	int column_end 	= width_tile 	+ column_start;

	int row, col, count = 0;
	for (row = row_start; row < row_end && row < height_mosaic; row++){
		for (col = column_start; col < column_end && col < width_mosaic; col++){
			tile_copy->image[count].r = mosaic_base->image[width_mosaic * row + col].r;
			tile_copy->image[count].g = mosaic_base->image[width_mosaic * row + col].g;
			tile_copy->image[count].b = mosaic_base->image[width_mosaic * row + col].b;
			count++;
		}
	}
	
	// adiciona as dimensoes a imagem auxiliar
	tile_copy->height	= height_tile;
	tile_copy->width	= width_tile;
}


// acha a pastilha com a menor distancia da imagem
int shortestDistance(struct Timage *image, struct Timage **tiles, 
int number_of_tiles){
	double distance, previous_distance = __DBL_MAX__;
	int shortest = 0;

	for (int i = 0; i < number_of_tiles; i++){
		distance = calculateDistance(tiles[i], image);
		if ( distance < previous_distance ){
			shortest			= i;
			previous_distance 	= distance;
		}
	}

	return shortest;
}


// calcula a distancia entre as duas imagens usando red mean
double calculateDistance(struct Timage *tile, struct Timage *image){
	double average_red, red, green, blue;
	
	average_red = (tile->averageR + image->averageR) / 2.0;

	red = (2.0 + average_red / 256.0) * pow((tile->averageR - image->averageR), 2.0);

	green = 4 * pow((tile->averageG - image->averageG), 2);

	blue = (2 + (255 - average_red)/256.0) * pow((tile->averageB - image->averageB), 2.0);

	return sqrt(red + green + blue);
}


// substitui pastilha na imagem de input
void replaceTile(struct Timage *mosaic_base, struct Timage *tile, 
int width_tile, int height_tile, int i, int j){
	int height_mosaic	= mosaic_base->height;
	int width_mosaic	= mosaic_base->width;

	// calcula o inicio do for com base no tamanho da pastilha e nas coordenadas
	// do bloco atual da imagem original
	int row_start 		= height_tile	* i;
	int column_start 	= width_tile 	* j;

	// calcula o final do for com base no inicio do for + a dimensao da pastilha
	int row_end 	= height_tile	+ height_tile	* i;
	int column_end 	= width_tile 	+ width_tile 	* j;

	int count = 0;
	for (int row = row_start; row < row_end && row < height_mosaic; row++){
		for (int col = column_start; col < column_end && col < width_mosaic; col++){
			mosaic_base->image[width_mosaic * row + col].r = tile->image[count].r;
			mosaic_base->image[width_mosaic * row + col].g = tile->image[count].g;
			mosaic_base->image[width_mosaic * row + col].b = tile->image[count].b;
			count++;
		}
	}
}

