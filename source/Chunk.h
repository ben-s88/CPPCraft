#pragma once

#include <BlockGeneric.h>
#include <array>
#include <vector>
#include <gl/glew.h>
#include <gl/GLU.h>
#include <memory>

class TerrainGenerator;
class Chunk
{
public:
	Chunk(glm::vec3 pos_, std::shared_ptr<TerrainGenerator> TG_);

	glm::mat4 getModelMatrix()
	{
		return glm::translate(glm::mat4(1.0f), glm::vec3(pos));
	}

	void generate();
	void generateMesh();
	void initBuffer();
	void draw();

	std::shared_ptr<TerrainGenerator> TG;
private:
	glm::ivec3 pos{};
	glm::mat4 modelMatrix;

	std::array<std::array<std::array<BlockGeneric, 16>, 16>, 16> blocks{}; // y, x, z
	std::vector<GLfloat> meshVS;
	std::vector<GLuint> meshVIs;
	int indexOffset = 24;

	GLuint VBO = 0;
	GLuint IBO = 0;
	GLuint VAO = 0;
};

