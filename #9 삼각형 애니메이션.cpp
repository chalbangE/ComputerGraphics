#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>#include <glm.hpp>
#include <ext.hpp>
#include <gtc/matrix_transform.hpp>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<GLclampf> colorRd(0.0f, 1.0f);
std::uniform_real_distribution<GLfloat> make(0.05f, 0.2f);
std::uniform_real_distribution<GLfloat> dxdy(-0.015f, 0.015f);
std::uniform_real_distribution<GLfloat> HF(0.001f, 0.005f);
std::uniform_int_distribution<int> ThetaPlus(3, 10);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

#define pie 3.141592653589793238

void make_shaderProgram();void make_vertexShaders();void make_fragmentShaders();void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);GLvoid drawScene();GLvoid Reshape(int w, int h);void Clear();void InitBuffer();char* filetobuf(const char*);
struct StructVBO
{
	GLuint pos, color;
}; 
struct Points
{
	glm::vec3 pos[3];
	glm::vec3 mid;
	glm::vec3 dis{ dxdy(gen),0.0f, 0.0f};
	glm::vec3 color[3]{ {colorRd(gen), colorRd(gen), colorRd(gen)}, {colorRd(gen), colorRd(gen), colorRd(gen)}, {colorRd(gen), colorRd(gen), colorRd(gen)} };
	GLfloat theta = 0.0f, k, half = 0.005;
	int thetaPlus = ThetaPlus(gen);
	Points() {}
	Points(glm::vec3 mxmy, GLfloat k) : mid(mxmy), k(k) {
		pos[0] = { mxmy.x, mxmy.y + k, 1.0f };
		pos[1] = { mxmy.x - (k / 3 * 2), mxmy.y - k, 1.0f };
		pos[2] = { mxmy.x + (k / 3 * 2), mxmy.y - k, 1.0f };

		if (dis.x >= 0.0f)
			dis.x *= -1;
		dis.y = dis.x;
	}
};


GLuint vao;
float winSizex = 800, winSizey = 800;
std::vector <StructVBO> Vbo;
std::vector <Points> Tri;


bool On[5]{};
glm::vec3 DxDy[4];
glm::vec3 Rectsp[12];
bool goX[4] = { true,true,true,true };
int Gorectsp[4] = { 1, 1, 1, 1 };


