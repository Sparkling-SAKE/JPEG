
#include "JPEG.h"
#include <queue>
#include "BmpImage.h"

// JPEG Quantization Table 초기화
vector<vector<int32>> Jpeg::LumaQuantizationTable = {
   {16,11,10,16,24,40,51,61},
   {12,12,14,19,26,58,60,55},
   {14,13,16,24,40,57,69,56},
   {14,17,22,29,51,87,80,62},
   {18,22,37,56,68,109,103,77},
   {24,35,55,64,81,104,113,92},
   {49,64,78,87,103,121,120,101},
   {72,92,95,98,112,100,103,99}
};

// Zigzag Scanning Index 64개 고정이므로 미리 초기화
vector<int32> Jpeg::ZigzagIndexX = { 0, 0, 1, 2, 1, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 4, 5, 6, 7, 7, 6, 5, 6, 7, 7 };
vector<int32> Jpeg::ZigzagIndexY = { 0, 1, 0, 0, 1, 2, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 5, 6, 7, 7, 6, 7 };

string Jpeg::ConvertDec2Bin(int32 dec)
{
	string bin = "";
	if (dec >= 0) 
	{
		while (dec) 
		{
			if (dec % 2 == 0)		
				bin.push_back('0');
			else					
				bin.push_back('1');

			dec /= 2; 
		}
	}
	else 
	{
		while (dec) 
		{
			if (dec % 2 == 0)		
				bin.push_back('1');
			else					
				bin.push_back('0');

			dec /= 2; 
		}
	}
	for (int32 i = 0; i < bin.length() / 2; ++i)
	{
		char temp = bin[i];
		bin[i] = bin[bin.length() - 1 - i];
		bin[bin.length() - 1 - i] = temp; 
	}

	return bin;
}

int32 Jpeg::ConvertBin2Dec(string bin)
{
	int32 dec = 0;
	bool isNegative = false;
	if (bin[0] == '0')
	{
		isNegative = true;
		for (int32 i = 0; i < bin.length(); ++i)
		{
			if (bin[i] == '0')
				bin[i] = '1';
			else			
				bin[i] = '0'; 
		}
	}
	int32 power = 1;
	for (int32 i = bin.length() - 1; i >= 0; --i)
	{
		if (bin[i] == '1')		
			dec += power;
		power *= 2;
	}

	if (isNegative)		return dec * -1;
	else				return dec;
}

Jpeg::Jpeg(vector<vector<uint8>>& pixel, int32 width, int32 height) : _pixel(pixel)
{
	_imageWidth = width;
	_imageHeight = height;
	_decompressPixel.resize(height);
	for (int32 i = 0; i < height; ++i)
		_decompressPixel[i].resize(width);
}

Jpeg::Jpeg(BmpImage& image)	: _pixel(image.GetPixelValue()), _imageWidth(image.GetWidth()), _imageHeight(image.GetHeight())
{
	_decompressPixel.resize(_imageHeight);
	for (int32 i = 0; i < _imageHeight; ++i)
		_decompressPixel[i].resize(_imageWidth);
}

void Jpeg::Compress()
{
	vector<vector<int32>> slice;
	vector<vector<int32>> macroBlock;
	for (int32 i = 0; i < _imageHeight; i += BLOCK_SIZE)
	{
		for (int32 j = 0; j < _imageWidth; j += BLOCK_SIZE)
		{
			slice = SliceImage(i, j);
			macroBlock = std::move(_dct.DiscreteCosineTransform(slice));
			Quantization(macroBlock);
			ZigZagScanning(macroBlock);
		}
	}
	Dpcm();
	AcHuffman();
	DcHuffman();

	unordered_map<int32, string>							dcCode = _dcHuffman.getDcSymbol();
	unordered_map<pair<int32, int32>, string, pair_hash>	acCode = _acHuffman.getAcSymbol();
	string blockEndSign = acCode[make_pair(0, 0)];

	_fout.open("Lena.jhj", ios_base::binary);
	int32 macroBlockNumber = (_imageWidth / BLOCK_SIZE) * (_imageHeight / BLOCK_SIZE);
	int32 acIndex = 0;
	for (int32 i = 0; i < macroBlockNumber; ++i)
	{
		_fout << dcCode[_dcRunLengthPair[i].first] << _dcRunLengthPair[i].second;
		while (true)
		{
			string code = acCode[_acRunLengthPair[acIndex++]];
			if (code == blockEndSign)
			{
				_fout << code;
				break;
			}
			_fout << code;
		}
	}
	_fout.close();

	ConvertByte2Bit();
}

