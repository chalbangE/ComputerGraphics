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
			// std::cout << vertex[vertIndex].x << " " << vertex[vertIndex].y << " " << vertex[vertIndex].z << std::endl;


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

			//path >> face[faceIndex].x >> nomal[nomalIndex].x >> 
			//	face[faceIndex].y >> nomal[nomalIndex].y >> 
			//	face[faceIndex].z >> nomal[nomalIndex].z;
		}
	}
	//pos.x = -(sum.x / vertexNum);
	//pos.y = -(sum.y / vertexNum);
	//pos.z = -(sum.z / vertexNum);
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

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
	// char count[128]; -임시 문자열 배열로, 파일에서 읽은 데이터를 저장합니다.
	//	int vertexnum = 0; -OBJ 파일에서 발견된 정점(버텍스)의 수를 추적하기 위한 변수입니다.
	//	int facenum = 0; -OBJ 파일에서 발견된 면의 수를 추적하기 위한 변수입니다.
	//	int uvnum = 0; -OBJ 파일에서 발견된 텍스처 좌표(UV)의 수를 추적하기 위한 변수입니다.
	//	int vertIndex = 0; -정점 데이터를 읽을 때 현재 정점의 인덱스를 추적하기 위한 변수입니다.
	//	int faceIndex = 0; -면 데이터를 읽을 때 현재 면의 인덱스를 추적하기 위한 변수입니다.
	//	int uvIndex = 0; -텍스처 좌표 데이터를 읽을 때 현재 텍스처 좌표의 인덱스를 추적하기 위한 변수입니다.
	//	glm::vec3 * vertex - 정점(버텍스) 좌표를 저장하는 동적 배열입니다.
	//	glm::vec3 * face - 면을 나타내는 데이터를 저장하는 동적 배열입니다.
	//	glm::vec3 * uvdata - 텍스처 좌표(UV) 데이터를 저장하는 동적 배열입니다.이 배열은 면의 텍스처 좌표를 저장합니다.
	//	glm::vec2 * uv - 텍스처 좌표(UV) 값을 저장하는 동적 배열입니다.이 배열은 uvnum의 크기에 따라 동적으로 할당됩니다.
	//	char bind[128]; -임시 문자열 배열로, 파일에서 읽은 데이터를 임시로 저장하는 데 사용됩니다.
	/*
	char count[128];
	int vertexnum = 0; // 점 개수
	int facenum = 0; // 면 개수
	int uvnum = 0; // 이놈 머임?

	//--- 1. 전체 버텍스 개수 및 삼각형 개수 세기
	while (!feof(path)) {
		fscanf(path, "%s", count);
		if (count[0] == 'v' && count[1] == '\0')
			vertexnum++;
		else if (count[0] == 'f' && count[1] == '\0')
			facenum++;
		else if (count[0] == 'v' && count[1] == 't' && count[3] == '\0')
			uvnum++;
		memset(count, '\0', sizeof(count));
	}
	rewind(path);

	int vertIndex = 0;
	int faceIndex = 0;
	int uvIndex = 0;

	//--- 2. 메모리 할당
	glm::vec3* vertex = new glm::vec3[vertexnum];
	glm::vec3* face = new glm::vec3[facenum];
	glm::vec3* uvdata = new glm::vec3[facenum];
	glm::vec2* uv = new glm::vec2[uvnum];
	char bind[128];

	while (!feof(path)) {
		fscanf(path, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(path, "%f %f %f\n", &vertex[vertIndex].x, &vertex[vertIndex].y, &vertex[vertIndex].z);
			vertIndex++;
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			unsigned int temp_face[3], temp_uv[3], temp_normal[3];
			fscanf(path, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&temp_face[0], &temp_uv[0], &temp_normal[0],
				&temp_face[1], &temp_uv[1], &temp_normal[1],
				&temp_face[2], &temp_uv[2], &temp_normal[2]);

			face[faceIndex].x = temp_face[0];
			face[faceIndex].y = temp_face[1];
			face[faceIndex].z = temp_face[2];
			uvdata[faceIndex].x = temp_uv[0];
			uvdata[faceIndex].y = temp_uv[1];
			uvdata[faceIndex].z = temp_uv[2];
			faceIndex++;
		}
		else if (bind[0] == 'v' && bind[1] == 't' && bind[2] == '\0') {
			fscanf(path, "%f %f\n", &uv[uvIndex].x, &uv[uvIndex].y);
			uvIndex++;
		}
	}*/
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
	//else if ("Tex" == str) {
	//	glBindBuffer(GL_ARRAY_BUFFER, m_TexVBO);
	//	glVertexAttribPointer(Loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//}
}