int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#9");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Clear();
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(10, TimerFunction, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	// 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// Location 번호 저장
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	int Te = glGetUniformLocation(shaderProgramID, "u_Te");
	int xy = glGetUniformLocation(shaderProgramID, "xy");
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation); // Vertex Position 정보 영역 시작 (Enable)

	for (int i = 0; i < Vbo.size(); ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].color); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		if (On[3]) {
			GLfloat theta = 0.0f;
			glUniform1f(Te, theta);
		}
		else {
			glUniform1f(Te, Tri[i].theta);
		}
		glUniform2f(xy, Tri[i].mid.x, Tri[i].mid.y);
		//std::cout << Tri[i].mid.x << " " << Tri[i].pos[0].x << std::endl;

		glDrawArrays(GL_TRIANGLES, 0, 3);
		//std::cout << i << std::endl;
	}

	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);

	glutSwapBuffers(); //--- 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5': {
		for (int i = 0; i < 4; ++i) {
			On[i] = false;
		}
		On[key - '1'] = true;

		for (int i = 0; i < 4; ++i) {
			DxDy[i].x = dxdy(gen);
			DxDy[i].y = dxdy(gen);
			DxDy[i].z = 0.0f;
		}

		if (key == '2') {
			for (int i = 0; i < 4; ++i) {
				DxDy[i].y = DxDy[i].x;
				DxDy[i].z = 0.0f;
			}
		}
		else if (key == '3') {
			for (int i = 0; i < Tri.size(); ++i) {
				Tri[i].mid.x = 0.75f;
				Tri[i].mid.y = 0.7f;

				Tri[i].pos[0] = { Tri[i].mid.x, Tri[i].mid.y + Tri[i].k, 1.0f };
				Tri[i].pos[1] = { Tri[i].mid.x - (Tri[i].k / 3 * 2), Tri[i].mid.y - Tri[i].k, 1.0f };
				Tri[i].pos[2] = { Tri[i].mid.x + (Tri[i].k / 3 * 2), Tri[i].mid.y - Tri[i].k, 1.0f };

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Tri[i].pos), &Tri[i].pos, GL_DYNAMIC_DRAW);

				if (DxDy[i].x > 0.0f) {
					DxDy[i].x *= -1;
				}
				DxDy[i].y = DxDy[i].x;
			}
		}
		else if (key == '4') {
			for (int i = 0; i < Tri.size(); ++i) {
				Tri[i].mid.x = 0.0f;
				Tri[i].mid.y = 0.0f;

				Tri[i].pos[0] = { Tri[i].mid.x, Tri[i].mid.y + Tri[i].k, 1.0f };
				Tri[i].pos[1] = { Tri[i].mid.x - (Tri[i].k / 3 * 2), Tri[i].mid.y - Tri[i].k, 1.0f };
				Tri[i].pos[2] = { Tri[i].mid.x + (Tri[i].k / 3 * 2), Tri[i].mid.y - Tri[i].k, 1.0f };

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Tri[i].pos), &Tri[i].pos, GL_DYNAMIC_DRAW);
			}
		}
		// std::cout << On[0] << std::endl;
		break;
	}
	case 'C':
	case 'c': {
		Vbo.clear();
		for (int i = 0; i < 4; ++i) {
			On[i] = false;
		}
		break;
	}
	}
	glutPostRedisplay(); // 화면 재 출력
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		if (On[0]) {
			for (int i = 0; i < Vbo.size(); ++i) {
				bool turnx = false;
				bool turny = false;

				for (int k = 0; k < 3; ++k) {
					Tri[i].pos[k] += DxDy[i];
					// std::cout << Tri[i].pos[k].x << std::endl;


					if (Tri[i].pos[k].x <= -1.0f || Tri[i].pos[k].x >= 1.0f)
						turnx = true;
					else if (Tri[i].pos[k].y <= -1.0f || Tri[i].pos[k].y >= 1.0f)
						turny = true;
				}

				Tri[i].mid += DxDy[i];

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Tri[i].pos), Tri[i].pos, GL_DYNAMIC_DRAW);

				if (turnx) {
					DxDy[i].x *= -1;
					Tri[i].theta += (90.0f * 0.0174533);
				}
				else if (turny) {
					DxDy[i].y *= -1;
					Tri[i].theta += (90.0f * 0.0174533);
				}
			}
		}
		else if (On[1]) {
			for (int i = 0; i < Tri.size(); ++i) {
				bool turnx = false;
				bool turny = false;
				bool realyturn = false;

				for (int k = 0; k < 3; ++k) {
					if (goX[i]) {
						Tri[i].pos[k].x += DxDy[i].x;
						Tri[i].mid.x += DxDy[i].x / 3;
						if ((Tri[i].pos[k].x <= -1.0f || Tri[i].pos[k].x >= 1.0f) && goX[i])
							turnx = true;
					}
					else {
						Tri[i].pos[k].y += DxDy[i].y;
						DxDy[i].z += DxDy[i].y / 3;
						Tri[i].mid.y += DxDy[i].y / 3; 
						if (Tri[i].pos[k].y - 0.02f <= -1.0f || Tri[i].pos[k].y + 0.02f >= 1.0f || DxDy[i].z >= 0.25f || DxDy[i].z <= -0.25f) {
							turny = true;
							if (Tri[i].pos[k].y - 0.02f <= -1.0f || Tri[i].pos[k].y + 0.02f >= 1.0f) {
								realyturn = true;
							}
						}
					}
					// std::cout << i << " : " << DxDy[i].y << std::endl;
				}

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Tri[i].pos), Tri[i].pos, GL_DYNAMIC_DRAW);

				if (turnx) {
					DxDy[i].x *= -1;
					Tri[i].theta += (90.0f * 0.0174533);
					goX[i] = false;
					// std::cout << i << " : " << Tri[i].theta << std::endl;
				}
				else if (turny) {
					if (realyturn) {
						DxDy[i].y *= -1;
						std::cout << "s" << std::endl;
					}
					DxDy[i].z = 0.0f;
					goX[i] = true;
				}
			}
		}
		else if (On[2]) {
			for (int i = 0; i < Tri.size(); ++i) {
				for (int k = 0; k < 3; ++k) {
					if (Gorectsp[i] % 2 == 1) {
						Tri[i].pos[k].x += Tri[i].dis.x;
						Tri[i].mid.x += Tri[i].dis.x / 3;
					}
					else {
						Tri[i].pos[k].y += Tri[i].dis.y;
						Tri[i].mid.y += Tri[i].dis.y / 3;
					}
				}

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Tri[i].pos), Tri[i].pos, GL_DYNAMIC_DRAW);

				if (Tri[i].mid.x - Tri[i].k <= Rectsp[Gorectsp[i]].x && Tri[i].mid.x + Tri[i].k >= Rectsp[Gorectsp[i]].x 
					&& Tri[i].mid.y - Tri[i].k <= Rectsp[Gorectsp[i]].y && Tri[i].mid.y + Tri[i].k >= Rectsp[Gorectsp[i]].y) {
					Gorectsp[i]++;
					if (Gorectsp[i] % 2 == 0) {
						Tri[i].dis.x *= -1;
						Tri[i].theta += (90.0f * 0.0174533);
						goX[i] = false;
					}
					else if (Gorectsp[i] % 2 == 1) {
						Tri[i].dis.y *= -1;
						Tri[i].dis.z = 0.0f;
						goX[i] = true;
					}

					if (Gorectsp[i] == 12) {
						Tri[i].mid = {0.75f, 0.7f, 1.0f};
						Tri[i].pos[0] = { Tri[i].mid.x, Tri[i].mid.y + Tri[i].k, 1.0f };
						Tri[i].pos[1] = { Tri[i].mid.x - (Tri[i].k / 3 * 2), Tri[i].mid.y - Tri[i].k, 1.0f };
						Tri[i].pos[2] = { Tri[i].mid.x + (Tri[i].k / 3 * 2), Tri[i].mid.y - Tri[i].k, 1.0f };
						Gorectsp[i] = 0;
					}
				}
			}
		}
		else if (On[3]) {
			for (int i = 0; i < Tri.size(); ++i) {
				float radian = Tri[i].theta * 0.0174533;
				glm::vec3 middlePoint = { 0.0f, 0.0f, 1.0f };

				Tri[i].mid.x = middlePoint.x + Tri[i].half * cos(radian);
				Tri[i].mid.y = middlePoint.y + Tri[i].half * sin(radian);
				Tri[i].pos[0] = { Tri[i].mid.x, Tri[i].mid.y + Tri[i].k, 1.0f };
				Tri[i].pos[1] = { Tri[i].mid.x - (Tri[i].k / 3 * 2), Tri[i].mid.y - Tri[i].k, 1.0f };
				Tri[i].pos[2] = { Tri[i].mid.x + (Tri[i].k / 3 * 2), Tri[i].mid.y - Tri[i].k, 1.0f };

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Tri[i].pos), Tri[i].pos, GL_DYNAMIC_DRAW);

				Tri[i].theta += Tri[i].thetaPlus;
				Tri[i].half += 0.002;
				if (Tri[i].theta >= 360)
					Tri[i].theta = 0;

				if (Tri[i].half >= 1.0f) {
					Tri[i].half = 0.01;
					Tri[i].theta = 0;

				}
			}
		}
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
	glutTimerFunc(10, TimerFunction, 1);
}

