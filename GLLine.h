#pragma once
#include "stdafx.h"
#include "GLShapes.h"
class GLLine :
    public GLShapes
{
public:
	GLLine() {};
	GLLine(glm::vec3 m);
	virtual ~GLLine() {};

	void draw();

	void draw_prepare(int Location, std::string Location_str);
};

