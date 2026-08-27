#pragma once
#include <Block.h>

class BlockGeneric : public Block
{
public:
	BlockGeneric()
	{
		pos = glm::ivec3(0);
		blockType = BP::AIR;
	}
	BlockGeneric(int x, int y, int z, BP::BlockPalette blockType_)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
		blockType = blockType_;
	}

	glm::mat4& getModelMatrix()
	{
		glm::mat4 mm = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
		return mm;
	}
};

