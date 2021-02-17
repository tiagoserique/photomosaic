#include "fotomosaico.h"

#define LINESIZE 1024

int main(int argc, char **argv){
	FILE* input  						= stdin;
	FILE* output 						= stdout;
	FILE* tempPastilha;
    
	struct dirent **nomesImagem 		= NULL;
	struct Timagem *imagemPrincipal		= NULL;
	struct Timagem **vetPastilha		= NULL;
	
	char *diretorioPastilhas			= "./tiles/";
	char diretorioInicial[SIZE];
	
	int contPastilha					= 0;
	int opcao							= 0;
	int qtdImagens						= 0;
	int ret								= 0;
	opterr 								= 0;


	// usa getopt para filtrar as entradas do programa
	while ((opcao = getopt(argc, argv, "i:o:p:h::")) != -1){
		switch (opcao){
			// opcao de entrada
			case 'i':
				input = fopen(optarg, "r");
				if (!input){
					fprintf(stderr, "Erro ao abrir imagem principal\n");
					exit(1);
				}
			break;
			
			// opcao de saida
			case 'o':
				output = fopen(optarg, "w+");
				if (!output){
					fprintf(stderr, "Erro ao abrir arquivo de saida\n");
					exit(1);
				}
			break;
			
			// opcao para indicar diretorio de pastilhas
			case 'p':
				diretorioPastilhas = optarg;
				break;

			// opcao de ajuda
			case 'h':
				fprintf(stderr, "\n");
				fprintf(stderr, "Use -i para entrada em arquivo\n");
				fprintf(stderr, "Use -o para saida em arquivo\n");
				fprintf(stderr, "Use < para entrada em stdin\n");
				fprintf(stderr, "Use > para saida em stdout\n");
				return 0;
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
	getcwd(diretorioInicial, sizeof(diretorioInicial));


	// muda o diretorio de executacao do programa e checa retorno
	ret = chdir(diretorioPastilhas);
	if (ret != 0){
		fprintf(stderr, "Erro ao entrar no diretorio %s\n", diretorioPastilhas);
		exit(1);
	}


	// escaneia diretorio atras de imagens .ppm 
	// e coloca os nomes em nomesImagem
	fprintf(stderr, "Reading tiles from %s\n", diretorioPastilhas);
	qtdImagens = scandir(".", &nomesImagem, filtro, alphasort);
	if (qtdImagens < 0){
     	fprintf(stderr, "Nao ha imagens .ppm neste diretorio\n");
		exit(1);
	}
	fprintf(stderr, "%i tiles read\n", qtdImagens);


	// aloca vetor de ponteiros para as pastilhas que serao lidas
	vetPastilha = malloc(sizeof(struct Timagem*) * qtdImagens);
	if (!vetPastilha){
		fprintf(stderr, "Nao foi possivel alocar memoria\n");
		exit(1);
	}

	for (int i = 0; i < qtdImagens; i++, contPastilha++){
		// abre a pastilha
		tempPastilha = fopen(nomesImagem[i]->d_name, "r");
		if (!tempPastilha){
			fprintf(stderr, "Erro ao abrir arquivo\n");
			exit(1);
		}

		// recebe ponteiro apos pastilha ser alocada 
		vetPastilha[contPastilha] = leImagem(tempPastilha);
		// verifica se foi retornado NULL
		if (!vetPastilha[i])
			contPastilha--;
		
		if (contPastilha == 0){
			fprintf(stderr, "Tile size is %ix%i\n", vetPastilha[0]->largura, 
					vetPastilha[0]->altura);
			fprintf(stderr, "Calculating tiles' average colors\n");
		}

		// fecha a pastilha
		fclose(tempPastilha);

		// libera memoria da string com nome do arquivo apos usado
		free(nomesImagem[i]);

	}
	// libera memoria do vetor de strings
	free(nomesImagem);
 

	// muda o diretorio de executacao do programa e checa retorno
	ret = chdir(diretorioInicial);
	if (ret != 0){
		fprintf(stderr, "Erro ao entrar no diretorio %s\n", diretorioInicial);
		exit(1);
	}


	// le as propriedades da imagem de entrada
	fprintf(stderr, "Reading input image\n");
	imagemPrincipal = leImagem(input);
	fclose(input);
	fprintf(stderr, "Input image PPM %s, %ix%i pixels\n", imagemPrincipal->formato, 
		imagemPrincipal->largura, imagemPrincipal->altura);


	// constroi o mosaico
	fprintf(stderr, "Building mosaic image\n");
	
	
	// escreve o resultado no arquivo de saida 
	fprintf(stderr, "Writing output file\n");
	escreveImagem(imagemPrincipal, output);


	// 
	for (int i = 0; i < qtdImagens; i++)
		desalocaImagem(vetPastilha[i]);
	free(vetPastilha);

	desalocaImagem(imagemPrincipal);

	fclose(output);
	exit(0);
}

// fprintf(stderr, "cheguei aq\n"); usar pra debugar
