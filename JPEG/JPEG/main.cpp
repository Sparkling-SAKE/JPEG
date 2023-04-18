
#include "JPEG.h"
#include "BMPReader.h"

int main()
{
	byte* pixels;
	int32 width;
	int32 height;
	int32 bytesPerPixel;
	FILE* imageFile; //파일 포인터
	FILE* outputImageFile;
	ReadBmpImage("Lena.bmp", "Lena_compressed.bmp", &pixels, width, height, bytesPerPixel, imageFile, outputImageFile);

	vector<vector<int>> pixel(width, vector<int>(height, 0));
	int cnt = 0;
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
			pixel[i][j] = pixels[cnt++];

	Jpeg jpeg(pixel, width, height);

	jpeg.Compress();
	jpeg.Decompress();

	WriteJpegImage(jpeg.GetReconstructPixel(), width, height, bytesPerPixel, outputImageFile);

	return 0;
}