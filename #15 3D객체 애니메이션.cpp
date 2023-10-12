#include "stdafx.h"
#include "GLShapes.h"
#include "GLCube.h"
// #include "GLTetrahedron.h"
#include "GLPyramid.h"
#include "GLLIne.h"

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);

void Init();
void TimerFunction(int value);
void Keyboard(unsigned char key, int x, int y);
void Special_Keyboard(int key, int x, int y);

float winSizex = 800, winSizey = 800;
GLuint vao;

GLCube cubeObj;
GLPyramid pyraObj;
GLLine lineObj;

int Mod = 0; // 기본 육면체 출력, 1은 사면체 출력
int Rotation_Mod = 0; // 1 왼쪽으로 2 오른쪽으로 3 위로 4 아래로
bool Back_Cull = true, Solid_Draw = true;
int start_point = 0, count_point = 36;
glm::vec3 Theta_plus{};

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#15 3D객체 애니메이션");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Init();
	//Clear();
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutSpecialFunc(Special_Keyboard); // 키보드 입력 콜백함수
	glutTimerFunc(10, TimerFunction, 1);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene()
{
	glUseProgram(shaderProgramID);

	// Location 번호 저장
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	unsigned int WorldTransLocation = glGetUniformLocation(shaderProgramID, "World_trans");
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glEnableVertexAttribArray(WorldTransLocation);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// X축 Y축 그리기
	lineObj.Update();
	lineObj.draw_prepare(PosLocation, "Pos");
	lineObj.draw_prepare(ColorLocation, "Color");
	lineObj.draw_prepare(WorldTransLocation, "World");
	lineObj.draw();

	if (Back_Cull) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}
	// std::cout << (int)glIsEnabled(GL_CULL_FACE) << std::endl;

	if (Mod == 0) {
		cubeObj.Update();
		cubeObj.draw_prepare(PosLocation, "Pos");
		cubeObj.draw_prepare(ColorLocation, "Color");
		cubeObj.draw_prepare(WorldTransLocation, "World");
		if (Solid_Draw) cubeObj.draw("solid");
		else cubeObj.draw("line");
	}
	else if (Mod == 1) {
		pyraObj.Update();
		pyraObj.draw_prepare(PosLocation, "Pos");
		pyraObj.draw_prepare(ColorLocation, "Color");
		pyraObj.draw_prepare(WorldTransLocation, "World");
		if (Solid_Draw) pyraObj.draw("solid");
		else pyraObj.draw("line");
	}

	if (Back_Cull) {
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}

	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);
	glDisableVertexAttribArray(WorldTransLocation);

	glutSwapBuffers();
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		if (Mod == 0) cubeObj.rotate_theta += Theta_plus;
		else pyraObj.rotate_theta += Theta_plus;
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
	glutTimerFunc(10, TimerFunction, 1);
}

