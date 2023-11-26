#include "GLSlice.h"

GLSlice::GLSlice() {
	summit_cnt = summit(rd);
	if (qkdgid(rd) == 1) {
		dis = glm::vec3{ Disrd(rd), DisYrd(rd) / 2.f, 0.f };
		pos = { -1.f, posrd(rd), 1.f };
	}
	else {
		dis = glm::vec3{ -Disrd(rd), DisYrd(rd) / 2.f, 0.f };
		pos = { 1.f, posrd(rd), 1.f };
	}

	std::vector <glm::vec3> color;

	color.emplace_back(colorRd(gen), colorRd(gen), colorRd(gen));
	for (int i = 1; i < summit_cnt; ++i) {
		color.emplace_back(color[0]);
	}

	r = Rrd(rd);
	Theta = Thetard(rd);
	GLfloat radian = Theta * 0.0174533f;

	for (int i = 0; i < summit_cnt; ++i) {
		glm::vec3 m = { 0.0f, 0.0f, 1.0f };
		m.x = pos.x + r * cos(radian);
		m.y = pos.y + r * sin(radian);
		firstpos.emplace_back(m);

		Theta += 360.0f / summit_cnt;
		radian = Theta * 0.0174533f;
		if (Theta >= 360.0f) {
			Theta -= 360.0f;
		}
	}

	std::vector <glm::vec3> move;
	std::vector <glm::vec3> col;

	glGenBuffers(1, &v_pos);
	glBindBuffer(GL_ARRAY_BUFFER, v_pos);
	glBufferData(GL_ARRAY_BUFFER, firstpos.size() * sizeof(glm::vec3), firstpos.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &v_color);
	glBindBuffer(GL_ARRAY_BUFFER, v_color);
	glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);

	move.clear();
	col.clear();
	glm::vec3 saveDis = dis;
	glm::vec3 savePos = pos;
	while (savePos.y >= -1.f) {
		saveDis.y -= 0.0003f;
		savePos += saveDis;
		saveDis.y -= 0.0003f;
		savePos += saveDis;
		saveDis.y -= 0.0003f;
		savePos += saveDis;
		saveDis.y -= 0.0003f;
		savePos += saveDis;
		move.push_back(savePos);
		col.push_back(color[0]);
		line_cnt++;
	}

	glGenBuffers(1, &line_pos);
	glBindBuffer(GL_ARRAY_BUFFER, line_pos);
	glBufferData(GL_ARRAY_BUFFER, move.size() * sizeof(glm::vec3), move.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &line_color);
	glBindBuffer(GL_ARRAY_BUFFER, line_color);
	glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec3), col.data(), GL_STATIC_DRAW);
}

GLSlice::GLSlice(int i)
{ }

GLSlice::GLSlice(std::vector <glm::vec3> first, GLfloat dis_x)
{
	on = true;
	cut = true;
	firstpos = first;
	summit_cnt = firstpos.size();
	dis = glm::vec3{ dis_x, -0.003, 0.f};

	std::vector <glm::vec3> color;
	color.emplace_back(colorRd(gen), colorRd(gen), colorRd(gen));
	for (int i = 1; i < summit_cnt; ++i) {
		color.emplace_back(color[0]);
	}

	glm::vec3 sum{ 0.f, 0.f, 0.f };
	for (int i = 0; i < firstpos.size(); ++i) {
		sum += firstpos[i];
	}
	pos = sum / glm::vec3{ summit_cnt, summit_cnt, summit_cnt };

	glGenBuffers(1, &v_pos);
	glBindBuffer(GL_ARRAY_BUFFER, v_pos);
	glBufferData(GL_ARRAY_BUFFER, firstpos.size() * sizeof(glm::vec3), firstpos.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &v_color);
	glBindBuffer(GL_ARRAY_BUFFER, v_color);
	glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
}

GLSlice::~GLSlice()
{ 
}

void GLSlice::draw(std::string draw_Mod) {
	if ("solid" == draw_Mod) {
		glDrawArrays(GL_TRIANGLE_FAN, 0, summit_cnt);
	}
	else if ("line" == draw_Mod) {
		glDrawArrays(GL_LINE_LOOP, 0, summit_cnt);
	}

}

void GLSlice::draw_prepare(int Location, std::string Location_str) {
	if ("Pos" == Location_str) {
		glBindBuffer(GL_ARRAY_BUFFER, v_pos);
		glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else if ("Color" == Location_str) {
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
}

void GLSlice::Update() {
	for (int i = 0; i < firstpos.size(); ++i) {
		firstpos[i] += dis;
	}

	glBindBuffer(GL_ARRAY_BUFFER, v_pos);
	glBufferData(GL_ARRAY_BUFFER, firstpos.size() * sizeof(glm::vec3), firstpos.data(), GL_STATIC_DRAW);
}

void GLSlice::Update(GLfloat Speed, glm::vec3 basket[4]) {
	for (int i = 0; i < firstpos.size(); ++i) {
		firstpos[i] += dis * Speed;
	}
	pos += dis * Speed;

	if (pos.x + r <= -1.0f && dis.x < 0.f) 
		on = false;
	else if (pos.x - r >= 1.0f && dis.x > 0.f) 
		on = false;
	else if (pos.y <= -1.5f) 
		on = false;

	glm::vec3 min{ 1.f, 1.f, 1.f };
	for (int i = 0; i < firstpos.size(); ++i) {
		if (min.y > firstpos[i].y)
			min = firstpos[i];
	}

	if (basket[1].x < min.x && basket[0].x > min.x
		&& basket[1].y >= min.y && basket[2].y <= min.y
		&& dis.y <= 0.f ) {
		basket_in = true;
	}

	if (!cut) {
		Theta += 1.f;
		GLfloat radian = Theta * 0.0174533f;
		firstpos.clear();
		for (int i = 0; i < summit_cnt; ++i) {
			glm::vec3 m = { 0.0f, 0.0f, 1.0f };
			m.x = pos.x + r * cos(radian);
			m.y = pos.y + r * sin(radian);
			firstpos.emplace_back(m);

			Theta += 360.0f / summit_cnt;
			radian = Theta * 0.0174533f;
			if (Theta >= 360.0f) {
				Theta -= 360.0f;
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, v_pos);
	glBufferData(GL_ARRAY_BUFFER, firstpos.size() * sizeof(glm::vec3), firstpos.data(), GL_STATIC_DRAW);
}