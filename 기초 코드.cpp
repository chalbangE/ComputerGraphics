
#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 700
#define HEIGHT 700
#define PI 3.14

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>

struct VBO {
	GLuint pos;
	GLuint color;
};
struct Vertex {
	GLfloat coord[3];
	GLfloat color[3];
};
GLuint shaderProgramID; //--- 쉐이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 쉐이더 객체
GLuint fragmentShader; //--- 프래그먼트 쉐이더 객체
std::vector <VBO> Vbo;
std::vector <Vertex> vertex;

float deg2rad(int degree) {
	return (float)degree * PI / 180;
}
void convertDeviceXY2OpenglXY(int x, int y, float* ox, float* oy) {
	*ox = (float)((x - (float)WIDTH / 2.0) * (float)(1.0 / (float)(WIDTH / 2.0)));
	*oy = -(float)((y - (float)HEIGHT / 2.0) * (float)(1.0 / (float)(HEIGHT / 2.0)));
}
void PutCoord(GLfloat* vertex, GLfloat x, GLfloat y) {
	*vertex = x / WIDTH;
	*(vertex + 1) = y / HEIGHT;
	*(vertex + 2) = 0.0;
}
void make_vertexShaders() {//--- 버텍스 쉐이더 객체 만들기
	GLchar* vertexSource;
	GLint result;
	GLchar errorLog[512];

	vertexSource = filetobuf("vertex.glsl");

	vertexShader = glCreateShader(GL_VERTEX_SHADER); //--- 버텍스 세이더 객체 만들기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0); //--- 세이더 코드를 세이더 객체에 넣기
	glCompileShader(vertexShader); //--- 버텍스 세이더 컴파일하기
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result); //--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders() {//--- 프래그먼트 쉐이더 객체 만들기
	GLchar* fragmentSource;
	GLint result;
	GLchar errorLog[512];

	fragmentSource = filetobuf("fragment.glsl");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //--- 프래그먼트 세이더 객체 만들기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0); //--- 세이더 코드를 세이더 객체에 넣기
	glCompileShader(fragmentShader); //--- 프래그먼트 세이더 컴파일
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result); //--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_shaderProgram() {//--- 쉐이더 프로그램 만들기
	GLint result;
	GLchar errorLog[512];

	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기

	shaderProgramID = glCreateProgram(); //--- 쉐이더 프로그램 만들기
	glAttachShader(shaderProgramID, vertexShader); //--- 쉐이더 프로그램에 버텍스 쉐이더 붙이기
	glAttachShader(shaderProgramID, fragmentShader); //--- 쉐이더 프로그램에 프래그먼트 쉐이더 붙이기
	glLinkProgram(shaderProgramID); //--- 쉐이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 쉐이더 객체를 쉐이더 프로그램에 링크했음으로, 쉐이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result); //--- 쉐이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
	}

	glUseProgram(shaderProgramID); //--- 만들어진 쉐이더 프로그램 사용하기
}
void InitBuffer() {//--- 버퍼 생성하고 데이터 받아오기
	GLint current;

	object.resize(object.size() + 1);
	glGenVertexArrays(1, &object[object.size() - 1].vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(object[object.size() - 1].vao); //--- VAO를 바인드하기

	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
	if (!current) {
		std::cout << "ERROR: VAO 바인딩 오류!" << std::endl;
		return;
	}
	glGenBuffers(2, object[object.size() - 1].vbo); //--- 2개의 VBO를 지정하고 할당하기

	//--- 위치 속성
	glBindBuffer(GL_ARRAY_BUFFER, object[object.size() - 1].vbo[0]);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current);
	if (!current) {
		std::cout << "ERROR: 위치 VBO 바인딩 오류!" << std::endl;
		return;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex[vertex.size() - 1].coord), vertex[vertex.size() - 1].coord, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//--- 색상 속성
	glBindBuffer(GL_ARRAY_BUFFER, object[object.size() - 1].vbo[1]);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current);
	if (!current) {
		std::cout << "ERROR: 색상 VBO 바인딩 오류!" << std::endl;
		return;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex[vertex.size() - 1].color), vertex[vertex.size() - 1].color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}
char* filetobuf(const char* file) {//--- 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
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
void ClearVAOnVBO() {
	for (int i = 0; i < object.size(); i++) {
		glDeleteVertexArrays(1, &object[i].vao);
		glDeleteBuffers(2, object[i].vbo);
	}
}
void UpdateVBO() {
	for (int i = 0; i < vertex.size(); i++) {
		glBindBuffer(GL_ARRAY_BUFFER, object[i].vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex[i].coord), vertex[i].coord);
		glBindBuffer(GL_ARRAY_BUFFER, object[i].vbo[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex[i].color), vertex[i].color);
	}
}

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand((int)time(NULL));
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(WIDTH, HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Computer Graphics"); // 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "ERROR: Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	make_shaderProgram();

	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutMouseFunc(Mouse); // 마우스 입력 콜백함수
	glutMotionFunc(Motion); // 마우스 눌린 상태에서의 이동 콜백함수
	glutTimerFunc(10, TimerFunction, 1); // 타이머함수 재 설정
	glutMainLoop();
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID); //--- 렌더링 파이프라인에 세이더 불러오기

	for (int i = 0; i < object.size(); i++) {
		glBindVertexArray(object[i].vao); //--- 사용할 VAO 불러오기
		glDrawArrays(GL_POINTS, 0, 1); //--- 점 그리기
	}

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q': {
		ClearVAOnVBO();
		glutLeaveMainLoop();
		break;
	}
	}
	glutPostRedisplay(); //--- 출력 콜백 함수 호출
}
void Mouse(int button, int state, int x, int y)
{
	//Vertex temp;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// 마우스 좌표 GL 좌표계로 변환
		float fx, fy;
		convertDeviceXY2OpenglXY(x, y, &fx, &fy);

		//temp = { {fx, fy, 0.0}, {0.0, 0.0, 0.0} };
		//vertex.emplace_back(temp);

		// 버텍스 추가
		//InitBuffer();
	}
	glutPostRedisplay(); //--- 출력 콜백 함수 호출
}
void Motion(int x, int y)
{

	glutPostRedisplay(); //--- 출력 콜백 함수 호출
}
void TimerFunction(int value)
{

	UpdateVBO(); // vbo 업데이트
	glutPostRedisplay(); // 화면 재 출력
	glutTimerFunc(10, TimerFunction, 1); // 타이머함수 재 설정
}