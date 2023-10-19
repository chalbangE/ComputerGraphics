#version 330 core

layout (location = 0) in vec3 in_Position; //--- 위치 변수: attribute position 0
layout (location = 1) in vec3 in_Color; //--- 컬러 변수: attribute position 1
out vec3 out_Color; //--- 프래그먼트 세이더에게 전달
uniform mat4 World_trans;
uniform mat4 Camera_trans;
uniform mat4 Projection_trans;

void main(void) 
{
    gl_Position = Projection_trans * Camera_trans * World_trans * vec4 (in_Position.x, in_Position.y, in_Position.z, 1.0);
    out_Color = in_Color;
}