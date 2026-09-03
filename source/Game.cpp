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

	SDL_Surface* textureAtlasSurface = IMG_Load("blocks-atlas.png");

	glGenTextures(1, &TO);
	glBindTexture(GL_TEXTURE_2D, TO);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 4);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureAtlasSurface->w, textureAtlasSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureAtlasSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(textureAtlasSurface);

	//ssao set up
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	glGenTextures(1, &ssaoColourBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColourBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "SSAO colour framebuffer not complete" << "\n";
	}

	//add blur stage later
	std::uniform_real_distribution<GLfloat> rf(0.0, 1.0);
	std::default_random_engine generator;
	randomFloats = &rf;
	
	for (int i = 0; i < 64; i++)
	{
		glm::vec3 sample{
			(*randomFloats)(generator) * 2.0 - 1.0f,
			(*randomFloats)(generator) * 2.0 - 1.0f,
			(*randomFloats)(generator)
		};
		sample = glm::normalize(sample);
		sample *= (*randomFloats)(generator);
		float scale = i / 64.f;
		scale *= scale * scale;

		scale = 0.1f + (scale * (1.0f - 0.1f));
		sample *= scale;

		ssaoKernel.push_back(sample);
	}

	for (int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			(*randomFloats)(generator) * 2.0 - 1.0f,
			(*randomFloats)(generator) * 2.0 - 1.0f,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &ssaoNoiseTexture);
	glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	camera = new Camera(glm::vec3(0.f, 1.f, 3.f), 45.5f, (float)width / height, 0.1f, 300.f);
	int platSize = 32;

	TG = std::shared_ptr<TerrainGenerator>(new TerrainGenerator(config["seed"]));

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

	GBuffer = new gBuffer(width, height);
	defaultShaderPass = new DefaultShaderPass("shaders/vert.glsl", "shaders/frag.glsl");
	//defaultShaderPass->use();

	deferredGeometryPass = new DefaultShaderPass("shaders/vert.glsl", "shaders/gBufferFrag.glsl");
	deferredGeometryPass->setInt("textureSampler", 0);

	deferredLightingPass = new DefaultShaderPass("shaders/deferredVert.glsl", "shaders/deferredFrag.glsl");
	deferredLightingPass->setInt("gPosition", 0);
	deferredLightingPass->setInt("gNormal", 1);
	deferredLightingPass->setInt("gAlbedo", 2);

	ssaoPass = new DefaultShaderPass("shaders/ssaoVert.glsl", "shaders/ssaoFrag.glsl");
	ssaoPass->setInt("gPosition", 0);
	ssaoPass->setInt("gNormal", 1);
	ssaoPass->setInt("gAlbedo", 2);

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

	GBuffer->bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	currentShader = deferredGeometryPass->use();

	currentShader->setMat4("perspective", glm::value_ptr(camera->perspective));

	glm::mat4 view = camera->getViewMatrix();
	currentShader->setMat4("view", glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TO);

	currentShader->setInt("textureSampler", 0);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	 
	for (Chunk* c : chunks){
		currentShader->setMat4("modelMatrix", glm::value_ptr(c->getModelMatrix()));
		c->draw();
	}

	//ssao pass
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	currentShader = ssaoPass->use();

	for (int i = 0; i < 64; i++)
	{
		currentShader->setVec3("samples[" + std::to_string(i) + "]", glm::value_ptr(ssaoKernel[i]));
	}
	currentShader->setMat4("perspective", glm::value_ptr(camera->perspective));
	currentShader->setMat4("view", glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GBuffer->gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GBuffer->gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//lighting pass

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	currentShader = deferredLightingPass->use();
	GBuffer->bindTextures();
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ssaoColourBuffer);
	currentShader->setInt("ssao", 3);

	renderQuad();
}

void Game::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
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

	delete GBuffer;
	delete defaultShaderPass;
	delete deferredGeometryPass;
	delete deferredLightingPass;
}