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
std::uniform_real_distribution<GLfloat> wh(0.02f, 0.1f);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

void make_shaderProgram();void make_vertexShaders();void make_fragmentShaders();void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);GLvoid drawScene();GLvoid Reshape(int w, int h);void Clear();void InitBuffer();char* filetobuf(const char*);
struct StructVBO
{
	GLuint pos, color;
	int mod = 0;
};

GLuint vao;
float winSizex = 800, winSizey = 800;
std::vector <StructVBO> Vbo;

int mod = 0;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#8");
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
	glutMainLoop();
}

GLvoid drawScene()
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);
	//--- 사용할 VAO 불러오기
	glBindVertexArray(vao);

	// Location 번호 저장
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1

	//std::cout << Vbo.size() << std::endl;
	for (int i = 5; i >= 0; --i) {
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(ColorLocation); // Vertex Position 정보 영역 시작 (Enable)
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].color); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		std::cout << i << std::endl;

		if (i < 4) {
			if (Vbo[i].mod == 0)
				glDrawArrays(GL_TRIANGLES, 0, 3);
			else
				glDrawArrays(GL_LINE_LOOP, 0, 3);
		}
		else	
			glDrawArrays(GL_LINE_LOOP, 0, 2);
	}

	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);

	glutSwapBuffers(); //--- 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'A':
	case 'a': {
		mod = 0;
		break;
	}
	case 'B':
	case 'b': {
		mod = 1;
		break;
	}
	case 'C':
	case 'c': {
		Vbo.clear();
		break;
	}
	}
	glutPostRedisplay(); // 화면 재 출력
}

void Mouse(int button, int state, int x, int y)
{
	GLfloat mx = 0.0f, my = 0.0f;
	StructVBO temp;
	int where = 0;

	mx = (x - (winSizex / 2)) / (winSizex / 2);
	my = -(y - (winSizey / 2)) / (winSizey / 2);

	if (state == GLUT_DOWN) {
		// 좌클릭 했을 때
		if (button == GLUT_LEFT_BUTTON) {
			if (mx > 0.0f && my > 0.0f)
				where = 0;
			else if (mx < 0.0f && my > 0.0f)
				where = 1;
			else if (mx < 0.0f && my < 0.0f)
				where = 2;
			else if (mx > 0.0f && my < 0.0f)
				where = 3;
			std::cout << where << std::endl;

			GLfloat k = make(gen);
			GLfloat pos[3][3] = { {mx, my + k, 1.0f}, {mx - (k / 3 * 2), my - k, 1.0f}, {mx + (k / 3 * 2), my - k, 1.0f} };
			GLfloat color[3][3] = { {colorRd(gen), colorRd(gen), colorRd(gen)},{colorRd(gen), colorRd(gen), colorRd(gen)},{colorRd(gen), colorRd(gen), colorRd(gen)} };
			temp.mod = mod;

			glGenBuffers(1, &temp.pos);
			glBindBuffer(GL_ARRAY_BUFFER, temp.pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
			glGenBuffers(1, &temp.color);
			glBindBuffer(GL_ARRAY_BUFFER, temp.color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

			Vbo[where] = temp;
		}
	}
}

void Clear()
{
	std::cout << "클리엉" << std::endl;
	Vbo.clear();
	for (int i = 0; i < 6; ++i) {
		Vbo.emplace_back();
	}
	GLfloat pos[2][3] = { {0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 1.0f} };
	GLfloat color[2][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	glGenBuffers(1, &Vbo[4].pos);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[4].pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	glGenBuffers(1, &Vbo[4].color);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[4].color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	GLfloat pos1 [2][3] = { {-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f} };
	GLfloat color1 [2][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	glGenBuffers(1, &Vbo[5].pos);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[5].pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos1), pos1, GL_STATIC_DRAW);
	glGenBuffers(1, &Vbo[5].color);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[5].color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color1), color1, GL_STATIC_DRAW);
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