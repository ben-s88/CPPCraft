#include "DefaultShaderPass.h"
#include <iostream>

DefaultShaderPass::DefaultShaderPass(std::string vertFilePath, std::string fragFilePath)
{
	std::string vertexShaderSource = loadShaderFromFile(vertFilePath);
	auto vertexShaderSourcePTR = vertexShaderSource.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSourcePTR, NULL);
	glCompileShader(vertexShader);

	GLint vertexCompiled;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexCompiled);
	if (vertexCompiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(vertexShader, 1024, &log_length, message);
		std::cout << "Vertex shader failed to compile: " << vertFilePath << " " << message << "\n";
	}

	std::string fragmentShaderSource = loadShaderFromFile(fragFilePath);
	auto fragmentShaderSourcePTR = fragmentShaderSource.c_str();

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSourcePTR, NULL);
	glCompileShader(fragmentShader);

	GLint fragmentCompiled;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentCompiled);
	if (fragmentCompiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(fragmentShader, 1024, &log_length, message);
		std::cout << "Fragment shader failed to compile: " << fragFilePath << " " << message << "\n";
	}

	programID = glCreateProgram();

	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	GLint program_linked;
	glGetProgramiv(programID, GL_LINK_STATUS, &program_linked);
	if (program_linked != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetProgramInfoLog(programID, 1024, &log_length, message);
		std::cout << "Program failed to link: " << message << "\n";
	}

	GLint lightPosUniform = glGetUniformLocation(programID, "lightPos");
	glUniform3f(programID, 0.f, 10.f, 0.f);
}
