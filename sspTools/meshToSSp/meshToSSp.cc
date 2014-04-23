#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SEED_NUMBER 563921 

double randValue(){
	return (((double) rand()) / RAND_MAX) * 0.8;
}

void writeInt(FILE* f, int n){
	fwrite (&n, 4, 1, f);
}

void writeDouble(FILE* f, double d){
	fwrite (&d, 8, 1, f);
}

int main(int argc, char **argv) {

	// Inizializza il Random Seed
	srand (SEED_NUMBER);

	if(argc==6){
		printf("\n*** From Mesh Sequence to SSP Converter ***\n");
		printf("  Max Index: %s\n", argv[1]);
		printf("  source filename: %s\n", argv[2]);		
		printf("  Bauer output filename: %s\n", argv[3]);
		printf("  Gauss output filename: %s\n", argv[4]);
		printf("  color range filename: %s\n", argv[5]);

		int maxIndex = atoi(argv[1]);
		int totalImages = 0;

		// Conta le immagini come farebbe nel ciclo successivo
		for(int i=0; pow(2,i/2.0)<=maxIndex; i++){
			totalImages++;
		}
		printf("  Total Images: %d\n", totalImages);

		// Inizializza ed apre i file
		char str[1024];
		strcpy (str,argv[2]);
		int filenameLen = strlen(str);
		FILE* r;
		FILE* bauer = fopen(argv[3],"w");
		FILE* gauss = fopen(argv[4],"w");
		FILE* ranges = fopen(argv[5],"r");
		int rMin, rMax;
		int trash;

		// Scrive il totale delle immagini
		writeInt(bauer, totalImages);		

		for(int i=0; pow(2,i/2.0)<=maxIndex; i++){

			if(i==0){ sprintf(&str[filenameLen], "0.tri"); }
			else{ sprintf(&str[filenameLen], "%d.tri", (int)round(pow(2,i/2.0))); }
			printf("Loading %s ...\n", str);

			// Apre il file
			r = fopen(str,"r");
			int vertices;
			trash = fscanf(r, "%d\n", &vertices);

			// Cerca minimo e massimo
			double x,y,z,min,max;
			if(vertices>0){
				trash = fscanf(r, "%lf %lf %lf\n", &x, &y, &z);
				min=max=z;
			}
			for(int j=1; j<vertices; j++){
				trash = fscanf(r, "%lf %lf %lf\n", &x, &y, &z);
				if(min>z) min=z;
				if(max<z) max=z;
			}

			// Carica il range dell'immagine originale
			trash = fscanf(ranges, "%d %d\n", &rMin, &rMax);

			// Definisce il mult
			double mult = (double)(rMax-rMin)/(max-min);
			fclose(r);

			// Legge il file
			r = fopen(str,"r");
			trash = fscanf(r, "%d\n", &vertices);
			int square = sqrt(vertices);
			writeInt(bauer, square);
			writeInt(bauer, square);

			if(i==0){
				writeInt(gauss, square);
				writeInt(gauss, square);
			}

			for(int j=0; j<vertices; j++){
				trash = fscanf(r, "%lf %lf %lf\n", &x, &y, &z);
				z += randValue();
				writeDouble(bauer, (double)rMin + ((z-min)*mult));
				if(i==0){
					writeDouble(gauss, (double)rMin + ((z-min)*mult));
				}
			}

			fclose(r);
		}

		fclose(bauer);
		fclose(gauss);
		fclose(ranges);
	}

	return 1;
}

