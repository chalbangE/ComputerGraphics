#include "stdafx.h"
#include "GLCube.h"

GLCube::GLCube(glm::vec3 m) : GLShapes(m) {
	glm::vec3 color[36];
	for (int i = 0; i < 36; ++i) {
		color[i] = { colorRd(gen), colorRd(gen), colorRd(gen) };
	}

	glGenBuffers(1, &v_color);
	glBindBuffer(GL_ARRAY_BUFFER, v_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
}

void GLCube::draw(std::string draw_Mod) {
	if ("solid" == draw_Mod)
		glDrawArrays(GL_TRIANGLES, 0, 36);
	else if ("line" == draw_Mod) {
		for (int i = 0; i < 36; i += 3)
			glDrawArrays(GL_LINE_LOOP, i, 3);
	}
}

void GLCube::draw_prepare(int Location, std::string Location_str) {
	if ("Pos" == Location_str) {
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_Cube);
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
