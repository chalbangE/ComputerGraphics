#include "GLTetrahedron.h"

GLTetrahedron::GLTetrahedron(glm::vec3 m) : GLShapes(m) 
{
	glm::vec3 color[12];
	for (int i = 0; i < 12; i += 3) {
		glm::vec3 c = { colorRd(gen), colorRd(gen), colorRd(gen) };
		color[i] = c;
		color[i + 1] = c;
		color[i + 2] = c;
	}

	glGenBuffers(1, &v_color);
	glBindBuffer(GL_ARRAY_BUFFER, v_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
}

void GLTetrahedron::draw()
{
	glDrawArrays(GL_TRIANGLES, 0, 15);
}

void GLTetrahedron::draw_prepare(int Location, std::string Location_str)
{
	if ("Pos" == Location_str) {
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_Tetra);
		glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else if ("Color" == Location_str) {
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else if ("World" == Location_str) {
		glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(World_mat));
	}
	//else if ("Tex" == str) {
	//	glBindBuffer(GL_ARRAY_BUFFER, m_TexVBO);
	//	glVertexAttribPointer(Loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//}

}
