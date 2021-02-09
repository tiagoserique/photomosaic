#include "fotomosaico.h"

#define LINESIZE 1024

int main(int argc, char **argv){
	char *input = NULL;
	char *output = NULL;
	char *diretorio = "./tiles/";
	int index;
	int opcao;

	opterr = 0;


	while ((opcao = getopt (argc, argv, "i:o:p:h::")) != -1){
		switch (opcao){
			case 'i':
				input = optarg;
				break;
			
			case 'o':
				output = optarg;
				break;
			
			case 'p':
				diretorio = optarg;
				break;

			case 'h':
				fprintf(stderr, "\n");
				fprintf(stderr, "Use -i para entrada em arquivo\n");
				fprintf(stderr, "Use -o para saida em arquivo\n");
				fprintf(stderr, "Use < para entrada em stdin\n");
				fprintf(stderr, "Use > para saida em stdout\n");
				return 0;
				break;
			
			case '?':
				if (optopt == 'i' || optopt == 'o' || optopt == 'p')
					fprintf (stderr, "Opcao -%c requer um argumento.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Opcao desconhecida `-%c'.\n", optopt);
				else
					fprintf (stderr, "Caracter de opcao desconhecido`\\x%x'.\n",
							optopt);
				return 1;
			default:
				abort ();
		}
	}

	printf ("input = %s\n output = %s\n diretorio = %s\n", input, output, diretorio);

	for (index = optind; index < argc; index++)
	printf ("Non-option argument %s\n", argv[index]);

	
	escreveImagem(leImagem(), output);


	// DIR *dirstream;
    // struct dirent *direntry;

	// // abre o diretório com as pastilhas
    // dirstream = opendir ("./");
    // if ( ! dirstream ){
    //   fprintf(stderr, "Couldn't open the directory");
    //   exit (1) ;
    // }

	// // varre as entradas do diretório aberto
    // for (;;){
	//   	// pega a próxima entrada
	// 	direntry = readdir(dirstream) ;
	
	// 	// se for nula, encerra a varredura
	// 	if (! direntry)
	// 		break ;
	
	// 	// seleciona o conteúdo da entrada
	// 	printf("%s\t", direntry->d_name);
	// 	switch(direntry->d_type){
	// 		case DT_REG:
	// 			printf (" (arquivo)\n") ;
	// 			break ;
	// 		default:
	// 			continue;
	// 	}
    // }

	// leImagem();

    // fecha o diretório
    // closedir(dirstream);

	exit(0);
}