#include "GLRect.h"

GLRect::GLRect(glm::vec3 m) : GLShapes(m) {
	glm::vec3 color[6];
	color[0] = { colorRd(gen), colorRd(gen), colorRd(gen) };
	for (int i = 1; i < 6; ++i) {
		color[i] = color[0];
	}

	glGenBuffers(1, &v_color);
	glBindBuffer(GL_ARRAY_BUFFER, v_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
}

GLRect::GLRect(int a) {
	glm::vec3 color[6];
	GLfloat c = colorRd(gen) - 0.1f;
	color[0] = { c, c, c };
	for (int i = 1; i < 6; ++i) {
		color[i] = color[0];
	}

	glGenBuffers(1, &v_color);
	glBindBuffer(GL_ARRAY_BUFFER, v_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
}

void GLRect::draw(std::string draw_Mod) {
	if ("solid" == draw_Mod)
		glDrawArrays(GL_TRIANGLES, 0, 6);
	else if ("line" == draw_Mod) {
		for (int i = 0; i < 6; i += 3)
			glDrawArrays(GL_LINE_LOOP, i, 3);
	}
}

void GLRect::draw_prepare(int Location, std::string Location_str) {
	if ("Pos" == Location_str) {
		glBindBuffer(GL_ARRAY_BUFFER, v_pos);
		glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else if ("Color" == Location_str) {
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else if ("World" == Location_str) {
		glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(World_mat));
	}
}

void GLRect::Update()
{
	World_mat = glm::mat4(1.0);

	World_mat = glm::scale(World_mat, Oscale);
	
	World_mat = glm::rotate(World_mat, glm::radians(trun_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(trun_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(trun_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

	World_mat = glm::translate(World_mat, Anipos);

	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

	World_mat = glm::translate(World_mat, -Anipos);

	World_mat = glm::translate(World_mat, pos);

	World_mat = glm::translate(World_mat, midpos);

	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));


	World_mat = glm::scale(World_mat, scale);

	World_mat = glm::translate(World_mat, -midpos);
}

void GLRect::Update22()
{
	World_mat = glm::mat4(1.0);

	World_mat = glm::scale(World_mat, Oscale);

	World_mat = glm::translate(World_mat, pos);
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
	World_mat = glm::translate(World_mat, -pos);

	World_mat = glm::translate(World_mat, pos);

	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

	World_mat = glm::scale(World_mat, scale);

	World_mat = glm::translate(World_mat, -midpos);
}