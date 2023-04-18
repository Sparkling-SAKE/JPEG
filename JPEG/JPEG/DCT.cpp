
#include "DCT.h"

vector<vector<int>> Dct::DiscreteCosineTransform(vector<vector<int>>& macroBlock)
{
	vector<vector<int>> dctCoefficient(BLOCK_SIZE, vector<int>(BLOCK_SIZE, 0));
	for (int u = 0; u < BLOCK_SIZE; ++u)
		for (int v = 0; v < BLOCK_SIZE; ++v)
			dctCoefficient[u][v] = CalcFrequencyCoefficient(u, v, macroBlock);

	return dctCoefficient;
}

vector<vector<int>> Dct::InverseDiscreteCosineTransform(vector<vector<int>>& macroBlock)
{
	vector<vector<int>> idctCoefficient(BLOCK_SIZE, vector<int>(BLOCK_SIZE, 0));
	for (int i = 0; i < BLOCK_SIZE; ++i)
		for (int j = 0; j < BLOCK_SIZE; ++j)
			idctCoefficient[i][j] = CalcSpatialCoefficient(i, j, macroBlock);

	return idctCoefficient;
}

int Dct::CalcFrequencyCoefficient(int u, int v, vector<vector<int>>& macroBlock)
{
	double coefficient = Dct::GetConstC(u) * Dct::GetConstC(v) / 4;
	double sum = 0.0;
	for (int i = 0; i < BLOCK_SIZE; ++i)
		for (int j = 0; j < BLOCK_SIZE; ++j)
			sum += cos((2 * i + 1) * u * PI / 16) * cos((2 * j + 1) * v * PI / 16) * macroBlock[i][j];
	
	if (sum >= 0)
		return static_cast<int>(coefficient * sum + 0.5);
	else
		return static_cast<int>(coefficient * sum - 0.5);
}

int Dct::CalcSpatialCoefficient(int i, int j, vector<vector<int>>& macroBlock)
{
	double sum = 0.0;
	for (int u = 0; u < BLOCK_SIZE; ++u)
		for (int v = 0; v < BLOCK_SIZE; ++v)
			sum += Dct::GetConstC(u) * Dct::GetConstC(v) / 4 *
				cos((2 * i + 1) * u * PI / 16) * cos((2 * j + 1) * v * PI / 16) * macroBlock[u][v];

	if (sum >= 0)
		return static_cast<int>(sum + 0.5);
	else
		return static_cast<int>(sum - 0.5);
}
