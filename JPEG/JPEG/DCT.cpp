
#include "DCT.h"

vector<vector<int32>> Dct::DiscreteCosineTransform(vector<vector<int32>>& macroBlock)
{
	vector<vector<int32>> dctCoefficient(BLOCK_SIZE, vector<int32>(BLOCK_SIZE, 0));
	for (int32 u = 0; u < BLOCK_SIZE; ++u)
		for (int32 v = 0; v < BLOCK_SIZE; ++v)
			dctCoefficient[u][v] = CalcFrequencyCoefficient(u, v, macroBlock);

	return dctCoefficient;
}

vector<vector<int32>> Dct::InverseDiscreteCosineTransform(vector<vector<int32>>& macroBlock)
{
	vector<vector<int32>> idctCoefficient(BLOCK_SIZE, vector<int32>(BLOCK_SIZE, 0));
	for (int32 i = 0; i < BLOCK_SIZE; ++i)
		for (int32 j = 0; j < BLOCK_SIZE; ++j)
			idctCoefficient[i][j] = CalcSpatialCoefficient(i, j, macroBlock);

	return idctCoefficient;
}

int32 Dct::CalcFrequencyCoefficient(int32 u, int32 v, vector<vector<int32>>& macroBlock)
{
	double coefficient = Dct::GetConstC(u) * Dct::GetConstC(v) / 4;
	double sum = 0.0;
	for (int32 i = 0; i < BLOCK_SIZE; ++i)
		for (int32 j = 0; j < BLOCK_SIZE; ++j)
			sum += cos((2 * i + 1) * u * PI / 16) * cos((2 * j + 1) * v * PI / 16) * macroBlock[i][j];
	
	if (sum >= 0)
		return static_cast<int32>(coefficient * sum + 0.5);
	else
		return static_cast<int32>(coefficient * sum - 0.5);
}

int32 Dct::CalcSpatialCoefficient(int32 i, int32 j, vector<vector<int32>>& macroBlock)
{
	double sum = 0.0;
	for (int32 u = 0; u < BLOCK_SIZE; ++u)
		for (int32 v = 0; v < BLOCK_SIZE; ++v)
			sum += Dct::GetConstC(u) * Dct::GetConstC(v) / 4 *
				cos((2 * i + 1) * u * PI / 16) * cos((2 * j + 1) * v * PI / 16) * macroBlock[u][v];

	if (sum >= 0)
		return static_cast<int32>(sum + 0.5);
	else
		return static_cast<int32>(sum - 0.5);
}
