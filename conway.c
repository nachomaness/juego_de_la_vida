#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
//#include "funciones.h"

#define PIXEL_WIDTH 50
#define PIXEL_HEIGHT 50

/* aca dejo un parser si los datos de iteraciones columnas y filas estan en un archivo
int parsear_datos(const char* nombre_arch, unsigned int* nfil
				, unsigned int* ncol, unsigned int* niter){
	FILE* archp = fopen(nombre_arch, "r");
	unsigned int i;
	char buffer[5];
	char char_actual;
	if(archp!=NULL){
		while(fgetc(archp) != ' '){
			i = 0;
			char_actual = fgetc(archp);
			buffer[i] = char_actual;
			i++;
		}
		while(i<6){
			buffer[i]=' ';
			i++;
		}
		*nfil = atoi(buffer);
		while(fgetc(archp) != ' '){
			i = 0;
			char_actual = fgetc(archp);
			buffer[i] = char_actual;
			i++;
		}
		while(i<6){
			buffer[i]=' ';
			i++;
		}
		*ncol = atoi(buffer);
		while(fgetc(archp) != ' '){
			i = 0;
			char_actual = fgetc(archp);
			buffer[i] = char_actual;
			i++;
		}
		while(i<6){
			buffer[i]=' ';
			i++;
		}
		*niter = atoi(buffer);
		fclose(archp);
		return 0;
	}else{
		return 1;
	}
}*/

void inicializar_valores(unsigned int nfil, unsigned int ncol, unsigned char* matriz){
	int i = 0, j = 0;
	for(; i<nfil; i++){
		j = 0;
		for(; j<ncol; j++){
			matriz[i*ncol+j]='-';
		}
	}
}

int parsear_posiciones (const char* nombre_arch, unsigned int nfil
				, unsigned int ncol, unsigned char* matriz){
	FILE* archp = fopen(nombre_arch, "r");
	if(archp!=NULL){
		printf("Leyendo estado inicial...\n");
		unsigned int i;
		char buffer[5];
		char char_actual;
		int x;
		int y;
		inicializar_valores(nfil, ncol, matriz);
		char_actual = fgetc(archp);
		while (!(feof(archp))){
			i = 0;
			while(char_actual != ' '){
				buffer[i] = char_actual;
				char_actual = fgetc(archp);
				i++;
			}
			while(i<5){
				buffer[i]=' ';
				i++;
			}
			x=atoi(buffer);
			char_actual = fgetc(archp);
			i = 0;
			while(char_actual != '\n'){
				buffer[i] = char_actual;
				char_actual = fgetc(archp);
				i++;
			}
			while(i<5){
				buffer[i]=' ';
				i++;
			}
			y=atoi(buffer);
			if (y<ncol && x<nfil){
				matriz[(y-1)*ncol+(x-1)]='X';
			}else{
				fprintf(stderr, "Posicion invalida\n");
			}
			char_actual = fgetc(archp);
		}
		fclose(archp);
		return 0;
	}else{
		return 1;
	}
}

int obtener_valor_numerico(unsigned int fil, unsigned int col, unsigned int ncol, unsigned char* matriz) {
    unsigned char value = matriz[fil*ncol+col];
    if (value == '-') {
        return 0;
    } else {
        return 1;
    }
}

void mostrar_matriz(int nfil,int ncol, unsigned char* matriz){
	int i = 0, j= 0 ;
	for(;i<nfil;i++){
		j = 0;
		for(;j<ncol;j++){
			printf("%c",matriz[i*ncol+j]);
		}
		printf("\n");
	}
	printf("\n");
}

void escribir_pbm(int nfil, int ncol, unsigned char* matriz, const char* nombre_arch, int iterac) {
    int width = PIXEL_WIDTH;
    int height = PIXEL_HEIGHT;

    char buffer[30];
    snprintf(buffer,sizeof(buffer),"%s_%d.pbm", nombre_arch,iterac);
    printf("Grabando %s\n",buffer);
    mostrar_matriz(nfil, ncol, matriz);

    FILE *bit_map = fopen(buffer ,"w");
    fprintf(bit_map, "%s\n", "P1"); // Header
    fprintf(bit_map, "%d %d\n",ncol * PIXEL_WIDTH, nfil*PIXEL_HEIGHT); // Width and Height
    int i, j, pixY, pixX;
    for (i = 0; i < nfil; i++) { // Writing the image
        for (pixY = 0; pixY < height; pixY++) {
            for (j = 0; j < ncol; j++) {
                for (pixX = 0; pixX < width; pixX++) {
                    unsigned int valor = obtener_valor_numerico(i, j, ncol, matriz);
                    fprintf(bit_map, "%d ", valor);
                }
            }
            putc('\n', bit_map);
        }
    }
    fclose( bit_map );
}

