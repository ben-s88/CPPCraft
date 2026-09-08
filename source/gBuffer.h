#pragma once
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>

class gBuffer
{
public:
	gBuffer(int width, int height);

	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
	}
	void unBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void bindTextures()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
	}

	GLuint gPosition;
	GLuint gNormal;
	GLuint gAlbedo;
private:
	GLuint _gBuffer;
	GLuint rboDepth;
};

