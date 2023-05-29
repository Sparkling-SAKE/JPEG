
#include "BmpManager.h"
#include "BmpImage.h"

string      _header = "";
ifstream    _fin = {};
ofstream    _fout = {};

BmpManager::~BmpManager()
{
    if (_fin.is_open())
        _fin.close();

    if (_fout.is_open())
        _fout.close();
}

void BmpManager::ReadImage(string fileName, OUT BmpImage* image)
{
    Clear();

    _fin.open(fileName, std::ios::binary);
    if (_fin.is_open() == false)
        return;

    int32 lookUpTable = 0;

    _fin.seekg(HEADER_SIZE + INFO_HEADER_SIZE - 8, SEEK_SET);
    _fin.read(reinterpret_cast<char*>(&lookUpTable), sizeof(lookUpTable));
    _fin.seekg(0, SEEK_SET);

    int header = 0;
    if (lookUpTable != 0)
        header = HEADER_SIZE + INFO_HEADER_SIZE + 1024;
    else
        header = HEADER_SIZE + INFO_HEADER_SIZE;

    // 원본 BMP 파일에서 헤더와 테이블 뽑아서 _header에 저장
    for (int i = 0; i < header; i++) 
        _header += _fin.get();

    int32 dataOffset; //데이터 시작 위치 주소값 
    _fin.seekg(DATA_OFFSET_OFFSET, SEEK_SET);
    _fin.read(reinterpret_cast<char*>(&dataOffset), sizeof(dataOffset));

    _fin.seekg(WIDTH_OFFSET, SEEK_SET);
    _fin.read(reinterpret_cast<char*>(&image->_width), sizeof(image->_width));

    _fin.seekg(HEIGHT_OFFSET, SEEK_SET);
    _fin.read(reinterpret_cast<char*>(&image->_height), sizeof(image->_height));

    int16 bitsPerPixel;
    _fin.seekg(BITS_PER_PIXEL_OFFSET, SEEK_SET);
    _fin.read(reinterpret_cast<char*>(&bitsPerPixel), sizeof(bitsPerPixel));

    image->_bytesPerPixel = static_cast<int32>(bitsPerPixel / 8);

    int paddedRowSize = static_cast<int>((image->_width + 3) / 4) * 4 * image->_bytesPerPixel;
    int unpaddedRowSize = image->_width * image->_bytesPerPixel;
    int totalSize = unpaddedRowSize * image->_height;

    vector<vector<byte>>& pixel = image->_pixel;
    pixel.resize(image->_width);
    for (vector<byte>& row : pixel)
        row.resize(image->_height);

    int size = pixel.size() - 1;
    for (int i = 0; i < image->_height; ++i)
    {
        _fin.seekg(dataOffset + (i * paddedRowSize), SEEK_SET);
        _fin.read(reinterpret_cast<char*>(&pixel[size - i][0]), unpaddedRowSize);
    }

    return;
}

void BmpManager::WriteImage(string fileName, BmpImage* image)
{
    _fout.open(fileName, std::ios::binary);

    if (_header.empty() == true)
        GetHeader();
    
    WriteHeader();

    int paddedRowSize = static_cast<int>((image->_width + 3) / 4) * 4 * image->_bytesPerPixel;

    int size = image->_pixel.size() - 1;
    for (int i = 0; i < image->_height; ++i)
        _fout.write(reinterpret_cast<const char*>(&image->_pixel[size - i][0]), paddedRowSize);
}

void BmpManager::Clear()
{
    if (_fin.is_open())
        _fin.close();

    if (_fout.is_open())
        _fout.close();
}

void BmpManager::WriteHeader()
{
    for (int i = 0; i < _header.length(); ++i)
        _fout.put(_header[i]);
}

void BmpManager::GetHeader()
{
    _fin.open("HEADER", std::ios::binary);
    while (_fin)
        _header += _fin.get();
}
