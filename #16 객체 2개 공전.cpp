#include "stdafx.h"
#include "GLShapes.h"
#include "GLCube.h"
#include "GLObj.h"
// #include "GLTetrahedron.h"
// #include "GLPyramid.h"
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
//void Special_Keyboard(int key, int x, int y);

float winSizex = 800, winSizey = 800;
GLuint vao;

GLObj cubeObj;
GLObj sphereObj;
GLObj cylinderObj;
GLObj potObj;
GLLine lineObj;

int Mod = 0, Select = 2; // 기본 육면체 출력, 1은 사면체 출력
int Rotation_Mod = 0; // 1 왼쪽으로 2 오른쪽으로 3 위로 4 아래로
glm::vec3 Theta_plus{}, Revolve_Theta_plus{};

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#16");
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
	//glutSpecialFunc(Special_Keyboard); // 키보드 입력 콜백함수
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
	glDrawArrays(GL_LINES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// std::cout << (int)glIsEnabled(GL_CULL_FACE) << std::endl;

	if (Mod == 0) {
		cubeObj.Update();
		cubeObj.draw_prepare(PosLocation, "Pos");
		cubeObj.draw_prepare(ColorLocation, "Color");
		cubeObj.draw_prepare(WorldTransLocation, "World");
		cubeObj.draw("line");

		sphereObj.Update();
		sphereObj.draw_prepare(PosLocation, "Pos");
		sphereObj.draw_prepare(ColorLocation, "Color");
		sphereObj.draw_prepare(WorldTransLocation, "World");
		sphereObj.draw("line");
	}
	else if (Mod == 1) {
		cylinderObj.Update();
		cylinderObj.draw_prepare(PosLocation, "Pos");
		cylinderObj.draw_prepare(ColorLocation, "Color");
		cylinderObj.draw_prepare(WorldTransLocation, "World");
		cylinderObj.draw("line");

		potObj.Update();
		potObj.draw_prepare(PosLocation, "Pos");
		potObj.draw_prepare(ColorLocation, "Color");
		potObj.draw_prepare(WorldTransLocation, "World");
		potObj.draw("line");
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

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
		if (Mod == 0) {
			if (Select == 0) {
				cubeObj.rotate_theta += Theta_plus;
				cubeObj.revolve_theta += Revolve_Theta_plus;
			}
			else if (Select == 1) {
				sphereObj.rotate_theta += Theta_plus;
				sphereObj.revolve_theta += Revolve_Theta_plus;
			}
			else {
				cubeObj.rotate_theta += Theta_plus;
				sphereObj.rotate_theta += Theta_plus;
				cubeObj.revolve_theta += Revolve_Theta_plus;
				sphereObj.revolve_theta += Revolve_Theta_plus;
			}
		}
		else {
			if (Select == 0) {
				cylinderObj.rotate_theta += Theta_plus;
				cylinderObj.revolve_theta += Revolve_Theta_plus;
			}
			else if (Select == 1) {
				potObj.rotate_theta += Theta_plus;
				potObj.revolve_theta += Revolve_Theta_plus;
			}
			else {
				cylinderObj.rotate_theta += Theta_plus;
				potObj.rotate_theta += Theta_plus;
				cylinderObj.revolve_theta += Revolve_Theta_plus;
				potObj.revolve_theta += Revolve_Theta_plus;
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

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
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
	case 'a':
	case 'A': {
		if (Revolve_Theta_plus.y == 2.0f) {
			Revolve_Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else
			Revolve_Theta_plus = glm::vec3{ 0.0f, 2.0f, 0.0f };
		break;
	}
	case 'd':
	case 'D': {
		if (Revolve_Theta_plus.y == -2.0f) {
			Revolve_Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else
			Revolve_Theta_plus = glm::vec3{ 0.0f, -2.0f, 0.0f };
		break;
	}
	case 'c':
	case 'C': {
		if (Mod == 0) Mod = 1;
		else Mod = 0;
		break;
	}
	case 's':
	case 'S': {
		cylinderObj.rotate_theta.x = 30.0f;
		cylinderObj.rotate_theta.y = -30.0f;
		cylinderObj.revolve_theta.x = 30.0f;
		cylinderObj.revolve_theta.y = -30.0f;

		cubeObj.rotate_theta.x = 30.0f;
		cubeObj.rotate_theta.y = -30.0f;
		cubeObj.revolve_theta.x = 30.0f;
		cubeObj.revolve_theta.y = -30.0f;

		potObj.rotate_theta.x = 30.0f;
		potObj.rotate_theta.y = -30.0f;
		potObj.revolve_theta.x = 30.0f;
		potObj.revolve_theta.y = -30.0f;

		sphereObj.rotate_theta.x = 30.0f;
		sphereObj.rotate_theta.y = -30.0f;
		sphereObj.revolve_theta.x = 30.0f;
		sphereObj.revolve_theta.y = -30.0f;

		Revolve_Theta_plus = glm::vec3{ 0.0f, 0.0f, 0.0f };
		Theta_plus = glm::vec3{ 0.0f, 0.0f, 0.0f };
		break;
	}
	case '1': {
		Select = 0;
		break;
	}
	case '2': {
		Select = 1;
		break;
	}
	case '3': {
		Select = 2;
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

	// Cube OBJ
	{
		std::ifstream inputFile("./OBJ/cube.obj");

		if (inputFile.is_open()) 
			cubeObj.objLoad(inputFile);
		else 
			std::cerr << "Failed to obj file" << std::endl;

		cubeObj.rotate_theta.x = 30.0f;
		cubeObj.rotate_theta.y = -30.0f;
		cubeObj.revolve_theta.x = 30.0f;
		cubeObj.revolve_theta.y = -30.0f;
		cubeObj.pos = glm::vec3{ -0.5f, 0.0f, 0.0f };
		cubeObj.scale = glm::vec3{ 0.25f, 0.25f, 0.25f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < cubeObj.face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), colorRd(gen) };
			color.emplace_back(a);
		}

		glGenBuffers(1, &cubeObj.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, cubeObj.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// Sphere OBJ
	{
		std::ifstream inputFile("./OBJ/sphere.obj");

		if (inputFile.is_open()) 
			sphereObj.objLoad(inputFile);
		else 
			std::cerr << "Failed to obj file" << std::endl;

		sphereObj.rotate_theta.x = 30.0f;
		sphereObj.rotate_theta.y = -30.0f;
		sphereObj.revolve_theta.x = 30.0f;
		sphereObj.revolve_theta.y = -30.0f;
		sphereObj.pos = glm::vec3{ 0.5f, 0.0f, 0.0f };
		sphereObj.scale = glm::vec3{ 0.2f, 0.2f, 0.2f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < sphereObj.face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), colorRd(gen) };
			color.emplace_back(a);
		}

		glGenBuffers(1, &sphereObj.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, sphereObj.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// Cylinder OBJ
	{
		std::ifstream inputFile("./OBJ/cylinder.obj");

		if (inputFile.is_open()) 
			cylinderObj.objLoad(inputFile);
		else 
			std::cerr << "Failed to obj file" << std::endl;

		cylinderObj.rotate_theta.x = 30.0f;
		cylinderObj.rotate_theta.y = -30.0f;
		cylinderObj.revolve_theta.x = 30.0f;
		cylinderObj.revolve_theta.y = -30.0f;
		cylinderObj.pos = glm::vec3{ -0.5f, 0.0f, 0.0f };
		cylinderObj.scale = glm::vec3{ 0.18f, 0.18f, 0.18f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < cylinderObj.face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), colorRd(gen) };
			color.emplace_back(a);
		}

		glGenBuffers(1, &cylinderObj.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, cylinderObj.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// Pot OBJ
	{
		std::ifstream inputFile("./OBJ/pot.obj");

		if (inputFile.is_open()) 
			potObj.objLoad(inputFile);
		else 
			std::cerr << "Failed to obj file" << std::endl;

		potObj.rotate_theta.x = 30.0f;
		potObj.rotate_theta.y = -30.0f;
		potObj.revolve_theta.x = 30.0f;
		potObj.revolve_theta.y = -30.0f;
		potObj.pos = glm::vec3{ 0.5f, 0.0f, 0.0f };
		potObj.scale = glm::vec3{ 0.2f, 0.2f, 0.2f };

		std::cout << potObj.face_cnt << std::endl;
		std::vector<glm::vec3> color;
		for (int i = 0; i < potObj.face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), colorRd(gen) };
			color.emplace_back(a);
		}

		glGenBuffers(1, &potObj.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, potObj.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}

	// 축
	{
		glm::vec3 line[6]{
			{1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}
		};

		glGenBuffers(1, &V_pos_Line);
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_Line);
		glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

		lineObj = GLLine({ 0.0f, 0.0f, 0.0f });
		lineObj.rotate_theta.x = 30.0f;
		lineObj.rotate_theta.y = -30.0f;
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