#pragma once
#include "GLShapes.h"
class GLTri :
    public GLShapes
{
public:
	GLuint v_pos;
	glm::vec3 Anipos{ 0.f, 0.f, 0.f }; // 애니메이션을 위해 원점으로 원하는 만큼 이동하기

	GLTri() {};
	GLTri(glm::vec3 a);

	void draw(std::string draw_Mod);

	void draw_prepare(int Location, std::string Location_str);

	void Update();
};