void Jpeg::Decompress()
{
	unordered_map<string, int32>							dcCode = _dcHuffman.getDcLength();
	unordered_map<string, pair<int32, int32>>				acCode = _acHuffman.getAcCode();

	ConvertBit2Byte();

	_fin.open("Lena.jhj", ios_base::binary);
	char c;
	int32 currentDc = 0, lastDc = 0;
	string code = "";
	vector<pair<int32, int32>> acVec;
	for (int32 i = 0; i < _imageHeight; i += BLOCK_SIZE)
	{
		for (int32 j = 0; j < _imageWidth; j += BLOCK_SIZE)
		{
			vector<vector<int32>> macroBlock(BLOCK_SIZE, vector<int32>(BLOCK_SIZE, 0));
			code = "";
			while (_fin.get(c))
			{
				code.push_back(c);
				if (dcCode.find(code) != dcCode.end())	break;
			}
			string dcBin = "";
			for (int32 i = 0; i < dcCode[code]; ++i)
			{
				_fin.get(c);
				dcBin.push_back(c);
			}
			currentDc = Jpeg::ConvertBin2Dec(dcBin) + lastDc;
			lastDc = currentDc;
			code = "";
			acVec.clear();
			while (_fin.get(c))
			{
				code.push_back(c);
				if (acCode.find(code) != acCode.end())
				{
					acVec.push_back(acCode[code]);
					if (acCode[code].first == 0 && acCode[code].second == 0)	break;
					code = "";
				}
			}
			macroBlock = std::move(InverseZigZagScanning(currentDc, acVec));
			InverseQuantization(macroBlock);
			macroBlock = std::move(_dct.InverseDiscreteCosineTransform(macroBlock));
			ReconstructImage(macroBlock, i, j);
		}
	}

	_fin.close();
	ConvertByte2Bit();

	_pixel.swap(_decompressPixel);
}

void Jpeg::Quantization(vector<vector<int32>>& macroBlock)
{
	for (int32 i = 0; i < BLOCK_SIZE; ++i)
		for (int32 j = 0; j < BLOCK_SIZE; ++j)
			macroBlock[i][j] = static_cast<int32>(static_cast<double>(macroBlock[i][j]) / LumaQuantizationTable[i][j]);
}

void Jpeg::InverseQuantization(vector<vector<int32>>& macroBlock)
{
	for (int32 i = 0; i < BLOCK_SIZE; ++i)
		for (int32 j = 0; j < BLOCK_SIZE; ++j)
			macroBlock[i][j] = macroBlock[i][j] * LumaQuantizationTable[i][j];
}

void Jpeg::ZigZagScanning(vector<vector<int32>>& macroBlock)
{
	int32 lastNonZeroIdx = 0;
	for (int32 i = 1; i < BLOCK_SIZE * BLOCK_SIZE; ++i)
		if (macroBlock[ZigzagIndexX[i]][ZigzagIndexY[i]] != 0)
			lastNonZeroIdx = i;

	int32 zeroRun = 0;
	_dcVector.push_back(macroBlock[0][0]);
	for (int32 i = 1; i < BLOCK_SIZE * BLOCK_SIZE; ++i)
	{
		if (lastNonZeroIdx == 0)
		{
			_acRunLengthPair.push_back(std::move(std::make_pair(0, 0)));
			break;
		}
		if (i == lastNonZeroIdx)
		{
			_acRunLengthPair.push_back(std::move(std::make_pair(zeroRun, macroBlock[ZigzagIndexX[i]][ZigzagIndexY[i]])));
			_acRunLengthPair.push_back(std::move(std::make_pair(0, 0)));
			break;
		}
		if (macroBlock[ZigzagIndexX[i]][ZigzagIndexY[i]] == 0 && zeroRun <= 15)
		{
			zeroRun++;
			continue;
		}
		_acRunLengthPair.push_back(std::move(std::make_pair(zeroRun, macroBlock[ZigzagIndexX[i]][ZigzagIndexY[i]])));
		zeroRun = 0;
	}
}

