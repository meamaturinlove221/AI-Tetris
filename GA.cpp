#include <cmath>
#include <fstream>
#include "Tetris.h"

using namespace std;

void Tetris::sorting()
{
	GAINFO* info = (GAINFO*)infoFile;
	int left, right, temp, mid;
	int i, j, k;
	float temp2[NUM_WEIGHTS];
	for (i = 1; i < POPULATION_SIZE; i++)
	{
		if (info->allFitness[i] < info->allFitness[i - 1])
		{
			temp = info->allFitness[i];
			for (k = 0; k < NUM_WEIGHTS; k++)
			{
				temp2[k] = info->allWeights[i][k];
			}
			left = 0;
			right = i - 1;
			while (left <= right)
			{
				mid = (left + right) / 2;
				if (info->allFitness[mid] < temp)
				{
					left = mid + 1;
				}
				else
				{
					right = mid - 1;
				}
			}
			for (j = i; j > left; j--)
			{
				info->allFitness[j] = info->allFitness[j - 1];
				for (k = 0; k < NUM_WEIGHTS; k++)
				{
					info->allWeights[j][k] = info->allWeights[j - 1][k];
				}
			}
			info->allFitness[left] = temp;
			for (k = 0; k < NUM_WEIGHTS; k++)
			{
				info->allWeights[left][k] = temp2[k];
			}
		}
	}
}

void Tetris::saveInfo()
{
	GAINFO* info = (GAINFO*)infoFile;
	FILE* file;

	fopen_s(&file, "generateInfo.txt", "wb+");
	memcpy(buffer, infoFile, FILE_SIZE);
	fwrite(buffer, FILE_SIZE, 1, file);
	fclose(file);
}

void Tetris::generateInitialWeights()
{
	infoFile = (unsigned char*)malloc(FILE_SIZE);
	GAINFO* info = (GAINFO*)infoFile;

	FILE* file;
	if (fopen_s(&file, "generateInfo.txt", "rb+") == 0)
	{
		fread(buffer, FILE_SIZE, 1, file);
		memcpy(infoFile, buffer, FILE_SIZE);
		fclose(file);
	}
	else
	{
		if (info)
		{
			info->generationNum = 100;
			info->count = 0;
			info->bestFitness = 0;
			for (int j = 0; j < NUM_WEIGHTS; j++)
			{
				info->bestWeights[j] = 0;
			}
			float k = 0;
			for (int i = 0; i < POPULATION_SIZE; i++)
			{
				float norm = 0;
				for (int j = 0; j < NUM_WEIGHTS; j++)
				{
					k = (rand() % 2000) * 0.001 - 1;
					info->allWeights[i][j] = k;
					norm += k * k;
				}
				norm = sqrtf(norm);
				for (int j = 0; j < NUM_WEIGHTS; j++)
				{
					info->allWeights[i][j] /= norm;
				}
				info->allFitness[i] = 0;
			}
		}
	}


	fopen_s(&file, "generateInfo.txt", "wb+");
	memcpy(buffer, infoFile, FILE_SIZE);
	fwrite(buffer, FILE_SIZE, 1, file);
	fclose(file);
}

void Tetris::generateChildren()
{
	GAINFO* info = (GAINFO*)infoFile;
	int i, j, k;
	int index1, index2, temp, indexWeight;
	float offspring[300][NUM_WEIGHTS], norm, tempf, change;

	sorting();
	for (i = 0; i < 300; i++)
	{
		index1 = 0;
		index2 = 0;
		for (j = 0; j < 100; j++)
		{
			temp = rand() % POPULATION_SIZE;
			if (info->allFitness[temp] > info->allFitness[index1]) 
			{
				index2 = index1;
				index1 = temp;
			}
			else if (info->allFitness[temp] > info->allFitness[index2]) 
			{
				index2 = temp;
			}
		}
		//crossover
		norm = 0;
		for (k = 0; k < NUM_WEIGHTS; k++)
		{
			offspring[i][k] = info->allWeights[index1][k] * info->allFitness[index1]
				+ info->allWeights[index2][k] * info->allFitness[index2];
			norm += offspring[i][k] * offspring[i][k];
		}
		norm = sqrtf(norm);
		for (k = 0; k < NUM_WEIGHTS; k++)
		{
			offspring[i][k] /= norm;
		}

		tempf = (rand() % 1000) * 0.001;
		if (tempf < 0.05)
		{
			norm = 0;
			indexWeight = rand() % NUM_WEIGHTS;
			change = (rand() % 1000) * 0.0004 - 0.2;
			for (k = 0; k < NUM_WEIGHTS; k++)
			{
				offspring[i][k] += change;
				norm += offspring[i][k] * offspring[i][k];
			}
			norm = sqrtf(norm);
			for (k = 0; k < NUM_WEIGHTS; k++)
			{
				offspring[i][k] /= norm;
			}
		}
	}
	for (i = 0; i < 300; i++)
	{
		for (k = 0; k < NUM_WEIGHTS; k++)
		{
			info->allWeights[i][k] = offspring[i][k];
		}
	}

	for (i = 0; i < POPULATION_SIZE; i++)
	{
		info->allFitness[i] = 0;
	}
}