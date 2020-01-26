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

void setPixelArray(unsigned char* pixelArray, FILE* stream, int offsetFirstBytePixelArray, int numElements, int paddingPixels, int width) {
  unsigned int pixelArrayStartIndex = offsetFirstBytePixelArray + 1;

  printf("%s %i", "here is the size of the pixel array : ", numElements);
  printf("\n");
  printf("%s %i", "here is the pixelArrayStartIndex :", pixelArrayStartIndex);
  printf("\n");

  // JACOB, now you need to work on inserting a pading pixel after every so many pixels are added per row.
  int numElementsInsertedOnRow = 0;
  int charsAllowedPerRow = (width - paddingPixels);
  for (int i = 0; i < numElements; i++) {
    fread(&pixelArray[i], 1, 1, stream);
    numElementsInsertedOnRow++;
    if (numElementsInsertedOnRow == charsAllowedPerRow) {
      // We are done for the row. Insert your padding pixels
      for (int j = 0; j < paddingPixels; j++) {
        i++;
        pixelArray[i] = 0;
      }
      numElementsInsertedOnRow = 0;
    }
  }

  for (int i = 0; i < numElements; i++) {
    printf("%s %i %s %02x", "Index ", i, ":", pixelArray[i]);
    printf("\n");  
  }
} 

int calculatePaddingPixels(int width) {
  int paddingPixelsNeeded = 0;
  int remainder = width / 3; // 3 Beacause a pixel is 3 bytes
  paddingPixelsNeeded = remainder % 4;
  return paddingPixelsNeeded;
}


void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale, FILE* stream) {

  fseek(stream, 0, SEEK_SET ); // stream is the file we are in. 0 sets it to where we want to read from SEEK_SET. SEEK_SET
                               // is the beginning of the file? Idk. I just know that 0 says where in the file we want to 
                               // start reading from.  

  // bmp Header variables
  unsigned char headerField[2];
  int fileSize;
  int dummyStorage;
  int offsetFirstBytePixelArray = 0;

  // --------------------- bmp Header--------------------------//
  fread(headerField, 2, 1, stream);
  fread(&fileSize, 4, 1, stream);
  fread(&dummyStorage, 4, 1, stream);
  fread(&offsetFirstBytePixelArray, 4, 1, stream);



  // DIB Header variables
  int dibHeaderSize;
  int width = 0; 
  int height = 0;
  int numColorPlanes;
  int numBitsPerPixel;
  int copmressionMethod;
  int imageSizeInBytes;
  int horizRes;
  int vertRes;
  int numColorsInPallete;
  int numImportantColors;

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
  unsigned int numElements = width * height;
  unsigned char pixelArray[numElements + 1];


  // Calculate pixel padding
  unsigned int paddingPixels = calculatePaddingPixels(width);
  
  
  printf("%s %i", "Padding pixels: ", paddingPixels);
  printf("\n");
  // printf("%s %u", "Here is the pixelOfset: ", offsetFirstBytePixelArray);
  // printf("\n");
  // printf("%s %u", "Here is the width: ", width);
  // printf("\n");
  // printf("%s %u", "Here is the height: ", height);
  // printf("\n");
  // printf("%s %lu", "Here is the size of the pixelArray: ", sizeof(pixelArray)/sizeof(pixelArray[0]));
  // printf("\n");



 setPixelArray(pixelArray, stream, offsetFirstBytePixelArray, numElements, paddingPixels, width);




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