vector<vector<int32>> Jpeg::InverseZigZagScanning(int32 dc, vector<pair<int32, int32>> ac)
{
	vector<vector<int32>> macroBlock(BLOCK_SIZE, vector<int32>(BLOCK_SIZE, 0));
	macroBlock[0][0] = dc;
	int32 acIndex = 0;
	for (int32 i = 1; i < BLOCK_SIZE * BLOCK_SIZE; ++i)
	{
		int32 zeroRun = ac[acIndex].first;
		int32 value = ac[acIndex].second;
		if (zeroRun == 0 && value == 0)
			break;
		i += zeroRun;
		macroBlock[ZigzagIndexX[i]][ZigzagIndexY[i]] = value;
		acIndex++;
	}

	return macroBlock;
}

vector<vector<int32>> Jpeg::SliceImage(int32 heightZeroIdx, int32 widthZeroIdx)
{
	vector<vector<int32>> macroBlock(BLOCK_SIZE, vector<int32>(BLOCK_SIZE, 0));
	for (int32 i = 0; i < BLOCK_SIZE; ++i)
		for (int32 j = 0; j < BLOCK_SIZE; ++j)
			macroBlock[i][j] = _pixel[heightZeroIdx + i][widthZeroIdx + j] - 128;

	return macroBlock;
}

void Jpeg::ReconstructImage(vector<vector<int32>>& macroBlock, int32 heightZeroIdx, int32 widthZeroIdx)
{
	for (int32 i = 0; i < BLOCK_SIZE; ++i)
	{
		for (int32 j = 0; j < BLOCK_SIZE; ++j)
		{
			if (macroBlock[i][j] + 128 >= 0)
				_decompressPixel[heightZeroIdx + i][widthZeroIdx + j] = macroBlock[i][j] + 128;
		}
	}
}

void Jpeg::ConvertByte2Bit()
{
	_fin.open("Lena.jhj", ios_base::binary);
	char c;
	vector<char> byteChar;
	int32 byteNumber;
	int32 count = 0;
	string bitStream = "";
	while (_fin.get(c))
	{
		bitStream.push_back(c);
		count++;
		if (count >= 7)
		{
			byteChar.push_back(ConvertBin2Dec(bitStream));
			//_fout << byteChar;
			bitStream = "";
			count = 0;
		}
	}
	_fin.close();

	_fout.open("Lena.jhj", ios_base::binary);
	for (int32 i = 0; i < byteChar.size(); ++i)
		_fout << byteChar[i];

	if (count != 0)
		_fout << '\n' << bitStream;

	_fout.close();
}

void Jpeg::ConvertBit2Byte()
{
	_fin.open("Lena.jhj", ios_base::binary);
	char c, bitNumber;
	int32 count = 0;
	vector<string> bitStream;
	while (_fin.get(c))
	{
		if (c == '\n')
			break;
		bitStream.push_back(ConvertDec2Bin(static_cast<int32>(c)));
		//_fout << bitStream;
	}

	string lastBit = "";
	while (_fin.get(c))
	{
		lastBit.push_back(c);
	}
	_fin.close();

	_fout.open("Lena.jhj", ios_base::binary);
	for (int32 i = 0; i < bitStream.size(); ++i)
		_fout << bitStream[i];

	_fout << lastBit;
	_fout.close();
}

void Jpeg::Dpcm()
{
	vector<int32> differential(_dcVector.size(), 0);
	differential[0] = _dcVector[0];
	for (int32 i = 1; i < _dcVector.size(); ++i)
		differential[i] = _dcVector[i] - _dcVector[i - 1];

	// differential 2진수로 바꾸고 pair<length, value> 식으로 바꾸기
	for (int32 i = 0; i < differential.size(); ++i)
	{
		string bin = Jpeg::ConvertDec2Bin(differential[i]);
		_dcRunLengthPair.push_back(std::move(make_pair(bin.length(), bin)));
	}
	_dcVector.clear();
	_dcVector = std::move(differential);
}

void Jpeg::AcHuffman()
{
	unordered_map<pair<int32, int32>, int32, pair_hash> acFrequency;		// <AC value , frequency>
	for (int32 i = 0; i < _acRunLengthPair.size(); ++i)
		acFrequency[_acRunLengthPair[i]]++;

	_acHuffman.HuffmanCoding(acFrequency);
}

void Jpeg::DcHuffman()
{
	unordered_map<int32, int32> dcFrequency;				// <DC value , frequency>
	for (int32 i = 0; i < _dcRunLengthPair.size(); ++i)
		dcFrequency[_dcRunLengthPair[i].first]++;

	_dcHuffman.HuffmanCoding(dcFrequency);
}
