#pragma once

#include "Types.h"

class BmpManager;

/*-------------
	BmpImage
--------------*/

class BmpImage
{
	friend class BmpManager;
public:
	BmpImage() = default;
	~BmpImage() = default;

	vector<vector<uint8>>&		GetPixelValue()				  { return _pixel; }
	int32						GetHeight()				const { return _height; }
	int32						GetWidth()				const { return _width; }
	int32						GetBytesPerPixel()		const { return _bytesPerPixel; }

private:
	vector<vector<uint8>>	_pixel				= {};
	int32					_height				= 0;
	int32					_width				= 0;
	// RGB : 3, GRAY : 1
	int32					_bytesPerPixel		= 0;
};