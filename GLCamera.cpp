#include "GLCamera.h"

void GLCamera::Update()
{
	Camera_Mat = glm::lookAt(pos, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f });

	Camera_Mat = glm::rotate(Camera_Mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	Camera_Mat = glm::rotate(Camera_Mat, glm::radians(revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	Camera_Mat = glm::rotate(Camera_Mat, glm::radians(revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

	Camera_Mat = glm::translate(Camera_Mat, pos);
	Camera_Mat = glm::rotate(Camera_Mat, glm::radians(rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	Camera_Mat = glm::rotate(Camera_Mat, glm::radians(rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	Camera_Mat = glm::rotate(Camera_Mat, glm::radians(rotate_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
	Camera_Mat = glm::translate(Camera_Mat, -pos);
}
