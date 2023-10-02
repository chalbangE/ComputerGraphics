#version 330 core

//--- in_Position: attribute index 0 
//--- in_Color: attribute index 1

layout (location = 0) in vec3 in_Position; //--- 위치 변수: attribute position 0
layout (location = 1) in vec3 in_Color; //--- 컬러 변수: attribute position 1
out vec3 out_Color; //--- 프래그먼트 세이더에게 전달
uniform float u_Te;
uniform vec2 xy;

void main(void) 
{
	gl_Position = vec4 (((in_Position.x - xy.x) * cos(u_Te)) - ((in_Position.y - xy.y) * sin(u_Te)) + xy.x, ((in_Position.x - xy.x) * sin(u_Te)) + ((in_Position.y - xy.y) * cos(u_Te)) + xy.y, in_Position.z, 1.0);
	out_Color = in_Color;
}	