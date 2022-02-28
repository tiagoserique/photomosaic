/* CI-1002 (ERE 2)
Prof. Carlos Maziero

Author:
Tiago Serique Valadares (GRR20195138)
*/

#include "photomosaic.h"

int main(int argc, char **argv){
	FILE* input = stdin;
	FILE* output = stdout;
	FILE* temp_tile;
    
	struct dirent **image_names = NULL;
	struct Timage *mosaic_base = NULL;
	struct Timage **tiles = NULL;
	
	char *tiles_directory				= "./tiles/";
	char home_directory[SIZE + 1];
	
	int tile_count = 0;
	int option = 0;
	int number_of_tiles = 0;
	int ret = 0;
	opterr = 0;


	// usa getopt para filtrar as entradas do programa
	while ( (option = getopt(argc, argv, "i:o:p:h::")) != -1 ){
		switch ( option ){
			// opcao de entrada
			case 'i':
				input = fopen(optarg, "r");
				if ( !input ){
					fprintf(stderr, "Not able to open the input image\n");
					exit(EXIT_FAILURE);
				}
			break;
			
			// opcao de saida
			case 'o':
				output = fopen(optarg, "w+");
				if ( !output ){
					fprintf(stderr, "Not able to open the output image\n");
					exit(EXIT_FAILURE);
				}
			break;
			
			// opcao para indicar diretorio de pastilhas
			case 'p':
				tiles_directory = optarg;
				break;

			// opcao de ajuda
			case 'h':
				fprintf(stderr, "Este programa cria fotomosaicos a partir de\n");
				fprintf(stderr, "uma imagem de input e um conjunto de imagens\n");
				fprintf(stderr, "menores, chamadas de pastilhas.\n");
				fprintf(stderr, "As imagens devem estar em formato ppm.\n");
				
				fprintf(stderr, "\nExemplo de uso:\n");
				fprintf(stderr, "mosaico -i input.ppm -o output.ppm\n");
				
				fprintf(stderr, "\nOpcoes:\n");
				fprintf(stderr, "Use -i para entrada em arquivo\n");
				fprintf(stderr, "Use -o para saida em arquivo\n");
				fprintf(stderr, "Use < para entrada em stdin\n");
				fprintf(stderr, "Use > para saida em stdout\n");
				fprintf(stderr, "Use -p para indicar diretorio de pastilhas\n");
				
				fprintf(stderr, "\n");
				fprintf(stderr, "\n");

				return EXIT_SUCCESS;
			break;
			
			// caso seja digitada uma opcao diferente
			case '?':
				if (optopt == 'i' || optopt == 'o' || optopt == 'p')
					fprintf(stderr, "Opcao -%c requer um argumento.\n", optopt);
				else if (isprint (optopt))
					fprintf(stderr, "Opcao desconhecida `-%c'.\n", optopt);
				else
					fprintf(stderr, "Caracter de opcao desconhecido`\\x%x'.\n",
							optopt);
			return 1;
			
			default:
				abort();
		}
	}


	// pega o diretorio inicial em que o programa esta rodando
	getcwd(home_directory, sizeof(home_directory));


	// muda o diretorio de executacao do programa e checa retorno
	ret = chdir(tiles_directory);
	if ( ret != 0 ){
		fprintf(stderr, "Not able to change directory %s\n", tiles_directory);
		exit(EXIT_FAILURE);
	}


	// escaneia diretorio atras de imagens .ppm 
	// e coloca os nomes em image_names
	fprintf(stderr, "Reading tiles from %s\n", tiles_directory);
	number_of_tiles = scandir(".", &image_names, filter, alphasort);
	if ( number_of_tiles < 0 ){
     	fprintf(stderr, "There's not .ppm images in this directory\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "%i tiles read\n", number_of_tiles);


	// aloca vetor de ponteiros para as pastilhas que serao lidas
	tiles = tilesArrayAlloc(number_of_tiles);
	if ( !tiles ){
		fprintf(stderr, "Nao foi possivel alocar memoria\n");
		exit(EXIT_FAILURE);
	}


	// faz leitura das pastilhas e as coloca em um vetor
	for (int i = 0; i < number_of_tiles; i++, tile_count++){
		// abre a pastilha
		temp_tile = fopen(image_names[i]->d_name, "r");
		if ( !temp_tile ){
			fprintf(stderr, "Erro ao abrir arquivo\n");
			exit(EXIT_FAILURE);
		}

		// recebe ponteiro apos pastilha ser alocada 
		tiles[tile_count] = readImage(temp_tile);
		// verifica se foi retornado NULL
		if ( !tiles[i] )
			tile_count--;
		
		if ( tile_count == 0 ){
			fprintf(stderr, "Tile size is %ix%i\n", tiles[0]->width, 
					tiles[0]->height);
			fprintf(stderr, "Calculating tiles' average colors\n");
		}

		// fecha a pastilha
		fclose(temp_tile);

		// libera memoria da string com nome do arquivo apos usado
		free(image_names[i]);

	}
	// libera memoria do vetor de strings
	free(image_names);
 

	// muda o diretorio de executacao do programa e checa retorno
	ret = chdir(home_directory);
	if ( ret != 0 ){
		fprintf(stderr, "Erro ao entrar no diretorio %s\n", home_directory);
		exit(EXIT_FAILURE);
	}


	// le as propriedades da imagem de entrada
	fprintf(stderr, "Reading input image\n");
	mosaic_base = readImage(input);
	fclose(input);
	fprintf(stderr, "Input image PPM %s, %ix%i pixels\n", mosaic_base->formato, 
		mosaic_base->width, mosaic_base->height);


	if ( mosaic_base->width < tiles[0]->width 
	|| mosaic_base->height < tiles[0]->height ){
		fprintf(stderr, "A imagem de input deve ser maior que as pastilhas\n");
		exit(EXIT_FAILURE);
	}


	// constroi o mosaico
	fprintf(stderr, "Building mosaic image\n");
	buildMosaic(mosaic_base, tiles, number_of_tiles);
	

	// escreve o resultado no arquivo de saida 
	fprintf(stderr, "Writing output file\n");
	writeImage(mosaic_base, output);


	// libera memoria das pastilhas
	for (int i = 0; i < number_of_tiles; i++)
		imageDealloc(tiles[i]);
	free(tiles);


	// libera memoria da imagem principal
	imageDealloc(mosaic_base);

	fclose(output);
	exit(EXIT_SUCCESS);
}
