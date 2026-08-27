#include "Game.h"
#include <glm/gtc/type_ptr.hpp>

#include <PerlinNoise.hpp>
#include <TerrainGenerator.h>
#include <omp.h>

Game::Game()
{ }

bool Game::init(bool vsync)
{
	std::ifstream configFile("config.json");
	config = json::parse(configFile);
	configFile.close();

	width = config["width"];
	height = config["height"];

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initalise video: " << SDL_GetError() << "\n";
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow(title.c_str(), 100, 100, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	if (!window)
	{
		std::cout << "SDL could not create window: " << SDL_GetError() << "\n";
		return false;
	}

	GLContext = SDL_GL_CreateContext(window);

	if (!GLContext)
	{
		std::cout << "SDL could not create GL context: " << SDL_GetError() << "\n";
		return false;
	}

	if (SDL_GL_SetSwapInterval(config["vsync"]) != 0)
	{
		std::cout << "Unable to set vsync: " << SDL_GetError() << "\n";
	}

	if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0)
	{
		std::cout << "Unable to set mouse mode: " << SDL_GetError() << "\n";
	}

	GLenum glewStatus = glewInit();
	if (glewStatus != GLEW_OK)
	{
		std::cout << "glew could not be initalised: " << glewGetErrorString(glewStatus) << "\n";
		return false;
	}

	std::string vertexShaderSource = loadShaderFromFile("shaders/vert.glsl");
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
		std::cout << "Vertex shader failed to compile: " << message << "\n";
	}

	std::string fragmentShaderSource = loadShaderFromFile("shaders/frag.glsl");
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
		std::cout << "Fragment shader failed to compile: " << message << "\n";
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

	/*std::vector<GLfloat> vertexData{ //u step 0.03125 - v step 0.0625
	-1.f, -1.f, 1.f,     0.25f, 0.3125f, //0 bottom left
	1.f, -1.f, 1.f,     0.28125f, 0.3125f, // bottom right
	-1.f, 1.f, 1.f,     0.25f, 0.25f, // top left
	1.f, 1.f, 1.f,     0.28125f, 0.25f, // top right

	1.f, -1.f, -1.f,     0.28125f, 0.3125f, // bottom right back
	1.f, 1.f, -1.f,     0.28125f, 0.25f, // top right back

	-1.f, -1.f, -1.f,     0.25f, 0.3125f, // bottom left back
	-1.f, 1.f, -1.f,     0.25f, 0.25f, //7 top left back
	};*/

	std::vector<GLfloat> vertexData{
		// z+
		0.0f, 0.0f,  0.0f,    0.25f, 0.3125f,
		1.0f, 0.0f,  0.0f,    0.28125f, 0.3125f,
		1.0f, 1.0f,  0.0f,    0.28125f, 0.25f,
		0.0f, 1.0f,  0.0f,    0.25f, 0.25f,

		// z-
		1.0f, 0.0f, -1.0f,    0.25f, 0.3125f,
		0.0f, 0.0f, -1.0f,    0.28125f, 0.3125f,
		0.0f, 1.0f, -1.0f,    0.28125f, 0.25f,
		1.0f, 1.0f, -1.0f,    0.25f, 0.25f,

		//x-
		0.0f, 0.0f, -1.0f,    0.25f, 0.3125f,
		0.0f, 0.0f,  0.0f,    0.28125f, 0.3125f,
		0.0f, 1.0f,  0.0f,    0.28125f, 0.25f,
		0.0f, 1.0f, -1.0f,    0.25f, 0.25f,

		//x+
		1.0f, 0.0f,  0.0f,    0.25f, 0.3125f,
		1.0f, 0.0f, -1.0f,    0.28125f, 0.3125f,
		1.0f, 1.0f, -1.0f,    0.28125f, 0.25f,
		1.0f, 1.0f,  0.0f,    0.25f, 0.25f,

		//y+
		0.0f, 1.0f,  0.0f,    0.25f, 0.3125f,
		1.0f, 1.0f,  0.0f,    0.28125f, 0.3125f,
		1.0f, 1.0f, -1.0f,    0.28125f, 0.25f,
		0.0f, 1.0f, -1.0f,    0.25f, 0.25f,

		//y-
		0.0f, 0.0f, -1.0f,    0.25f, 0.3125f,
		1.0f, 0.0f, -1.0f,    0.28125f, 0.3125f,
		1.0f, 0.0f,  0.0f,    0.28125f, 0.25f,
		0.0f, 0.0f,  0.0f,    0.25f, 0.25f,
	};


	std::vector<GLuint> vertexIndicies{ // block atlas is 32(23 textures + white space)x16 - each texture 16x16 pixel 
		1,  0,  2,   3,  2,  0,   // Front
		5,  4,  6,   7,  6,  4,   // Back
		9,  8,  10,  11, 10, 8,   // Left
		13, 12, 14,  15, 14, 12,   // Right
		17, 16, 18,  19, 18, 16,   // Top
		21, 20, 22,  23, 22, 20    // Bottom
	};

	SDL_Surface* textureAtlasSurface = IMG_Load("blocks-atlas.png");

	glGenTextures(1, &TO);
	glBindTexture(GL_TEXTURE_2D, TO);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 4);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureAtlasSurface->w, textureAtlasSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureAtlasSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(textureAtlasSurface);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndicies.size() * sizeof(GLuint), vertexIndicies.data(), GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	GLint lightPosUniform = glGetUniformLocation(programID, "lightPos");
	glUniform3f(programID, 0.f, 10.f, 0.f);

	camera = new Camera(glm::vec3(0.f, 1.f, 3.f), 45.5f, (float)width / height, 0.1f, 300.f);
	int platSize = 32;

	siv::PerlinNoise perlin{ 1 };
	int octaves = 4;
	double heightFactor = 50;
	float resolution = 200.f;
	double noiseNum;

	TG = std::shared_ptr<TerrainGenerator>(new TerrainGenerator(config["seed"]));
	std::cout << config["seed"];

	//Chunk* c;
	#pragma omp parallel for
	for (int y = -1; y < 4; y++)
	{
	for (int x = -10; x < 10; x++)
	{
		for (int z = -10; z < 10; z++)
		{
			Chunk* c = new Chunk(glm::vec3{ x * 16, y * 16, z * 16}, TG);
			c->generate();
			c->generateMesh();

			#pragma omp critical
			{
				chunks.push_back(c);
			}
		}
	}
	}

	for (Chunk* c : chunks)
	{
		c->initBuffer();
	}

	return true;
}

