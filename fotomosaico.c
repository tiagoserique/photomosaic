#include "fotomosaico.h"

struct imagem leImagem(FILE* arq){
	
	struct imagem pastilha;
	int ret;
	
	retiraComentarios(arq);

	fscanf(arq, "%s", pastilha.formato);
	if ( !strcpy(pastilha.formato, "P3") ){
		perror("Formato errado");
		fclose(arq);
		exit(1);
	}
	printf("Formato: %s\n", pastilha.formato);

	ignoraLinhaEmBranco(arq);

	ret = fscanf(arq, "%d %d", &pastilha.largura, &pastilha.altura);
	if (ret != 2){
		perror("Erro ao pegar tamanho da imagem");
		fclose(arq);
		exit(1);
	}
	printf("Largura: %d, Altura: %d\n", pastilha.largura, pastilha.altura);

	ret = fscanf(arq, "%d", &pastilha.valorMax);
	if ( ret != 1){
		perror("Erro ao pegar valor maximo dos componentes");
		exit(1);
	}
	printf("Valor maximo cor: %d\n", pastilha.valorMax);

	// mostraArquivo(arq);

	pastilha.pixels = (struct pixelP3 *)malloc(sizeof(struct pixelP3) * 
						pastilha.largura * pastilha.altura);
	
	if ( !pastilha.pixels ){
		perror("Erro ao alocar memoria");
		fclose(arq);
		exit(1);
	}

	for (int i = 0; i < (pastilha.largura * pastilha.altura); i++){
		fscanf(arq, "%d ", &pastilha.pixels[i].r);
		fscanf(arq, "%d ", &pastilha.pixels[i].g);
		fscanf(arq, "%d ", &pastilha.pixels[i].b);
	}

	fclose(arq);
	return pastilha;
}


// Varre o arquivo por procurando "#" e substitui por '\0'
void retiraComentarios(FILE* arq){
	char line[20];

	fgets(line, 20, arq);
	while ( ! feof(arq) ){
		line[strcspn(line, "#")] = '\0' ;
		fgets(line, 20, arq);
	}
	rewind(arq);
}


void mostraArquivo(FILE* arq){
	char line[1024];

	fgets(line, 1024, arq);
	while( !feof(arq) ){
		printf("%s\n", line);		
		fgets(line, 1024, arq);
	}
}


void ignoraLinhaEmBranco(FILE* arq){
	char line[20];
	
	fgets(line, 20, arq);
	while( line[0] == '\0' || !strcmp(line,"\n")){
		fgets(line, 20, arq);
	};
}


