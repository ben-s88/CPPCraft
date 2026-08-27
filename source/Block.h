#pragma once

#include <BlockPalette.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Block
{
public:
	BP::BlockPalette blockType{};
	glm::vec<3, uint8_t, glm::packed_highp> pos{};
};

