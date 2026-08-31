#include "ShaderPass.h"

void ShaderPass::setMat4(std::string name, const glm::f32* valuePtr)
{
	GLint location = glGetUniformLocation(programID, name.c_str());

	glProgramUniformMatrix4fv(programID, location, 1, GL_FALSE, valuePtr);
}

void ShaderPass::setMat4(GLint name, const glm::f32* valuePtr)
{
	glProgramUniformMatrix4fv(programID, name, 1, GL_FALSE, valuePtr);
}

void ShaderPass::setInt(std::string name, int value)
{
	GLint location = glGetUniformLocation(programID, name.c_str());

	glProgramUniform1i(programID, location, value);
}
