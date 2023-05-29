
#pragma once

#include "Types.h"

#include "DCT.h"
#include "Huffman.h"

class BmpImage;

using namespace std;

class Jpeg
{
private:
	static vector<vector<int>>		LumaQuantizationTable;
	static vector<int>				ZigzagIndexX;
	static vector<int>				ZigzagIndexY;
	static const int				BLOCK_SIZE = 8;
	
	static string					ConvertDec2Bin(int dec);
	static int						ConvertBin2Dec(string bin);

public:
	Jpeg() = delete;
	Jpeg(vector<vector<int>>& pixel, int width, int height);
	Jpeg(BmpImage* image) {}
	~Jpeg() = default;

public:
	void					Compress();
	void					Decompress();

public:
	vector<vector<int>>&	GetReconstructPixel() { return _decompressPixel; }

private:
	void					Quantization(vector<vector<int>>& macroBlock);
	void					InverseQuantization(vector<vector<int>>& macroBlock);
	void					ZigZagScanning(vector<vector<int>>& macroBlock);
	vector<vector<int>>		InverseZigZagScanning(int dc, vector<pair<int, int>> ac);
	void					Dpcm();
	void					DcHuffman();
	void					AcHuffman();

	vector<vector<int>>		SliceImage(int heightZeroIdx, int widthZeroIdx);
	void					ReconstructImage(vector<vector<int>>& macroBlock, int heightZeroIdx, int widthZeroIdx);

	void					ConvertByte2Bit();
	void					ConvertBit2Byte();

private:
	Dct							_dct;
	Huffman						_acHuffman;
	Huffman						_dcHuffman;
	vector<int>					_dcVector;
	vector<pair<int, int>>		_acRunLengthPair;		// pair <ZeroRun, value>
	vector<pair<int, string>>   _dcRunLengthPair;		// pair <length,  value>

	ofstream					_fout;
	ifstream					_fin;
	
	vector<vector<int>>			_pixel;
	vector<vector<int>>			_decompressPixel;
	int							_imageWidth = 0;
	int							_imageHeight = 0;
};