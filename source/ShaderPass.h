#pragma once

#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <string>

class ShaderPass
{
public:
	ShaderPass* use()
	{
		glUseProgram(programID);
		return this;
	}

	void setMat4(std::string name, const glm::f32* valuePtr);
	void setMat4(GLint name, const glm::f32* valuePtr);

	void setInt(std::string name, int value);
protected:
	std::string loadShaderFromFile(std::string fileName)
	{
		std::string result = "";

		std::string line = "";
		std::ifstream myFile(fileName);

		if (myFile.is_open())
		{
			while (std::getline(myFile, line))
			{
				result += line + "\n";
			}
			myFile.close();
		}
		return result;
	}

	GLuint programID;
};

