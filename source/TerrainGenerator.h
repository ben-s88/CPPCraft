#pragma once

#include <PerlinNoise.hpp>
#include <FastNoiseLite.h>
#include <BlockPalette.h>

class TerrainGenerator
{
public:
	TerrainGenerator(int seed_);

	int getSeed()
	{
		return seed;
	}

	BP::BlockPalette getBlock(int x, int y, int z);

	float squashFactor = 0.023f;
	int baseHeight = 40;
private:
	siv::PerlinNoise PN{0};
	FastNoiseLite noise;
	FastNoiseLite PVnoise;
	int seed;
};

