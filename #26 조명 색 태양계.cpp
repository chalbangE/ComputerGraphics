#include "stdafx.h"
#include "GLShapes.h"
#include "GLObj.h"
#include "GLPyramid.h"
#include "GLLight.h"
#include "GLLIne.h"
#include "GLCamera.h"

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);

void Init();
void TimerFunction(int value);
void Keyboard(unsigned char key, int x, int y);
void Special_Keyboard(int key, int x, int y);

using namespace std;

float winSizex = 800, winSizey = 800;
GLuint vao;

GLObj Holly;
GLObj Pumkin;
GLObj Heart;
GLLine lineObj;
GLCamera Camera;
GLLight Light;

bool Lbt = false;
glm::vec3 click_mouse{};

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#25 15 + 조명");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Init();
	//Clear();
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
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

	// 버텍스 쉐이더에게 전달
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	int NormalLocation = glGetAttribLocation(shaderProgramID, "in_Normal");
	unsigned int WorldTransLocation = glGetUniformLocation(shaderProgramID, "World_trans");
	unsigned int CameraLocation = glGetUniformLocation(shaderProgramID, "Camera_trans");
	unsigned int ProjectionLocation = glGetUniformLocation(shaderProgramID, "Projection_trans");
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glEnableVertexAttribArray(NormalLocation);
	glEnableVertexAttribArray(WorldTransLocation);

	// 프래그먼트 쉐이더에게 전달
	int LightPosLocation = glGetUniformLocation(shaderProgramID, "Light_Pos");
	unsigned int LightColorLocation = glGetUniformLocation(shaderProgramID, "Light_Color");
	unsigned int ViewPosLocation = glGetUniformLocation(shaderProgramID, "View_Pos");
	glEnableVertexAttribArray(LightPosLocation);
	glEnableVertexAttribArray(LightColorLocation);
	glEnableVertexAttribArray(ViewPosLocation);

	// 카메라 변환
	Camera.Update();
	Camera.draw_prepare(ViewPosLocation, "View_Pos");

	// 투영 변환
	glm::mat4 Projection_Mat = glm::mat4(1.0f);
	Projection_Mat = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 50.f);
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, &Projection_Mat[0][0]);
	glUniformMatrix4fv(CameraLocation, 1, GL_FALSE, glm::value_ptr(Camera.Camera_Mat));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// 광원
	Light.Update();
	Light.draw_prepare(PosLocation, "Pos");
	Light.draw_prepare(ColorLocation, "Color");
	Light.draw_prepare(NormalLocation, "Normal");
	Light.draw_prepare(WorldTransLocation, "World");
	Light.draw_prepare(LightPosLocation, "LightPos");
	Light.draw_prepare(LightColorLocation, "LightColor");
	Light.draw("solid");

	lineObj.Update();
	lineObj.draw_prepare(PosLocation, "Pos");
	lineObj.draw_prepare(ColorLocation, "Color");
	lineObj.draw_prepare(WorldTransLocation, "World");
	lineObj.draw();

	Holly.Update();
	Holly.draw_prepare(PosLocation, "Pos");
	Holly.draw_prepare(ColorLocation, "Color");
	Holly.draw_prepare(NormalLocation, "Normal");
	Holly.draw_prepare(WorldTransLocation, "World");
	Holly.draw("solid");

	Pumkin.Update();
	Pumkin.draw_prepare(PosLocation, "Pos");
	Pumkin.draw_prepare(ColorLocation, "Color");
	Pumkin.draw_prepare(NormalLocation, "Normal");
	Pumkin.draw_prepare(WorldTransLocation, "World");
	Pumkin.draw("solid");

	Heart.Update();
	Heart.draw_prepare(PosLocation, "Pos");
	Heart.draw_prepare(ColorLocation, "Color");
	Heart.draw_prepare(NormalLocation, "Normal");
	Heart.draw_prepare(WorldTransLocation, "World");
	Heart.draw("solid");

	glDisable(GL_DEPTH_TEST);

	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);
	glDisableVertexAttribArray(NormalLocation);
	glDisableVertexAttribArray(WorldTransLocation);
	glDisableVertexAttribArray(LightPosLocation);
	glDisableVertexAttribArray(LightColorLocation);
	glDisableVertexAttribArray(ViewPosLocation);
	glDisableVertexAttribArray(ProjectionLocation);
	glDisableVertexAttribArray(CameraLocation);

	glutSwapBuffers();
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

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
	case 'A': {
		Light.revolve_theta.y -= 5.f;
		break;
	}
	case 'd':
	case 'D': {
		Light.revolve_theta.y += 5.f;
		break;
	}
	case 'c':
	case 'C': {
		static int color = 1;
		if (color == 0)
			Light.L_color = glm::vec3{ 1.f, 1.f, 1.f };
		else if (color == 1)
			Light.L_color = glm::vec3{ 1.f, 0.1f, 0.1f };
		else if (color == 2)
			Light.L_color = glm::vec3{ 0.1f, 1.f, 0.1f };
		else if (color == 3) {
			Light.L_color = glm::vec3{ 0.1f, 0.1f, 1.f };
			color = -1;
		}
		color++;
		break;
	}
	case 'q':
	case 'Q': {
		exit(829);
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
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

GLvoid Mouse(int button, int state, int x, int y)
{
	glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };

	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			Lbt = true;
			click_mouse = m;
		}
	}
	else if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			Lbt = false;
			click_mouse = glm::vec3{ 0.f, 0.f, 0.f };
		}
	}
}

