
#pragma once

#include <cmath>
#include <vector>

#include "Types.h"

#define PI 3.1415926535

using namespace std;

class Dct
{
public:
	Dct() = default;
	~Dct() = default;

private:
	static double GetConstC(int32 idx)		{ return idx == 0 ? 1 / sqrt(2) : 1; }
	static const int32 BLOCK_SIZE = 8;

public:
	vector<vector<int32>> DiscreteCosineTransform(vector<vector<int32>>& macroBlock);
	vector<vector<int32>> InverseDiscreteCosineTransform(vector<vector<int32>>& macroBlock);

private:
	int32 CalcFrequencyCoefficient(int32 u, int32 v, vector<vector<int32>>& macroBlock);
	int32 CalcSpatialCoefficient(int32 i, int32 j, vector<vector<int32>>& macroBlock);
};

