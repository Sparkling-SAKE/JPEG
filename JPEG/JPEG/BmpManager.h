#pragma once
#include "Types.h"

#define DATA_OFFSET_OFFSET		0x000A
#define WIDTH_OFFSET			0x0012
#define HEIGHT_OFFSET			0x0016
#define BITS_PER_PIXEL_OFFSET	0x001C
#define HEADER_SIZE				14
#define INFO_HEADER_SIZE		40
#define NO_COMPRESION			0
#define MAX_NUMBER_OF_COLORS	0
#define ALL_COLORS_REQUIRED		0

class BmpImage;

/*---------------
	BmpManager
----------------*/

class BmpManager
{
public:
	BmpManager() = default;
	~BmpManager();

public:
	static void				ReadImage(string fileName, OUT BmpImage& image);
	static void				WriteImage(string fileName, BmpImage& image);
	static void				Clear();

private:
	static void				WriteHeader();
	static void				GetHeader();

private:
	static string			_header;

	static ifstream			_fin;
	static ofstream			_fout;
};