GLvoid Motion(int x, int y)
{
	if (Lbt) {
		glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };

		if (m.x < click_mouse.x)
			Camera.revolve_theta.z += 1.f;
		else if (m.x > click_mouse.x)
			Camera.revolve_theta.z += -1.f;

		if (m.y < click_mouse.y)
			Camera.revolve_theta.x += 1.f;
		else if (m.y > click_mouse.y)
			Camera.revolve_theta.x += -1.f;

		click_mouse = m;
	}
}

void Init()
{
	glBindVertexArray(vao);
	// 카메라
	Camera.pos = glm::vec3{ 1.f, 1.f, 3.f };

	// Light
	{
		std::ifstream inputFile("./OBJ/sphere.obj");

		if (inputFile.is_open())
			Light.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Light.pos = Camera.pos;
		Light.scale = glm::vec3{ 0.05f, 0.05f, 0.05f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ 0.23f, 0.52f, 1.0f };
		for (int i = 0; i < Light.face_cnt * 3; ++i) {
			color.emplace_back(a);
		}
		Light.L_color = glm::vec3{ 1.f, 1.f, 1.f };

		glGenBuffers(1, &Light.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Light.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// Holly
	{
		std::ifstream inputFile("./OBJ/Holly.obj");

		if (inputFile.is_open())
			Holly.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Holly.pos = glm::vec3{ -0.7f, 0.f, 0.f };
		Holly.pos = glm::vec3{ 0.f, 0.f, 0.f };
		Holly.scale = glm::vec3{ 0.03f, 0.03f, 0.03f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ 1, 0, 0 };
		for (int i = 0; i < Holly.face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Holly.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Holly.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	//  Pumkin
	{
		std::ifstream inputFile("./OBJ/pumpkin.obj");

		if (inputFile.is_open())
			Pumkin.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Pumkin.pos = glm::vec3{ -0.35f, 0.f, 0.f };
		Pumkin.scale = glm::vec3{ 0.5f, 0.5f, 0.5f };
		Pumkin.size *= Pumkin.scale;

		std::vector<glm::vec3> color;
		glm::vec3 a{ 0, 1, 0 };
		for (int i = 0; i < Pumkin.face_cnt * 3; ++i) {
			color.emplace_back(a);
			//if (i % 6 == 5)
			//	a = glm::vec3{ colorRd(rd), colorRd(rd), colorRd(rd) };
		}

		glGenBuffers(1, &Pumkin.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Pumkin.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	//  Heart
	{
		std::ifstream inputFile("./OBJ/heart.obj");

		if (inputFile.is_open())
			Heart.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Heart.pos = glm::vec3{ -0.7f, 0.f, 0.f };
		Heart.scale = glm::vec3{ 0.02f, 0.02f, 0.02f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ 0, 0, 1 };
		for (int i = 0; i < Heart.face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Heart.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Heart.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// X축 Y축
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
	}

	cout << " c : 조명 색을 다른 색으로 바뀌도록 한다. 3종류의 다른 색을 적용해본다" << endl <<
		" r/R : 조명의 위치를 중심의 구의 y축에 대하여 양/음 방향으로 회전한다" << endl <<
		" q : 프로그램 종료" << endl;
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
	vertexSource = filetobuf("3d_vertex_v3_light.glsl");
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
	fragmentSource = filetobuf("light_fragment.glsl");
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