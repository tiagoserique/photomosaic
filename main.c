#include "fotomosaico.h"

int main(int argc, char **argv){
	FILE* input  						= stdin;
	FILE* output 						= stdout;
	FILE* tempPastilha;
    
	struct dirent **nomesImagem 		= NULL;
	struct Timagem *imagemPrincipal		= NULL;
	struct Timagem **pastilhas			= NULL;
	
	char *diretorioPastilhas			= "./tiles/";
	char diretorioInicial[SIZE];
	
	int contPastilha					= 0;
	int opcao							= 0;
	int qtdPastilhas					= 0;
	int ret								= 0;
	opterr 								= 0;


	// usa getopt para filtrar as entradas do programa
	while ((opcao = getopt(argc, argv, "i:o:p:h::")) != -1){
		switch (opcao){
			// opcao de entrada
			case 'i':
				input = fopen(optarg, "r");
				if (!input){
					fprintf(stderr, "Erro ao abrir imagem de input\n");
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
		fprintf(stderr, "Erro ao tentar entrar no diretorio %s\n", diretorioPastilhas);
		exit(1);
	}


	// escaneia diretorio atras de imagens .ppm 
	// e coloca os nomes em nomesImagem
	fprintf(stderr, "Reading tiles from %s\n", diretorioPastilhas);
	qtdPastilhas = scandir(".", &nomesImagem, filtro, alphasort);
	if (qtdPastilhas < 0){
     	fprintf(stderr, "Nao ha imagens .ppm neste diretorio\n");
		exit(1);
	}
	fprintf(stderr, "%i tiles read\n", qtdPastilhas);


	// aloca vetor de ponteiros para as pastilhas que serao lidas
	pastilhas = alocaVetorPastilhas(qtdPastilhas);
	if (!pastilhas){
		fprintf(stderr, "Nao foi possivel alocar memoria\n");
		exit(1);
	}


	// faz leitura das pastilhas e as coloca em um vetor
	for (int i = 0; i < qtdPastilhas; i++, contPastilha++){
		// abre a pastilha
		tempPastilha = fopen(nomesImagem[i]->d_name, "r");
		if (!tempPastilha){
			fprintf(stderr, "Erro ao abrir arquivo\n");
			exit(1);
		}

		// recebe ponteiro apos pastilha ser alocada 
		pastilhas[contPastilha] = leImagem(tempPastilha);
		// verifica se foi retornado NULL
		if (!pastilhas[i])
			contPastilha--;
		
		if (contPastilha == 0){
			fprintf(stderr, "Tile size is %ix%i\n", pastilhas[0]->largura, 
					pastilhas[0]->altura);
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


	if (imagemPrincipal->largura < pastilhas[0]->largura 
	|| imagemPrincipal->altura < pastilhas[0]->altura){
		fprintf(stderr, "A imagem de input deve ser maior que as pastilhas\n");
		exit(1);
	}

	// constroi o mosaico
	fprintf(stderr, "Building mosaic image\n");
	constroiMosaico(imagemPrincipal, pastilhas, qtdPastilhas);
	

	// escreve o resultado no arquivo de saida 
	fprintf(stderr, "Writing output file\n");
	escreveImagem(imagemPrincipal, output);


	for (int i = 0; i < qtdPastilhas; i++)
		desalocaImagem(pastilhas[i]);
	free(pastilhas);

	desalocaImagem(imagemPrincipal);

	fclose(output);
	exit(0);
}
