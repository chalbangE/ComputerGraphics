#pragma once
#include "stdafx.h"
#include "GLShapes.h"

class GLCube :
    public GLShapes
{
public:
	GLCube() {};
	GLCube(glm::vec3 m);
	virtual ~GLCube() {};

	void draw(std::string draw_Mod);

	void draw_prepare(int Location, std::string Location_str);
};

