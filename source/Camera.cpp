#define GLM_ENABLE_EXPERIMENTAL

#include "Camera.h"
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

Camera::Camera(glm::vec3 pos, float fov_, float aspectRatio_, float near_, float far_)
{
	position = pos;
	upDirection = glm::vec3{0.f, 1.f, 0.f};
	viewDirection = glm::vec3{0.f, 0.f, -1.f};

	fov = fov_;
	aspectRatio = aspectRatio_;
	near = near_;
	far = far_;

	recalculatePerspectiveMatrix();
}

void Camera::handleMouseInput(int xMove, int yMove)
{
	yaw += sensitivity * xMove;
	pitch -= sensitivity * yMove;

	if (pitch > 89.f)
	{
		pitch = 89.f;
	}
	else if (pitch < -89.f)
	{
		pitch = -89.f;
	}

	glm::vec3 newDirection;

	newDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newDirection.y = sin(glm::radians(pitch));
	newDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	viewDirection = newDirection;
}

void Camera::moveForward()
{
	position += viewDirection * speed;
}
void Camera::moveBackward()
{
	position -= viewDirection * speed;
}
void Camera::moveRight()
{
	position += glm::cross(viewDirection, upDirection) * speed;
}
void Camera::moveLeft()
{
	position -= glm::cross(viewDirection, upDirection) * speed;
}

void Camera::moveUp()
{
	position.y += speed;
}

void Camera::moveDown()
{
	position.y -= speed;
}
