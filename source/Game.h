#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <json.hpp>
using json = nlohmann::json;

#include <Camera.h>
#include <BlockGeneric.h>
#include <Chunk.h>

class TerrainGenerator;
class Game
{
public:
	Game();
	bool init(bool vsync);

	void gameLoop();
	void handleInput();
	void update();
	void draw();

	void cleanUp();

	void printProgramLog(GLuint program);
	void printShaderLog(GLuint shader);

	void setWindowTitle(const char* title)
	{
		SDL_SetWindowTitle(window, title);
	}

	bool quit = false;
private:
	json config;
	int height;
	int width;

	int lastTime = 0;
	int frameTime = 0;

	SDL_Window* window = nullptr;
	std::string title = "CPP Craft";
	SDL_GLContext GLContext = nullptr;

	Camera* camera = nullptr;

	std::shared_ptr<TerrainGenerator> TG;
	Chunk* myChunk = nullptr;
	std::vector<Chunk*> chunks;

	std::vector<BlockGeneric> blocks;

	GLuint programID = 0;

	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint IBO = 0;
	GLuint TO = 0;


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
};
