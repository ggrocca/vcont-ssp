#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CImg.h"

void writeTriple(FILE* f, int a, int b, int c){
	fprintf( f, "%d %d %d\n", a, b, c);
}

int main(int argc, char **argv) {

	if(argc==4){
		printf("\n*** From PNG to Mesh Converter ***\n");
		printf("  source filename: %s\n", argv[1]);		
		printf("  output filename: %s\n", argv[2]);
		printf("  color range filename: %s\n", argv[3]);

		cimg_library::CImg<unsigned char>* img = new cimg_library::CImg<unsigned char>(argv[1]);
		cimg_library::CImg<unsigned char> image = *(img);
		FILE* f = fopen(argv[2],"w");
		int min=255;
		int max=0;
		int z;

		// ***** Vertici *****
		fprintf( f, "%d\n", img->width() * img->height() );

		for(int x=0; x<img->width(); x++){
			for(int y=img->height()-1; y>=0; y--){
				// modificato per velocizzare il processo
				z = image(x,y,0,0);
				if(z < min) min=z;
				if(z > max) max=z;
				writeTriple(f, x, y, z); //getImagePixelValue(img, x, y));
			}
		}


		// ***** Triangoli *****
		fprintf( f, "%d\n", (img->width()-1) * (img->height()-1) * 2 );

		for(int x=0; x<img->width(); x++){
			for(int y=img->height()-1; y>=0; y--){

				// DIAGONALE NO-SE
				
				if(x>0 && y<img->height()-1){
					writeTriple(f, x*img->height()+y, x*img->height()+(y+1), (x-1)*img->height()+y);					
				}
				if(y>0 && x<img->width()-1){
					writeTriple(f, x*img->height()+y, x*img->height()+(y-1), (x+1)*img->height()+y);					
				}
				

				// DIAGONALE NE-SO
				/*
				if(x>0 && y>0){
					writeTriple(f, x*img->height()+y, (x-1)*img->height()+y, x*img->height()+(y-1));					
				}
				if(y<img->height()-1 && x<img->width()-1){
					writeTriple(f, x*img->height()+y, (x+1)*img->height()+y, x*img->height()+(y+1));					
				}
				*/
			}
		}

		fclose(f);

		f = fopen(argv[3],"a");
		fprintf( f, "%d %d\n", min, max);
		fclose(f);

		printf("*** Convertion completed ***\n\n");
	}

	return 1;
}

