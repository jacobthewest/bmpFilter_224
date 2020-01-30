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
  return ((blue + green + red) / 3);
}

void applyGrayscaleToPixel(unsigned char* pixel) {
  unsigned char blue = *pixel;
  unsigned char green = *(pixel + 1);
  unsigned char red = *(pixel + 2);
  unsigned char averageIntensity = getAverageIntensity(blue, green, red);
  
  *pixel = averageIntensity;
  *(pixel + 1) = averageIntensity;
  *(pixel + 2) = averageIntensity;
}

void applyThresholdToPixel(unsigned char* pixel) {
  unsigned char blue = *pixel;
  unsigned char green = *(pixel + 1);
  unsigned char red = *(pixel + 2);
  unsigned char averageIntensity = getAverageIntensity(blue, green, red);
  
  if(averageIntensity < 128) { 
    // All black
    *pixel = 0x00;
    *(pixel + 1) = 0x00;
    *(pixel + 2) = 0x00; 
  } else {
    // All white
    *pixel = 0xff;
    *(pixel + 1) = 0xff;
    *(pixel + 2) = 0xff;
  }
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
  if(isGrayscale) {
    applyGrayscaleToPixel(pixel);
  } else {
    applyThresholdToPixel(pixel);
  }
}

int calculatePaddingPixels(int bytesFromGivenPixels) {
  int paddingPixelsNeeded = 0;
  if((bytesFromGivenPixels % 4) != 0)  {
    paddingPixelsNeeded = 4 - (bytesFromGivenPixels % 4);
  }  
  return paddingPixelsNeeded;
}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
  for(int i = 0; i < width; i++) {
    applyFilterToPixel((row + (i * 3)), isGrayscale);
  }
}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
  int bytesFromGivenPixels = width * 3;
  int paddingPixelsPerRow = calculatePaddingPixels(bytesFromGivenPixels);
  int bytesInRow = bytesFromGivenPixels + paddingPixelsPerRow;

  for(int i = 0; i < height; i++) {
    applyFilterToRow((pixelArray + (bytesInRow * i)), width, isGrayscale);
  }
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


  offsetFirstBytePixelArray = *(int*)(bmpFileAsBytes + 10);
  width = *(int*)(bmpFileAsBytes + 18);
  height = *(int*)(bmpFileAsBytes + 22);
  pixelArray = bmpFileAsBytes + offsetFirstBytePixelArray;

#ifdef DEBUG
  printf("offsetFirstBytePixelArray = %u\n", offsetFirstBytePixelArray);
  printf("width = %u\n", width);
  printf("height = %u\n", height);
  printf("pixelArray = %p\n", pixelArray);
#endif
  
  unsigned char pixelArrayCopy[numElements + 1];
  copyPixelArray(pixelArray, pixelArrayCopy, numElements);
  for(int i = 0; i < (numElements + 1); i++) {
    printf("Byte at %d: %d", i, pixelArray[i]);
    printf("\n");
  }
  applyFilterToPixelArray(pixelArrayCopy, imageWidthInBytes, height, isGrayscale);
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