#pragma once
#include "stdafx.h"

class GLShapes
{
public:
	glm::vec3 rotate_theta{ 0.0f, 0.0f, 0.0f }; // 회전값 x y z
	glm::vec3 revolve_theta{ 0.0f, 0.0f, 0.0f }; // 회전값 x y z
	glm::vec3 pos{ 0.0f, 0.0f, 0.0f }; // 객체의 중점
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f }; // 확대축소용
	glm::mat4 World_mat;
	GLuint v_color{};

	GLShapes() {}
	GLShapes(glm::vec3 m) : pos(m) { }
	virtual ~GLShapes() {};

	void Update();
};

