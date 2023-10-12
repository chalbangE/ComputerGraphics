#pragma once

#include "stdafx.h"
#include "GLShapes.h"
class GLPyramid :
    public GLShapes
{
public:
	GLPyramid() {};
	GLPyramid(glm::vec3 m);
	virtual ~GLPyramid() {};

	void draw(std::string draw_Mod);

	void draw_prepare(int Location, std::string Location_str);
};