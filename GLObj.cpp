#include "GLObj.h"

GLObj::GLObj(glm::vec3 m) : GLShapes(m) {

}

void GLObj::objLoad(std::ifstream& path) {
	char count[100];
	int vertexNum = 0;
	int faceNum = 0;
	int nomalNum = 0;

	// 끝날때까지로 수정하기
	while (path >> count) {
		if (count[0] == 'v' && count[1] == '\0')
			vertexNum += 1;
		else if (count[0] == 'v' && count[1] == 'n' && count[2] == '\0')
			nomalNum += 1;
		else if (count[0] == 'f' && count[1] == '\0')
			faceNum += 1;
		memset(count, '\0', sizeof(count));
	}
	face_cnt = faceNum;

	glm::vec4* vertex = new glm::vec4[vertexNum * 2];
	glm::vec4* face = new glm::vec4[faceNum * 2];
	glm::vec4* nomal = new glm::vec4[nomalNum * 2];
	char bind[128];
	int vertIndex = 0;
	int faceIndex = 0;
	int nomalIndex = 0;
	glm::vec3 max{}, min{}, sum{}, avg{};

	path.clear();
	path.seekg(0, std::ios::beg);
	while (path >> bind) {
		if (bind[0] == 'v' && bind[1] == '\0') {
			path >> vertex[vertIndex].x >> vertex[vertIndex].y >> vertex[vertIndex].z;			

			if (vertex[vertIndex].x > max.x)	max.x = vertex[vertIndex].x;
			if (vertex[vertIndex].y > max.y)	max.y = vertex[vertIndex].y;
			if (vertex[vertIndex].z > max.z)	max.z = vertex[vertIndex].z;

			if (vertex[vertIndex].x < min.x)	min.x = vertex[vertIndex].x;
			if (vertex[vertIndex].y < min.y)	min.y = vertex[vertIndex].y;
			if (vertex[vertIndex].z < min.z)	min.z = vertex[vertIndex].z;

			sum.x += vertex[vertIndex].x;
			sum.y += vertex[vertIndex].y;
			sum.z += vertex[vertIndex].z;

			vertIndex++;
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			for (int i = 0; i < 3; i++) {
				// "/" 문자를 뛰어넘기 위해 ignore 함수를 사용
				path >> face[faceIndex][i];
				path.ignore(std::numeric_limits<std::streamsize>::max(), '/');
				path.ignore(std::numeric_limits<std::streamsize>::max(), '/');
				path >> nomal[nomalIndex][i];
			}
			// std::cout << face[faceIndex].x << " "<< nomal[nomalIndex].x << " " << face[faceIndex].y << " "<< nomal[nomalIndex].y << " " << face[faceIndex].z << " " << nomal[nomalIndex].z << std::endl;
			faceIndex++;
			nomalIndex++;
		}
	}
	midpos.x = (sum.x / vertIndex);
	midpos.y = (sum.y / vertIndex);
	midpos.z = (sum.z / vertIndex);
	pos = glm::vec3{ 0.f, 0.f, 0.f };

	std::vector<glm::vec3> objpos;
	for (int i = 0; i < faceIndex; ++i) {
		glm::vec3 a[3]{
			{ vertex[static_cast<int>(face[i].x - 1)].x, vertex[static_cast<int>(face[i].x - 1)].y, vertex[static_cast<int>(face[i].x - 1)].z },
			{ vertex[static_cast<int>(face[i].y - 1)].x, vertex[static_cast<int>(face[i].y - 1)].y, vertex[static_cast<int>(face[i].y - 1)].z },
			{ vertex[static_cast<int>(face[i].z - 1)].x, vertex[static_cast<int>(face[i].z - 1)].y, vertex[static_cast<int>(face[i].z - 1)].z }
		};
		objpos.push_back(a[0]);
		objpos.push_back(a[1]);
		objpos.push_back(a[2]);
	}

	glGenBuffers(1, &v_pos);
	glBindBuffer(GL_ARRAY_BUFFER, v_pos);
	glBufferData(GL_ARRAY_BUFFER, objpos.size() * sizeof(glm::vec3), objpos.data(), GL_STATIC_DRAW);

	delete[] vertex;
	delete[] face;
	delete[] nomal;
}

void GLObj::draw(std::string draw_Mod) {
	if ("solid" == draw_Mod)
		glDrawArrays(GL_TRIANGLES, 0, face_cnt * 3);
	else if ("line" == draw_Mod) {
		for (int i = 0; i < face_cnt * 3; i += 3)
			glDrawArrays(GL_LINE_LOOP, i, 3);
	}
}

void GLObj::draw_prepare(int Location, std::string Location_str) {
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

void GLObj::Update()
{
	World_mat = glm::mat4(1.0);

	World_mat = glm::scale(World_mat, Oscale);

	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

	World_mat = glm::translate(World_mat, pos);

	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

	World_mat = glm::scale(World_mat, scale);

	World_mat = glm::translate(World_mat, -midpos);
}

void GLObj::Update20()
{
	World_mat = glm::mat4(1.0);

	World_mat = glm::translate(World_mat, pos - Object_mid);
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
	World_mat = glm::translate(World_mat, -(pos - Object_mid));

	World_mat = glm::translate(World_mat, pos);

	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

	World_mat = glm::scale(World_mat, scale);

	World_mat = glm::translate(World_mat, -midpos);
}

void GLObj::Update22()
{
	World_mat = glm::mat4(1.0);

	World_mat = glm::translate(World_mat, Ani_mid);
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
	World_mat = glm::translate(World_mat, -Ani_mid);

	World_mat = glm::translate(World_mat, pos);

	World_mat = glm::translate(World_mat, -Object_mid);
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
	World_mat = glm::rotate(World_mat, glm::radians(rotate_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
	World_mat = glm::translate(World_mat, Object_mid);

	World_mat = glm::scale(World_mat, scale);

	World_mat = glm::translate(World_mat, -midpos);
}