#include "Chunk.h"
#include <TerrainGenerator.h>
#include <iostream>

Chunk::Chunk(glm::vec3 pos_ ,std::shared_ptr<TerrainGenerator> TG_)
{
	TG = std::shared_ptr<TerrainGenerator>(TG_);
	pos = pos_;
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
}
void Chunk::generate()
{
	for (int y = 0; y < 16; y++)
		for (int x = 0; x < 16; x++)
			for (int z = 0; z < 16; z++)
			{
				blocks[y][x][z].blockType = TG->getBlock(x + pos.x, y + pos.y, z + pos.z);
			}
}

void Chunk::generateMesh()
{
	meshVS.reserve(100);
	int x, y, z;
	unsigned int count = 0;
	x = y = z = 0;
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				if (blocks[y][x][z].blockType == BP::AIR) { continue; }
				BlockGeneric& currentBlock = blocks[y][x][z];

				auto& uvs = BP::UVS[currentBlock.blockType];

				meshVS.insert(meshVS.end(), {
					0.0f + x, 0.0f + y, 0.0f + z,   uvs[0], uvs[1], 0.f, 0.f, 1.f, // z+
					1.0f + x, 0.0f + y, 0.0f + z,   uvs[2], uvs[3], 0.f, 0.f, 1.f,
					1.0f + x, 1.0f + y, 0.0f + z,   uvs[4], uvs[5], 0.f, 0.f, 1.f,
					0.0f + x, 1.0f + y, 0.0f + z,   uvs[6], uvs[7], 0.f, 0.f, 1.f,

					// z-
					1.0f + x, 0.0f + y, -1.0f + z,  uvs[0], uvs[1], 0.f, 0.f, -1.f,
					0.0f + x, 0.0f + y, -1.0f + z,  uvs[2], uvs[3], 0.f, 0.f, -1.f,
					0.0f + x, 1.0f + y, -1.0f + z,  uvs[4], uvs[5], 0.f, 0.f, -1.f,
					1.0f + x, 1.0f + y, -1.0f + z,  uvs[6], uvs[7], 0.f, 0.f, -1.f,

					//x-
					0.0f + x, 0.0f + y, -1.0f + z,  uvs[0], uvs[1], -1.f, 0.f, 0.f,
					0.0f + x, 0.0f + y, 0.0f + z,   uvs[2], uvs[3], -1.f, 0.f, 0.f,
					0.0f + x, 1.0f + y, 0.0f + z,   uvs[4], uvs[5], -1.f, 0.f, 0.f,
					0.0f + x, 1.0f + y, -1.0f + z,  uvs[6], uvs[7], -1.f, 0.f, 0.f,

					//x+
					1.0f + x, 0.0f + y, 0.0f + z,   uvs[0], uvs[1], 1.f, 0.f, 0.f,
					1.0f + x, 0.0f + y, -1.0f + z,  uvs[2], uvs[3], 1.f, 0.f, 0.f,
					1.0f + x, 1.0f + y, -1.0f + z,  uvs[4], uvs[5], 1.f, 0.f, 0.f,
					1.0f + x, 1.0f + y, 0.0f + z,   uvs[6], uvs[7], 1.f, 0.f, 0.f,

					//y+
					0.0f + x, 1.0f + y, 0.0f + z,   uvs[0], uvs[1], 0.f, 1.f, 0.f,
					1.0f + x, 1.0f + y, 0.0f + z,   uvs[2], uvs[3], 0.f, 1.f, 0.f,
					1.0f + x, 1.0f + y, -1.0f + z,  uvs[4], uvs[5], 0.f, 1.f, 0.f,
					0.0f + x, 1.0f + y, -1.0f + z,  uvs[6], uvs[7], 0.f, 1.f, 0.f,

					//y-
					0.0f + x, 0.0f + y, -1.0f + z,  uvs[0], uvs[1], 0.f, -1.f, 0.f,
					1.0f + x, 0.0f + y, -1.0f + z,  uvs[2], uvs[3], 0.f, -1.f, 0.f,
					1.0f + x, 0.0f + y, 0.0f + z,   uvs[4], uvs[5], 0.f, -1.f, 0.f,
					0.0f + x, 0.0f + y, 0.0f + z,   uvs[6], uvs[7], 0.f, -1.f, 0.f }
					);

				/*meshVIs.insert(meshVIs.end(), {
						0 + (count * indexOffset),  1 + (count * indexOffset),  2 + (count * indexOffset),   0 + (count * indexOffset),  2 + (count * indexOffset),  3 + (count * indexOffset),   // Front
						5 + (count * indexOffset),  4 + (count * indexOffset),  6 + (count * indexOffset),   7 + (count * indexOffset),  6 + (count * indexOffset),  4 + (count * indexOffset),   // Back
						9 + (count * indexOffset),  8 + (count * indexOffset),  10 + (count * indexOffset),  11 + (count * indexOffset), 10 + (count * indexOffset), 8 + (count * indexOffset),   // Left
						13 + (count * indexOffset), 12 + (count * indexOffset), 14 + (count * indexOffset),  15 + (count * indexOffset), 14 + (count * indexOffset), 12 + (count * indexOffset),   // Right
						17 + (count * indexOffset), 16 + (count * indexOffset), 18 + (count * indexOffset),  19 + (count * indexOffset), 18 + (count * indexOffset), 16 + (count * indexOffset),   // Top
						21 + (count * indexOffset), 20 + (count * indexOffset), 22 + (count * indexOffset),  23 + (count * indexOffset), 22 + (count * indexOffset), 20 + (count * indexOffset)    // Bottom
					});*/
				meshVIs.insert(meshVIs.end(), {
					// z+  (normal  0,  0, +1)
					0 + (count * indexOffset), 1 + (count * indexOffset), 2 + (count * indexOffset),
					0 + (count * indexOffset), 2 + (count * indexOffset), 3 + (count * indexOffset),

					// z-  (normal  0,  0, -1)
					4 + (count * indexOffset), 5 + (count * indexOffset), 6 + (count * indexOffset),
					4 + (count * indexOffset), 6 + (count * indexOffset), 7 + (count * indexOffset),

					// x-  (normal -1,  0,  0)
					8 + (count * indexOffset), 9 + (count * indexOffset), 10 + (count * indexOffset),
					8 + (count * indexOffset), 10 + (count * indexOffset), 11 + (count * indexOffset),

					// x+  (normal +1,  0,  0)
					12 + (count * indexOffset), 13 + (count * indexOffset), 14 + (count * indexOffset),
					12 + (count * indexOffset), 14 + (count * indexOffset), 15 + (count * indexOffset),

					// y+  (normal  0, +1,  0)
					16 + (count * indexOffset), 17 + (count * indexOffset), 18 + (count * indexOffset),
					16 + (count * indexOffset), 18 + (count * indexOffset), 19 + (count * indexOffset),

					// y-  (normal  0, -1,  0)
					20 + (count * indexOffset), 21 + (count * indexOffset), 22 + (count * indexOffset),
					20 + (count * indexOffset), 22 + (count * indexOffset), 23 + (count * indexOffset)
					});
				count++;
			}
		}
	}
}

void Chunk::initBuffer()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, meshVS.size() * sizeof(GLfloat), meshVS.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshVIs.size() * sizeof(GLuint), meshVIs.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 5));

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Chunk::draw()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glDrawElements(GL_TRIANGLES, meshVIs.size(), GL_UNSIGNED_INT, 0);
}