void Keyboard(unsigned char key, int x, int y)
{
	// 좌 / 우 / 상 / 하로 객체를 이동한다. (x축, y축으로 이동)
	//	s : 초기 위치로 리셋(자전 애니메이션도 멈추기)
	switch (key)
	{
		// 큐브 / 피라미드
		{
	case 'c':
	case 'C': {
		Mod = 0;
		break;
	}
	case 'p':
	case 'P': {
		Mod = 1;
		break;
	}
		}
		// 은면제거 / 면 or 선
		{
	case 'h':
	case 'H': {
		if (Back_Cull) Back_Cull = false;
		else Back_Cull = true;
		break;
	}
	case 'w':
	case 'W': {
		if (Solid_Draw) Solid_Draw = false;
		else Solid_Draw = true;
		break;
	}
		}
		// 자전키
		{
	case 'j':
	case 'J': {
		Theta_plus = glm::vec3{ 0.0f, -2.0f, 0.0f };
		if (Rotation_Mod == 1) {
			Rotation_Mod = 0;
			Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else Rotation_Mod = 1;
		break;
	}
	case 'l':
	case 'L': {
		Theta_plus = glm::vec3{ 0.0f, 2.0f, 0.0f };
		if (Rotation_Mod == 2) {
			Rotation_Mod = 0;
			Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else  Rotation_Mod = 2;
		break;
	}
	case 'i':
	case 'I': {
		Theta_plus = glm::vec3{ 2.0f, 0.0f, 0.0f };
		if (Rotation_Mod == 3) {
			Rotation_Mod = 0;
			Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else Rotation_Mod = 3;
		break;
	}
	case 'k':
	case 'K': {
		Theta_plus = glm::vec3{ -2.0f, 0.0f, 0.0f };
		if (Rotation_Mod == 4) {
			Rotation_Mod = 0;
			Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else Rotation_Mod = 4;
		break;
	}
		}
		// 리셋
	case 's':
	case 'S': {
		Init();
		Rotation_Mod = 0;
		Theta_plus = { 0.0f, 0.0f, 0.0f };
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void Special_Keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: {
		if (Mod == 0)
			cubeObj.pos += glm::vec3{ -0.005f, 0.0f, 0.0f };
		else if (Mod == 1)
			pyraObj.pos += glm::vec3{ -0.005f, 0.0f, 0.0f };
		break;
	}
	case GLUT_KEY_RIGHT: {
		if (Mod == 0)
			cubeObj.pos += glm::vec3{ 0.005f, 0.0f, 0.0f };
		else if (Mod == 1)
			pyraObj.pos += glm::vec3{ 0.005f, 0.0f, 0.0f };
		break;
	}
	case GLUT_KEY_UP: {
		if (Mod == 0)
			cubeObj.pos += glm::vec3{ 0.0f, 0.005f, 0.0f };
		else if (Mod == 1)
			pyraObj.pos += glm::vec3{ 0.0f, 0.005f, 0.0f };
		break;
	}
	case GLUT_KEY_DOWN: {
		if (Mod == 0)
			cubeObj.pos += glm::vec3{ 0.0f, -0.005f, 0.0f };
		else if (Mod == 1)
			pyraObj.pos += glm::vec3{ 0.0f, -0.005f, 0.0f };
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void Init()
{
	glBindVertexArray(vao);

	// 육면체 Cube
	{
		glm::vec3 cube[36]{
			{ -0.1f, -0.1f, 0.1f }, { 0.1f, -0.1f, 0.1f }, { -0.1f, 0.1f, 0.1f },
			{ -0.1f, 0.1f, 0.1f }, { 0.1f, -0.1f, 0.1f }, { 0.1f, 0.1f, 0.1f },

			{ 0.1f, -0.1f, 0.1f }, { 0.1f, -0.1f, -0.1f }, { 0.1f, 0.1f, 0.1f },
			{ 0.1f, 0.1f, 0.1f }, { 0.1f, -0.1f, -0.1f }, { 0.1f, 0.1f, -0.1f },

			{ 0.1f, -0.1f, -0.1f }, { -0.1f, -0.1f, -0.1f }, { 0.1f, 0.1f, -0.1f },
			{ 0.1f, 0.1f, -0.1f }, { -0.1f, -0.1f, -0.1f }, { -0.1f, 0.1f, -0.1f },

			{ -0.1f, -0.1f, -0.1f }, { -0.1f, -0.1f, 0.1f }, { -0.1f, 0.1f, -0.1f },
			{ -0.1f, 0.1f, -0.1f }, { -0.1f, -0.1f, 0.1f }, { -0.1f, 0.1f, 0.1f },

			{ -0.1f, 0.1f, 0.1f }, { 0.1f, 0.1f, 0.1f }, { -0.1f, 0.1f, -0.1f },
			{ -0.1f, 0.1f, -0.1f }, { 0.1f, 0.1f, 0.1f }, { 0.1f, 0.1f, -0.1f },

			{ -0.1f, -0.1f, 0.1f }, { -0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, 0.1f },
			{ -0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, 0.1f }
		};

		glGenBuffers(1, &V_pos_Cube);
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_Cube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

		cubeObj = GLCube({ 0.0f, 0.0f, 0.0f });
		cubeObj.rotate_theta.x = 30.0f;
		cubeObj.rotate_theta.y = -30.0f;
		cubeObj.scale = glm::vec3{ 2.0f, 2.0f, 2.0f };
	}
	// 사각뿔 Pyra
	{
		glm::vec3 pyra[18]{
			{ 0.0f, 0.1f, 0.0f }, { 0.1f, -0.1f, -0.1f }, { -0.1f, -0.1f, -0.1f },

			{ 0.0f, 0.1f, 0.0f }, { 0.1f, -0.1f, 0.1f }, { 0.1f, -0.1f, -0.1f },

			{ 0.0f, 0.1f, 0.0f }, { -0.1f, -0.1f, -0.1f }, { -0.1f, -0.1f, 0.1f },

			{ 0.0f, 0.1f, 0.0f }, { -0.1f, -0.1f, 0.1f }, { 0.1f, -0.1f, 0.1f },

			{ -0.1f, -0.1f, 0.1f }, { -0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, 0.1f },
			{ -0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, 0.1f }
		};

		glGenBuffers(1, &V_pos_Pyra);
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_Pyra);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyra), pyra, GL_STATIC_DRAW);

		pyraObj = GLPyramid({ 0.0f, 0.0f, 0.0f });
		pyraObj.rotate_theta.x = 30.0f;
		pyraObj.rotate_theta.y = -30.0f;
		pyraObj.scale = glm::vec3{ 2.0f, 2.0f, 2.0f };
	}
	// X축 Y축
	{
		glm::vec3 line[4]{
			{1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
		};

		glGenBuffers(1, &V_pos_Line);
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_Line);
		glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

		lineObj = GLLine({ 0.0f, 0.0f, 0.0f });
	}

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
	vertexSource = filetobuf("3d_vertex.glsl");
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