
#pragma once

#include <stdio.h> // FILE*, fseek, fread 등 사용

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

typedef unsigned int    int32;
typedef short           int16;
typedef unsigned char   byte;

void ReadBmpImage(const char* fileName, const char* outfileName, byte** pixels, int32& width, int32& height, int32& bytesPerPixel, FILE*& imageFile, FILE*& OUT) // (파일이름, 픽셀값 담을 배열, 가로길이 값의 시작 위치, 세로길이 값의 시작 위치, 픽셀당 바이트 수 시작 위치
{

    if (fopen_s(&imageFile, fileName, "rb"))//파일을 바이너리 모드로 열기
        return;
    int32 dataOffset; //데이터 시작 위치 주소값 
    int32 LookUpTable = 0;
    fseek(imageFile, HEADER_SIZE + INFO_HEADER_SIZE - 8, SEEK_SET); //fseek(파일변수,이동byte,기준위치)
    fread(&LookUpTable, 4, 1, imageFile); //fread(메모리주소,크기,갯수,파일변수)
    fseek(imageFile, 0, SEEK_SET);

    if (fopen_s(&OUT, outfileName, "wb"))
        return;

    int header = 0;
    if (LookUpTable)
        header = HEADER_SIZE + INFO_HEADER_SIZE + 1024;
    else
        header = HEADER_SIZE + INFO_HEADER_SIZE;
    for (int i = 0; i < header; i++) // 원본 BMP 파일에서 헤더와 테이블 뽑아서 새로운 BMP 파일의 헤더에 써줌
    {
        int get = getc(imageFile);
        putc(get, OUT);
    }

    fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET); //fseek(파일변수,이동byte,기준위치)
    fread(&dataOffset, 4, 1, imageFile); //fread(메모리주소,크기,갯수,파일변수)
    fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
    fread(&width, 4, 1, imageFile);
    fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
    fread(&height, 4, 1, imageFile);
    int16 bitsPerPixel;
    fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
    fread(&bitsPerPixel, 2, 1, imageFile);
    bytesPerPixel = ((int32)bitsPerPixel) / 8; //3 bytes per pixel when color, 1 byte per pixel when grayscale


    int paddedRowSize = static_cast<int>((width + 3) / 4) * 4 * bytesPerPixel;
    int unpaddedRowSize = width * bytesPerPixel;
    int totalSize = unpaddedRowSize * height;

    *pixels = new byte[totalSize];
    int i = 0;
    byte* currentRowPointer = *pixels + ((height - 1) * unpaddedRowSize);
    for (i = 0; i < height; i++)
    {
        fseek(imageFile, dataOffset + (i * paddedRowSize), SEEK_SET);
        fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
        currentRowPointer -= unpaddedRowSize;
    }

    fclose(imageFile);
}

void WriteJpegImage(vector<vector<int>> pixel, int32 width, int32 height, int32 bytesPerPixel, FILE*& outputFile)
{
    byte* pixels = new byte[width * height * bytesPerPixel];
    int idx = 0;
    for (int i = 0; i < width; ++i)
        for (int j = 0; j < height; ++j)
            pixels[idx++] = pixel[i][j];
    int paddedRowSize = (int)(4 * (float)width / 4.0f) * bytesPerPixel;
    int unpaddedRowSize = width * bytesPerPixel;
    for (int i = 0; i < height; i++)
    {
        int pixelOffset = ((height - i) - 1) * unpaddedRowSize;
        fwrite(&pixels[pixelOffset], 1, paddedRowSize, outputFile);
    }
    fclose(outputFile);
}
