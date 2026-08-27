#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 pos, float fov_, float aspectRatio_, float near_, float far_);

	void handleMouseInput(int xMove, int yMove);

	void moveForward();
	void moveBackward();
	void moveRight();
	void moveLeft();
	void moveDown();
	void moveUp();

	void recalculatePerspectiveMatrix()
	{
		perspective = glm::perspective(glm::radians(fov), aspectRatio, near, far);
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(position, position + viewDirection, upDirection);
	}

	void setFov(float newFov)
	{
		fov = newFov;
		recalculatePerspectiveMatrix();
	}
	void setAspectRatio(int width, int height)
	{
		aspectRatio = (float)width / height;
		recalculatePerspectiveMatrix();
	}
	void setNear(float newNear)
	{
		near = newNear;
		recalculatePerspectiveMatrix();
	}
	void setFar(float newFar)
	{
		far = newFar;
		recalculatePerspectiveMatrix();
	}

	void increaseSpeed()
	{
		speed *= 2;
	}
	void decreaseSpeed()
	{
		speed /= 2;
	}

	glm::vec3 position;

	float sensitivity = .1f;
	float speed = .2f;
	
	float pitch = 0.f;
	float yaw = -90.f;

	glm::mat4 perspective;
private:
	glm::vec3 viewDirection;
	glm::vec3 upDirection;

	float fov;
	float aspectRatio;
	float near;
	float far;
};

