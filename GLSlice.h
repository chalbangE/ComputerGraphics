#pragma once
#include "GLShapes.h"
#include "stdafx.h"

class GLSlice :
    public GLShapes
{
public:
	GLuint v_pos;
	GLuint line_pos;
	GLuint line_color;
	int line_cnt = 0;
	int summit_cnt = 0;
	glm::vec3 dis;
	GLfloat r = 0.f;
	std::vector <glm::vec3> firstpos;
	bool on = true;
	bool slice = false;
	bool cut = false;
	bool basket_in = false;
	int whereCross[2];
	GLfloat Theta;

	GLSlice();
	GLSlice(int i);
	GLSlice(std::vector <glm::vec3> first, GLfloat dis_x);
	virtual ~GLSlice();

	void draw(std::string draw_Mod);
	void draw_prepare(int Location, std::string Location_str);
	void Update();
	void Update(GLfloat Speed, glm::vec3 basket[4]);
};

static std::uniform_int_distribution<int> summit(3, 8);
static std::uniform_int_distribution<int> qkdgid(1, 2);
static std::uniform_real_distribution<GLclampf> Thetard(0.0f, 360.0f);
static std::uniform_real_distribution<GLclampf> Disrd(0.005f, 0.01f);
static std::uniform_real_distribution<GLclampf> DisYrd(0.05f, 0.06f);
static std::uniform_real_distribution<GLclampf> posrd(-1.f, 0.f);
static std::uniform_real_distribution<GLclampf> Rrd(0.2f, 0.3f);