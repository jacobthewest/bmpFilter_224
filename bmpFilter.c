#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define BAD_NUMBER_ARGS 1
#define FSEEK_ERROR 2
#define FREAD_ERROR 3
#define MALLOC_ERROR 4
#define FWRITE_ERROR 5                          

/**
 * Parses the command line.
 *
 * argc:      the number of items on the command line (and length of the
 *            argv array) including the executable
 * argv:      the array of arguments as strings (char* array)
 * grayscale: the integer value is set to TRUE if grayscale output indicated
 *            outherwise FALSE for threshold output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *isGrayscale) {
  if (argc > 2) {
    printf("Usage: %s [-g]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }
  
  if (argc == 2 && strcmp(argv[1], "-g") == 0) {
    *isGrayscale = TRUE;
  } else {
    *isGrayscale = FALSE;
  }

  return stdin;
}

unsigned getFileSizeInBytes(FILE* stream) {
  unsigned fileSizeInBytes = 0;
  
  rewind(stream);
  if (fseek(stream, 0L, SEEK_END) != 0) {
    exit(FSEEK_ERROR);
  }
  fileSizeInBytes = ftell(stream);

  return fileSizeInBytes;
}

void getBmpFileAsBytes(unsigned char* ptr, unsigned fileSizeInBytes, FILE* stream) {
  rewind(stream);
  if (fread(ptr, fileSizeInBytes, 1, stream) != 1) {
#ifdef DEBUG
    printf("feof() = %x\n", feof(stream));
    printf("ferror() = %x\n", ferror(stream));
#endif
    exit(FREAD_ERROR);
  }
}

unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red) {
  printf("TODO: unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red)\n");
  return 0;
}

void applyGrayscaleToPixel(unsigned char* pixel) {
  printf("TODO: void applyGrayscaleToPixel(unsigned char* pixel)\n");
}

void applyThresholdToPixel(unsigned char* pixel) {
  printf("TODO: void applyThresholdToPixel(unsigned char* pixel)\n");
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
  printf("TODO: void applyFilterToPixel(unsigned char* pixel, int isGrayscale)\n");
}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
  printf("TODO: void applyFilterToRow(unsigned char* row, int width, int isGrayscale)\n");
}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
  printf("TODO: void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale)\n");
}

int getOffset(char* offset) {
  return 0;
}


void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale, FILE* stream) {

  fseek(stream, 0, SEEK_SET ); // stream is the file we are in. 0 sets it to where we want to read from SEEK_SET. SEEK_SET
                               // is the beginning of the file? Idk. I just know that 0 says where in the file we want to 
                               // start reading from.  

  // bmp Header variables
  unsigned char headerField[2];
  unsigned int fileSize;
  unsigned int dummyStorage;
  unsigned int offsetFirstBytePixelArray = 0;

  // --------------------- bmp Header--------------------------//
  fread(headerField, 2, 1, stream);
  fread(&fileSize, 4, 1, stream);
  fread(&dummyStorage, 4, 1, stream);
  fread(&offsetFirstBytePixelArray, 4, 1, stream);



  // DIB Header variables
  unsigned int dibHeaderSize;
  unsigned int width = 0; 
  unsigned int height = 0;
  unsigned int numColorPlanes;
  unsigned int numBitsPerPixel;
  unsigned int copmressionMethod;
  unsigned int imageSizeInBytes;
  unsigned int horizRes;
  unsigned int vertRes;
  unsigned int numColorsInPallete;
  unsigned int numImportantColors;

  // --------------------- DIB Header--------------------------//
  fread(&dibHeaderSize, 4, 1, stream);
  fread(&width, 4, 1, stream);
  fread(&height, 4, 1, stream);
  fread(&numColorPlanes, 2, 1, stream);
  fread(&numBitsPerPixel, 2, 1, stream);
  fread(&copmressionMethod, 4, 1, stream);
  fread(&imageSizeInBytes, 4, 1, stream);
  fread(&horizRes, 4, 1, stream);
  fread(&vertRes, 4, 1, stream);
  fread(&numColorsInPallete, 4, 1, stream);
  fread(&numImportantColors, 4, 1, stream);



  // Post-headers pixel array
  unsigned char* pixelArray = NULL;

  printf("%s %ui", "Here is the pixelOfset: ", offsetFirstBytePixelArray);

  // printf("HF %s\n\t", headerField);
  // printf("fileSize %i\n\t", fileSize);
  // printf("dummyStorage %i\n\t", dummyStorage);
  // printf("offset %i\n\t", offsetFirstBytePixelArray);


  printf("TODO: set offsetFirstBytePixelArray\n");
  printf("TODO: set width\n");
  printf("TODO: set height\n");
  printf("TODO: set the pixelArray to the start of the pixel array\n");

#ifdef DEBUG
  printf("offsetFirstBytePixelArray = %u\n", offsetFirstBytePixelArray);
  printf("width = %u\n", width);
  printf("height = %u\n", height);
  printf("pixelArray = %p\n", pixelArray);
#endif

  applyFilterToPixelArray(pixelArray, width, height, isGrayscale);
}

int main(int argc, char **argv) {
  int grayscale = FALSE;
  unsigned fileSizeInBytes = 0;
  unsigned char* bmpFileAsBytes = NULL;
  FILE *stream = NULL;
  
  stream = parseCommandLine(argc, argv, &grayscale);
  fileSizeInBytes = getFileSizeInBytes(stream);

#ifdef DEBUG
  printf("fileSizeInBytes = %u\n", fileSizeInBytes);
#endif

  bmpFileAsBytes = (unsigned char *)malloc(fileSizeInBytes);
  if (bmpFileAsBytes == NULL) {
    exit(MALLOC_ERROR);
  }
  getBmpFileAsBytes(bmpFileAsBytes, fileSizeInBytes, stream);

  parseHeaderAndApplyFilter(bmpFileAsBytes, grayscale, stream);

#ifndef DEBUG
  if (fwrite(bmpFileAsBytes, fileSizeInBytes, 1, stdout) != 1) {
    exit(FWRITE_ERROR);
  }
#endif

  free(bmpFileAsBytes);
  return 0;
}