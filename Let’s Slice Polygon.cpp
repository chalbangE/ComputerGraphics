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

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

#define pie 3.141592653589793238

void make_shaderProgram();void make_vertexShaders();void make_fragmentShaders();void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);GLvoid drawScene();GLvoid Reshape(int w, int h);void Init();void InitBuffer();char* filetobuf(const char*);void Motion(int x, int y);// void SortPoints(std::vector <glm::vec3>& move, std::vector <glm::vec3>& color, std::vector < Points > Pos);

GLuint vao;
float winSizex = 800, winSizey = 800;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#12");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	Init();
	make_shaderProgram();
	InitBuffer();
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

	//

	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);

	glutSwapBuffers(); //--- 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	}
	glutPostRedisplay(); // 화면 재 출력
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
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

	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {

		}
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON) {

		}
	}
}void Motion(int x, int y)
{
	glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };
	

	glutPostRedisplay(); // 화면 재 출력
}

void Init()
{ }

//void SortPoints(std::vector<glm::vec3>& move, std::vector<glm::vec3>& color, std::vector<Points> Pos)
//{
//	// 점을 순서에 맞게 배치
//	if (Pos.begin()->posCnt >= 3) {
//		for (int k = 1; k < Pos.size() - 1; ++k) {
//			move.push_back(Pos.begin()->move);
//			color.push_back(Pos.begin()->color);
//
//			move.push_back(Pos[k].move);
//			color.push_back(Pos[k].color);
//
//			move.push_back(Pos[k + 1].move);
//			color.push_back(Pos[k + 1].color);
//		}
//	}
//	else if (Pos.begin()->posCnt == 2) {
//		move.push_back(Pos.begin()->move);
//		color.push_back(Pos.begin()->color);
//
//		move.push_back(Pos.back().move);
//		color.push_back(Pos.back().color);
//	}
//	else {
//		move.push_back(Pos.begin()->move);
//		color.push_back(Pos.begin()->color);
//		// std::cout << move.back().x << "\t" << move.back().y << std::endl;
//	}
//}

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