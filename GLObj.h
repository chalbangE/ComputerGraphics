#pragma once
#include "GLShapes.h"

class GLObj :
    public GLShapes
{
public:
    GLuint v_pos;
    GLint face_cnt = 0;

    GLObj(){}
    GLObj(glm::vec3 m);

    void objLoad(std::ifstream& path);

    void draw(std::string draw_Mod);

    void draw_prepare(int Location, std::string Location_str);

    void Update();
};

