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

private:
	vector<vector<byte>>	_pixel = {};
	int32					_height = 0;
	int32					_width = 0;
	// RGB : 3, GRAY : 1
	int32					_bytesPerPixel = 0;
};