#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <math.h>

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

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

#define pie 3.141592653589793238

struct StructVBO
{
	GLuint pos, color;
};
struct Points
{
	glm::vec3 move;
	glm::vec3 color{ colorRd(gen), colorRd(gen), colorRd(gen) };
	Points() {}
	Points(glm::vec3 m) : move(m) { }
};

void make_shaderProgram();void make_vertexShaders();void make_fragmentShaders();void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);GLvoid drawScene();GLvoid Reshape(int w, int h);void Clear();void InitBuffer();char* filetobuf(const char*);void Motion(int x, int y);bool PointsInTrue(Points point);
GLuint vao;
std::vector <StructVBO> Vbo;
float winSizex = 800, winSizey = 800;
std::vector <Points> rect;
int seclect = 999;
Points mouse;
int HowmanyPos = 4;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#13");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Clear();
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(10, TimerFunction, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	// 배경색 설정
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// Location 번호 저장
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation); // Vertex Position 정보 영역 시작 (Enable)

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < Vbo.size(); ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].color); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

		if (i == 0)
			glDrawArrays(GL_LINE_LOOP, 0, HowmanyPos);
		else 
			glDrawArrays(GL_LINES, 0, 2);
	}

	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);

	glutSwapBuffers(); //--- 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'C':
	case 'c': {
		Clear();
		break;
	}
	case '+':
	case '=': {
		HowmanyPos++;
		Clear();
		break;
	}
	case '_':
	case '-': {
		HowmanyPos--;
		if (HowmanyPos < 3) {
			HowmanyPos++;
		}
		Clear();
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
		glm::vec3 move[999], color[999];
		for (int i = 0; i < rect.size(); ++i) {
			move[i] = rect[i].move;
			color[i] = rect[i].color;
		}

		glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
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
	GLfloat n = 0.015f;

	glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };
	mouse = Points(m);

	if (state == GLUT_DOWN) {
		// 좌클릭 했을 때
		if (button == GLUT_LEFT_BUTTON) {
			for (int i = 0; i < rect.size(); ++i) {
				if (mouse.move.x >= rect[i].move.x - n && mouse.move.x <= rect[i].move.x + n && mouse.move.y >= rect[i].move.y - n && mouse.move.y <= rect[i].move.y + n) {
					seclect = i;
					break;
				}
			}
		}
		// 우클릭 했을 때
		else if (button == GLUT_RIGHT_BUTTON) {
		}
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON) {
			seclect = 999;
		}
	}
}void Motion(int x, int y)
{
	glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };
	Points mm = Points(m);
	if (seclect < rect.size()) {
		GLfloat mx = 0.0f, my = 0.0f;

		mx = (x - (winSizex / 2)) / (winSizex / 2);
		my = -(y - (winSizey / 2)) / (winSizey / 2);

		rect[seclect].move.x = mx;
		rect[seclect].move.y = my;

		glm::vec3 move[999], color[999];
		for (int i = 0; i < rect.size(); ++i) {
			move[i] = rect[i].move;
			color[i] = rect[i].color;
		}

		glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	}
	else if (PointsInTrue(mouse)) {
		glm::vec3 dxdy = mouse.move - mm.move;

		for (int i = 0; i < rect.size(); ++i) {
			rect[i].move -= dxdy;
		}

		mouse = mm;
	}
	glutPostRedisplay(); // 화면 재 출력
}

void Clear()
{
	for (int i = 0; i < Vbo.size(); ++i) {
		glDeleteBuffers(1, &Vbo[i].pos);
		glDeleteBuffers(1, &Vbo[i].color);
	}
	Vbo.clear();
	rect.clear();
	glm::vec3 MM = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 3; ++i) {
		Vbo.emplace_back();
	}
	
	GLfloat r = 0.7f, Theta = 90.0f;
	GLfloat radian = Theta * 0.0174533f;

	for (int i = 0; i < HowmanyPos; ++i) {
		Theta += 360.0f / HowmanyPos; 
		radian = Theta * 0.0174533f;
		if (Theta >= 360.0f) {
			Theta -= 360.0f;
		}
		glm::vec3 m = { 0.0f, 0.0f, 0.0f };
		m.x = MM.x + r * cos(radian);
		m.y = MM.y + r * sin(radian);
		rect.emplace_back(Points(m));
	}

	glm::vec3 move[999], color[999];
	for (int i = 0; i < rect.size(); ++i) {
		move[i] = rect[i].move;
		color[i] = rect[i].color;
	}

	glGenBuffers(1, &Vbo[0].pos);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_STATIC_DRAW);
	glGenBuffers(1, &Vbo[0].color);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	// x축 y축 선긋기
	{
		GLfloat pos[2][3] = { {0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 1.0f} };
		GLfloat color[2][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

		glGenBuffers(1, &Vbo[1].pos);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
		glGenBuffers(1, &Vbo[1].color);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

		GLfloat pos1[2][3] = { {-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f} };
		GLfloat color1[2][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

		glGenBuffers(1, &Vbo[2].pos);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pos1), pos1, GL_STATIC_DRAW);
		glGenBuffers(1, &Vbo[2].color);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color1), color1, GL_STATIC_DRAW);
	}
}

bool PointsInTrue(Points point) {	int cnt = 0;
	for (int i = 0; i < rect.size(); ++i) {

		if (i + 1 < rect.size()) {
			glm::vec3 pos1, pos2;

			pos1 = rect[i].move;
			pos2 = rect[i + 1].move;
			if (pos1.y < pos2.y) {
				glm::vec3 temp;
				temp = pos1;
				pos1 = pos2;
				pos2 = temp;
			}

			if (pos1.x == pos2.x /*두 점의 직선의 방정식이 세로로 곧을 때*/ ) {
				if (point.move.y <= pos1.y && point.move.y >= pos2.y
					&& point.move.x <= pos1.x) {
					cnt++;
				}
			}
			else {
				GLfloat m = ((pos2.y - pos1.y) / (pos2.x - pos1.x));

				if (point.move.x <= (point.move.y - pos1.y + (m * pos1.x)) / m
					&& point.move.y <= pos1.y && point.move.y >= pos2.y) {
					cnt++;
				}
			}
		}
		else {
			glm::vec3 pos1, pos2;

			pos1 = rect[i].move;
			pos2 = rect.begin()->move;
			if (pos1.y < pos2.y) {
				glm::vec3 temp;
				temp = pos1;
				pos1 = pos2;
				pos2 = temp;
			}

			if (pos1.x == pos2.x /*두 점의 직선의 방정식이 세로로 곧을 때*/) {
				if (point.move.y <= pos1.y && point.move.y >= pos2.y
					&& point.move.x <= pos1.x) {
					cnt++;
				}
			}
			else {
				GLfloat m = ((pos2.y - pos1.y) / (pos2.x - pos1.x));

				if (point.move.x <= (point.move.y - pos1.y + (m * pos1.x)) / m
					&& point.move.y <= pos1.y && point.move.y >= pos2.y) {
					cnt++;
				}
			}
		}
	}
	if (cnt % 2 == 1 /*마우스 포인터가 도형 안에 위치하면*/)
		return true;	else		return false;}

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
	vertexSource = filetobuf("vertex.glsl");
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