
#pragma once

#include "Types.h"

#include "DCT.h"
#include "Huffman.h"

class BmpImage;

using namespace std;

class Jpeg
{
private:
	static vector<vector<int32>>		LumaQuantizationTable;
	static vector<int32>				ZigzagIndexX;
	static vector<int32>				ZigzagIndexY;
	static const int32					BLOCK_SIZE = 8;
	
	static string					ConvertDec2Bin(int32 dec);
	static int32					ConvertBin2Dec(string bin);

public:
	Jpeg() = delete;
	Jpeg(vector<vector<uint8>>& pixel, int32 width, int32 height);
	Jpeg(BmpImage& image);
	~Jpeg() = default;

public:
	void					Compress();
	void					Decompress();

public:
	vector<vector<uint8>>&	GetReconstructPixel() { return _decompressPixel; }

private:
	void					Quantization(vector<vector<int32>>& macroBlock);
	void					InverseQuantization(vector<vector<int32>>& macroBlock);
	void					ZigZagScanning(vector<vector<int32>>& macroBlock);
	vector<vector<int32>>	InverseZigZagScanning(int32 dc, vector<pair<int32, int32>> ac);
	void					Dpcm();
	void					DcHuffman();
	void					AcHuffman();

	vector<vector<int32>>	SliceImage(int32 heightZeroIdx, int32 widthZeroIdx);
	void					ReconstructImage(vector<vector<int32>>& macroBlock, int32 heightZeroIdx, int32 widthZeroIdx);

	void					ConvertByte2Bit();
	void					ConvertBit2Byte();

private:
	Dct								_dct;
	Huffman							_acHuffman;
	Huffman							_dcHuffman;
	vector<int32>					_dcVector;
	vector<pair<int32, int32>>		_acRunLengthPair;		// pair <ZeroRun, value>
	vector<pair<int32, string>>		_dcRunLengthPair;		// pair <length,  value>

	ofstream						_fout;
	ifstream						_fin;
	
	vector<vector<uint8>>&			_pixel;
	vector<vector<uint8>>			_decompressPixel;
	int32							_imageWidth = 0;
	int32							_imageHeight = 0;
};