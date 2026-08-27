#pragma once

#include <map>
#include <array>
#include <glm/glm.hpp>

namespace BP
{
	static float atlasX = 0.03125f;
	static float atlasY = 0.0625f;
enum BlockPalette
{
	AIR = 0,
	DIRT,
	GRASS,
	STONE,
	COBBLESTONE
};

static std::map<BlockPalette, std::array<float, 8>> UVS = { //x0.03125, y0.0625
	{DIRT , {0.25f, 0.3125f,
			0.28125f, 0.3125f,
			0.28125f, 0.25f,
			0.25f, 0.25f,}},
	{STONE, {19 * atlasX, 11 * atlasY, //x19, y10
			20 * atlasX, 11 * atlasY,
			20 * atlasX, 10 * atlasY,
			19 * atlasX, 10 * atlasY}}
};
}
