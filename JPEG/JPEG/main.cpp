
#include "JPEG.h"
#include "BmpManager.h"
#include "BmpImage.h"
#include "Types.h"

int main()
{
	BmpImage image;
	BmpManager::ReadImage("Lena.bmp", OUT &image);

	Jpeg jpeg(&image);

	jpeg.Compress();
	jpeg.Decompress();

	BmpManager::WriteImage("decompressed.bmp", OUT &image);

	return 0;
}