extern unsigned int vecinos(unsigned char *a, unsigned int i, unsigned int j,
											unsigned int M, unsigned int N);

void iterar_matriz(int niterac, int nfil, int ncol, unsigned char* matriz, const char* nombre_arch){
	char matriz_aux[nfil][ncol];
	int ix, iy, vivos;
	for(ix=0;ix<nfil;ix++){
		for(iy=0;iy<ncol;iy++){
			matriz_aux[ix][iy]=matriz[ix*ncol+iy];
		}
	}
	int i =0;
	for(; i<niterac; i++){
		for(ix=0; ix<nfil; ix++)
		{
			for(iy=0; iy<ncol; iy++)
	  		{//Vecinos inicio
	  			vivos = vecinos(matriz, ix, iy, nfil, ncol);
		 	  	//condicional para determinar si la casilla vive o muere
		 	  	if(vivos<2 || vivos>3)
		   		{
	    			matriz_aux[ix][iy]='-';
	    		}else if(vivos==3 && matriz_aux[ix][iy]=='-'){
		    	// esta muerto
		       		matriz_aux[ix][iy]='X';
		    	}
		  	}// final del for iy
		} // final del for ix
	for(ix=0;ix<nfil;ix++){
		for(iy=0;iy<ncol;iy++){
			matriz[ix*ncol+iy]=matriz_aux[ix][iy];
		}
	}
	escribir_pbm(nfil, ncol, matriz, nombre_arch, i+1);
	}
}

extern void version();

int main(int argc, char** argv){

	/* si las columnas, filas e iteraciones se cargan de un archivo
	if(argc == 3){
		unsigned int nfilas;
		unsigned int ncolumnas;
		unsigned int niterac;
		int estado = parsear_datos(argv[1], &nfilas, &ncolumnas, &niterac);
		if(estado==0){
			char* matriz = malloc(ncolumnas * nfilas * sizeof(char));
			estado = parsear_posiciones(argv[2], nfilas, ncolumnas, matriz);
			if (estado == 0){
				printf("Estado inicial\n");
				mostrar_matriz(nfilas, ncolumnas, matriz);
				escribir_pbm(nfilas, ncolumnas, matriz);
				iterar_matriz(niterac, nfilas, ncolumnas, matriz);
				free(matriz);
				return 0;
			}else{
			fprintf(stderr,"Error al abrir el archivo %s\n",argv[1]);
				return 1;
			}
		}else{
			fprintf(stderr,"Error al abrir el archivo %s\n",argv[2]);
			return 1;
		}*/
	if(argc == 2){
		if(strcmp(argv[1],"-h")==0){
			printf("Uso:\n");
			printf("conway -h\n");
			printf("conway -V\n");
			printf("conway i M N inputfile [-o outputprefix]\n");
			printf("Opciones:\n");
			printf("-h, --help    Imprime este mensaje.\n");
			printf("-V, --version Da la versión del programa.\n");
			printf("-o Prefijo de los archivos de salida.\n");
			return 0;
		}else if(strcmp(argv[1],"-V")==0){
			version();
			return 0;
		}else{
			fprintf(stderr, "Error de argumentos\n");
			return 1;
		}
	}else if(argc == 5){
		unsigned int niterac = atoi(argv[1]);
		unsigned int nfilas = atoi(argv[2]);
		unsigned int ncolumnas = atoi(argv[3]);
		unsigned char* matriz = malloc(ncolumnas * nfilas * sizeof(char));
		int estado = parsear_posiciones(argv[4], nfilas, ncolumnas, matriz);
		if(estado==0){
			escribir_pbm(nfilas, ncolumnas, matriz, argv[4], 0);
			iterar_matriz(niterac, nfilas, ncolumnas, matriz, argv[4]);
  			free(matriz);
  			printf("Listo\n");
  			return 0;
  		}else{
			fprintf(stderr,"Error al abrir el archivo %s\n",argv[4]);
			return 1;
		}
	}else if(argc == 7 && (strcmp(argv[5],"-o"))==0){
		unsigned int niterac = atoi(argv[1]);
		unsigned int nfilas = atoi(argv[2]);
		unsigned int ncolumnas = atoi(argv[3]);
		unsigned char* matriz = malloc(ncolumnas * nfilas * sizeof(char));
		int estado = parsear_posiciones(argv[4], nfilas, ncolumnas, matriz);
		if(estado==0){
			escribir_pbm(nfilas, ncolumnas, matriz, argv[6], 0);
			iterar_matriz(niterac, nfilas, ncolumnas, matriz, argv[6]);
  			free(matriz);
  			printf("Listo\n");
  			return 0;
  		}else{
			fprintf(stderr,"Error al abrir el archivo %s\n",argv[4]);
			return 1;
		}
	}else{
		fprintf(stderr, "Error de argumentos\n");
		return 1;
	}
}

