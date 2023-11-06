#pragma once
#include "GLShapes.h"

class GLObj :
    public GLShapes
{
public:
    GLuint v_pos;
    GLint face_cnt = 0;
    glm::vec3 Object_mid{ 0.f, 0.f, 0.f };
    glm::vec3 Ani_mid{ 0.f, 0.f, 0.f };

    GLObj(){}
    GLObj(glm::vec3 m);

    void objLoad(std::ifstream& path);

    void draw(std::string draw_Mod);

    void draw_prepare(int Location, std::string Location_str);

    void Update();
    void Update20();
    void Update22();
};

