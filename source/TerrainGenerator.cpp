#include "TerrainGenerator.h"
#include <iostream>

TerrainGenerator::TerrainGenerator(int seed_)
{
	seed = seed_;

	noise.SetSeed(seed);
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(0.01f);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(4);

	PVnoise.SetSeed(seed);
	PVnoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	PVnoise.SetFrequency(0.01f);
}

BP::BlockPalette TerrainGenerator::getBlock(int x, int y, int z)
{
	double noiseNum;
	double groundLevel;

	//noiseNum = PN.noise3D(x * frequency, y * frequency, z * frequency);
	noiseNum = noise.GetNoise((float)x, (float)y, (float)z);
	groundLevel = (PVnoise.GetNoise((float)x, (float)z) - y) * squashFactor;
	//std::cout << groundLevel << "\n";
	noiseNum += groundLevel;

	if (noiseNum > 0.f)
	{
		return BP::DIRT;
	}
	else
	{
		return BP::AIR;
	}
}
