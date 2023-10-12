#pragma once
#include "GLShapes.h"
class GLTetrahedron :
    public GLShapes
{
public:
	GLTetrahedron() {};
	GLTetrahedron(glm::vec3 m);

	void draw();

	void draw_prepare(int Location, std::string Location_str);
};

