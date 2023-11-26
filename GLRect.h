#pragma once
#include "GLShapes.h"
#include "stdafx.h"

class GLRect :
    public GLShapes
{
public:
	GLuint v_pos;
	glm::vec3 Anipos{ 0.f, 0.f, 0.f }; // 애니메이션을 위해 원점으로 원하는 만큼 이동하기
	glm::vec3 trun_theta{ 0.f, 0.f, 0.f };

	GLRect() {};
	GLRect(glm::vec3 a);
	GLRect(int a);
	
	void draw(std::string draw_Mod);

	void draw_prepare(int Location, std::string Location_str);

	void Update();
	void Update22();
};

