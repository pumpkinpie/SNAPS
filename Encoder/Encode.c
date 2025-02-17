#include <stdio.h>
#include <stdlib.h>
#include "ejpgl.h"
#include "Encode.h"
#define __I386
#define __LITTLEENDIAN

INFOHEADER _bmpheader;
INFOHEADER *bmpheader;
JPEGHEADER _jpegheader;
long st, en;

FILE *infile, *outfile;
static unsigned char buffer[MACRO_BLOCK_SIZE*3];


void writejpegfooter()
{
       unsigned char footer[2];
       footer[0] = 0xff;
       footer[1] = 0xd9;
//       fseek(file,0,SEEK_END);
       fwrite(footer,sizeof(footer),1,outfile);
	return;

}

void readbmpfile(signed char pixelmatrix[MACRO_BLOCK_SIZE][MACRO_BLOCK_SIZE*3], unsigned int mrow, unsigned int mcol, INFOHEADER * header)
{
	unsigned int row, col;
	for(row = 0;row < MACRO_BLOCK_SIZE; row++) {
             	//Find first point of row in the matrix to be read.
		fseek(infile,-(3*header->width*(row + 1 + mrow*MACRO_BLOCK_SIZE)-(MACRO_BLOCK_SIZE*3)*mcol),SEEK_END);
             	//Read row from matrix
		fread(buffer, 1, MACRO_BLOCK_SIZE*3, infile);
		for(col = 0; col < MACRO_BLOCK_SIZE*3; col++) {
			pixelmatrix[row][col] = buffer[col]- 128;
		}
	}
	return;

 }


int openBMPJPG(int argc, char* bmpfilename, char* jpgfilename) {
	int jpegheadersize;
        argc = 2;
	if (argc<1) {
		printf("USAGE: bmp2jpg source_file destination_file\n");
              return 0;
  	}

 	bmpheader=&_bmpheader;
 
  	infile = fopen(bmpfilename,"rb");
  	if (infile == NULL) {
          	printf("Input file %s does not exist!\n\nUSAGE: jpegcodec source_file destination_file [/E] [compression_rate]\n", bmpfilename);
		exit(0);
  		}

  	outfile = fopen(jpgfilename,"wb");
  	if (outfile == NULL) {
          	printf("Output file %s does not work!\n\nUSAGE: jpegcodec source_file destination_file [/E] [compression_rate]\n", jpgfilename);
		exit(0);
  		}

  	if (getbmpheader(bmpheader) == 0) {
       	printf("\n%s is not a valid BMP-file",bmpfilename);
		exit(0);
  	}

    	jpegheadersize = writejpegheader(bmpheader, &_jpegheader);
	if (jpegheadersize == 0) {
       	printf("\nerror in writing jpg header");
		exit(0);
		}

    	fwrite(&_jpegheader,jpegheadersize,1,outfile);

  	//QueryPerformanceCounter(&st);

  	return 1;

}

int getbmpheader(INFOHEADER *header)
{
        int retval;
        unsigned char buffer[4];

        fseek(infile,14,SEEK_SET);
        fread(header, sizeof(INFOHEADER), 1, infile);
        fseek(infile, SEEK_CUR, 1024);

	 retval = 1;

	printf("\nImage width: %d pixels", bmpheader->width);
       printf("\nImage height: %d pixels", bmpheader->height);

       return retval;

}

void get_MB(int mb_row, int mb_col, signed char pixelmatrix[MACRO_BLOCK_SIZE][MACRO_BLOCK_SIZE*3]) {

    	readbmpfile(pixelmatrix, mb_row, mb_col,bmpheader);

}



int closeBMPJPG() {
  	unsigned int col, cols, row, rows;

       //QueryPerformanceCounter(&en);
       //printf("\nExecution time: %f seconds",(double)(en.QuadPart-st.QuadPart)/1000000);

	rows = bmpheader->height>>4;
       cols = bmpheader->width>>4;
       printf("\nProcessed more than %d %dx%d-blocks.",(rows-1)*cols,MATRIX_SIZE,MATRIX_SIZE);  // +col

     	writejpegfooter();

        fclose(outfile);
        fclose(infile);

	 return 0;

}



void put_char(unsigned char c) {

	fwrite(&c, 1, 1, outfile);

}