void Game::gameLoop()
{
	handleInput();
	update();
	draw();

	SDL_GL_SwapWindow(window);
}

void Game::handleInput()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				quit = true;
				break;
			case SDLK_w:
				camera->moveForward();
				break;
			case SDLK_s:
				camera->moveBackward();
				break;
			case SDLK_a:
				camera->moveLeft();
				break;
			case SDLK_d:
				camera->moveRight();
				break;
			case SDLK_SPACE:
				camera->moveUp();
				break;
			case SDLK_c:
				camera->moveDown();
				break;
			case SDLK_UP:
				camera->increaseSpeed();
				break;
			case SDLK_DOWN:
				camera->decreaseSpeed();
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			camera->handleMouseInput(e.motion.xrel, e.motion.yrel);
		default:
			break;
		}
	}
}

void Game::update()
{
}

void Game::draw()
{
	glViewport(0, 0, width, height);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(programID);

	GLint ModelMatrixLocation = glGetUniformLocation(programID, "modelMatrix");

	GLint PerspectiveMatrixLocation = glGetUniformLocation(programID, "perspective");
	glProgramUniformMatrix4fv(programID, PerspectiveMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera->perspective));

	glm::mat4 view = camera->getViewMatrix();
	GLint ViewMatrixLocation = glGetUniformLocation(programID, "view");
	glProgramUniformMatrix4fv(programID, ViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TO);

	GLint textureSampler = glGetUniformLocation(programID, "textureSampler");
	glUniform1i(textureSampler, 0);

	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	for (Chunk* c : chunks){
		glProgramUniformMatrix4fv(programID, ModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(c->getModelMatrix()));
		c->draw();
	}
}

void Game::cleanUp()
{
	SDL_DestroyWindow(window);
	SDL_Quit();

	for (Chunk* c : chunks) {
		delete c;
	}

	delete camera;
	delete myChunk;
}