void Mouse(int button, int state, int x, int y)
{
	glm::vec3 mxmy = { 0.0f, 0.0f, 1.0f };

	mxmy.x = (x - (winSizex / 2)) / (winSizex / 2);
	mxmy.y = -(y - (winSizey / 2)) / (winSizey / 2);

	if (state == GLUT_DOWN) {
		// 좌클릭 했을 때
		if (button == GLUT_LEFT_BUTTON) {
			if (Tri.size() == 4) {
				Tri.erase(Tri.begin());
				Vbo.erase(Vbo.begin());

				for (int i = 0; i < 3; ++i) {
					DxDy[i] = DxDy[i + 1];
					goX[i] = goX[i + 1];
					Gorectsp[i] = Gorectsp[i + 1];
				}
			}

			goX[3] = true;
			DxDy[3] = { dxdy(gen), DxDy[3].x, 0.0f};
			Gorectsp[3] = { 1 };
			GLfloat k = make(gen);
			
			if (On[2]) {
				mxmy = { 0.75f, 0.7f, 1.0f };
				Tri.emplace_back(mxmy, k);
			}
			else if (On[3]) {
				mxmy = { 0.0f, 0.0f, 1.0f };
				Tri.emplace_back(mxmy, k);				
			}
			else {
				Tri.emplace_back(mxmy, k);
			}
			Vbo.emplace_back();

			glGenBuffers(1, &Vbo.back().pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo.back().pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Tri.back().pos), &Tri.back().pos, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &Vbo.back().color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo.back().color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Tri.back().color), &Tri.back().color, GL_DYNAMIC_DRAW);
		}
	}
}

void Clear()
{
	Tri.clear();
	Vbo.clear();
	Rectsp[0] = { 0.75f, 0.7f, 1.0f };
	Rectsp[1] = { -0.75f, 0.7f, 1.0f };
	Rectsp[2] = { -0.75f, -0.7f, 1.0f };
	Rectsp[3] = { 0.75f, -0.7f, 1.0f };
	Rectsp[4] = { 0.75f, 0.5f, 1.0f };
	Rectsp[5] = { -0.6f, 0.5f, 1.0f };
	Rectsp[6] = { -0.6f, -0.5f, 1.0f };
	Rectsp[7] = { 0.6f, -0.5f, 1.0f };
	Rectsp[8] = { 0.6f, 0.3f, 1.0f };
	Rectsp[9] = { -0.4f, 0.3f, 1.0f };
	Rectsp[10] = { -0.4f, -0.3f, 1.0f };
	Rectsp[11] = { 0.4f, -0.3f, 1.0f };

}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}
void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기
}

void make_shaderProgram()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
	vertexSource = filetobuf("#9_vertex.glsl");
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}