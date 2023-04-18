
#pragma once

#include <cmath>
#include <vector>

#define PI 3.1415926535

using namespace std;

class Dct
{
public:
	Dct() = default;
	~Dct() = default;

private:
	static double GetConstC(int idx)		{ return idx == 0 ? 1 / sqrt(2) : 1; }
	static const int BLOCK_SIZE = 8;

public:
	vector<vector<int>> DiscreteCosineTransform(vector<vector<int>>& macroBlock);
	vector<vector<int>> InverseDiscreteCosineTransform(vector<vector<int>>& macroBlock);

private:
	int CalcFrequencyCoefficient(int u, int v, vector<vector<int>>& macroBlock);
	int CalcSpatialCoefficient(int i, int j, vector<vector<int>>& macroBlock);
};

