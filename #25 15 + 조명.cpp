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

GLObj Cube;
GLObj Strawberry;
GLLine lineObj;
GLCamera Camera;
GLLight Light;
GLLine Light_route;

int Mod = 0; // 기본 육면체 출력, 1은 사면체 출력
int Rotation_Mod = 0; // 1 왼쪽으로 2 오른쪽으로 3 위로 4 아래로
bool Back_Cull = true, Solid_Draw = true, Lbt = false;
int start_point = 0, count_point = 36;
glm::vec3 Theta_plus{};
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
	Light.draw_prepare(LightPosLocation, "LightPos");
	Light.draw_prepare(LightColorLocation, "LightColor");
	Light.draw_prepare(WorldTransLocation, "World");
	Light.draw("solid");

	lineObj.Update();
	lineObj.draw_prepare(PosLocation, "Pos");
	lineObj.draw_prepare(ColorLocation, "Color");
	lineObj.draw_prepare(WorldTransLocation, "World");
	lineObj.draw();

	if (Mod == 0) {
		Cube.Update();
		Cube.draw_prepare(PosLocation, "Pos");
		Cube.draw_prepare(ColorLocation, "Color");
		Cube.draw_prepare(NormalLocation, "Normal");
		Cube.draw_prepare(WorldTransLocation, "World");
		Cube.draw("solid");
	}
	else if (Mod == 1) {
		Strawberry.Update();
		Strawberry.draw_prepare(PosLocation, "Pos");
		Strawberry.draw_prepare(ColorLocation, "Color");
		Strawberry.draw_prepare(NormalLocation, "Normal");
		Strawberry.draw_prepare(WorldTransLocation, "World");
		Strawberry.draw("solid");
	}

	Light_route.Update();
	glBindBuffer(GL_ARRAY_BUFFER, Light_route.v_pos);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, Light_route.v_color);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glUniformMatrix4fv(WorldTransLocation, 1, GL_FALSE, glm::value_ptr(Light_route.World_mat));
	glDrawArrays(GL_LINE_LOOP, 0, 359);

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
		if (Mod == 0) Cube.rotate_theta += Theta_plus;
		else Strawberry.rotate_theta += Theta_plus;
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
		// 큐브 / 호박
	case 'n':
	case 'N': {
		if (Mod == 0) Mod = 1;
		else Mod = 0;
		break;
	}
	case 'm':
	case 'M': {
		if (Light.L_color.r == 1.f) Light.L_color = glm::vec3{ 0.1f, 0.1f, 0.1f };
		else Light.L_color = glm::vec3{ 1.f, 1.f, 1.f };
		break;
	}
	case 'a':
	case 'A': {
		Light.revolve_theta += glm::vec3{ 0.0f, -5.0f, 0.0f };
		break;
	}
	case 'd':
	case 'D': {
		Light.revolve_theta += glm::vec3{ 0.0f, 5.0f, 0.0f };
		break;
	}
	case 'z':
	case 'Z': {
		Light.pos += glm::vec3{ 0.0f, 0.f, -0.02f };

		glm::vec3 rob[360];
		GLfloat r = Light.pos.z;
		for (int i = 0; i < 360; ++i) {
			rob[i].x = r * glm::cos(glm::radians((float)i));
			rob[i].y = 0.f;
			rob[i].z = r * glm::sin(glm::radians((float)i));
		}
		glBindBuffer(GL_ARRAY_BUFFER, Light_route.v_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rob), rob, GL_STATIC_DRAW);
		break;
	}
	case 'x':
	case 'X': {
		Light.pos += glm::vec3{ 0.0f, 0.f, 0.02f };

		glm::vec3 rob[360];
		GLfloat r = Light.pos.z;
		for (int i = 0; i < 360; ++i) {
			rob[i].x = r * glm::cos(glm::radians((float)i));
			rob[i].y = 0.f;
			rob[i].z = r * glm::sin(glm::radians((float)i));
		}
		glBindBuffer(GL_ARRAY_BUFFER, Light_route.v_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rob), rob, GL_STATIC_DRAW);
		break;
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
		// 리셋
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
	case GLUT_KEY_LEFT: {
		if (Mod == 0)
			Cube.pos += glm::vec3{ -0.005f, 0.0f, 0.0f };
		else if (Mod == 1)
			Strawberry.pos += glm::vec3{ -0.005f, 0.0f, 0.0f };
		break;
	}
	case GLUT_KEY_RIGHT: {
		if (Mod == 0)
			Cube.pos += glm::vec3{ 0.005f, 0.0f, 0.0f };
		else if (Mod == 1)
			Strawberry.pos += glm::vec3{ 0.005f, 0.0f, 0.0f };
		break;
	}
	case GLUT_KEY_UP: {
		if (Mod == 0)
			Cube.pos += glm::vec3{ 0.0f, 0.005f, 0.0f };
		else if (Mod == 1)
			Strawberry.pos += glm::vec3{ 0.0f, 0.005f, 0.0f };
		break;
	}
	case GLUT_KEY_DOWN: {
		if (Mod == 0)
			Cube.pos += glm::vec3{ 0.0f, -0.005f, 0.0f };
		else if (Mod == 1)
			Strawberry.pos += glm::vec3{ 0.0f, -0.005f, 0.0f };
		break;
	}
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

		Light.pos = glm::vec3{ 0.f, 0.f, 0.5f };
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
	// 육면체 Cube
	{
		std::ifstream inputFile("./OBJ/cube.obj");

		if (inputFile.is_open())
			Cube.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Cube.pos = glm::vec3{ 0.f, 0.f, 0.f };
		Cube.scale = glm::vec3{ 0.3f, 0.3f, 0.3f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ 0.23f, 0.52f, 1.0f };
		for (int i = 0; i < Cube.face_cnt * 3; ++i) {
			color.emplace_back(a);
			//if (i % 6 == 5)
			//	a = glm::vec3{ colorRd(rd), colorRd(rd), colorRd(rd) };
		}

		glGenBuffers(1, &Cube.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Cube.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 딸기 Strawberry
	{
		std::ifstream inputFile("./OBJ/pumpkin.obj");

		if (inputFile.is_open())
			Strawberry.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Strawberry.pos = glm::vec3{ 0.f, 0.f, 0.f };
		Strawberry.scale = glm::vec3{ 0.6f, 0.6f, 0.6f };
		Strawberry.size *= Strawberry.scale;

		std::vector<glm::vec3> color;
		glm::vec3 a{ 0.23f, 0.52f, 1.0f };
		for (int i = 0; i < Strawberry.face_cnt * 3; ++i) {
			color.emplace_back(a);
			//if (i % 6 == 5)
			//	a = glm::vec3{ colorRd(rd), colorRd(rd), colorRd(rd) };
		}

		glGenBuffers(1, &Strawberry.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Strawberry.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 조명 경로
	{
		glm::vec3 color{ 1.f, 1.f, 1.f };
		glm::vec3 rob[360]{};
		glm::vec3 bin[360]{};

		GLfloat r = Light.pos.z;
		for (int i = 0; i < 360; ++i) {
			bin[i] = color;
			rob[i].x = r * glm::cos(glm::radians((float)i));
			rob[i].y = 0.f;
			rob[i].z = r * glm::sin(glm::radians((float)i));
		}

		glGenBuffers(1, &Light_route.v_pos);
		glBindBuffer(GL_ARRAY_BUFFER, Light_route.v_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rob), rob, GL_STATIC_DRAW);
		glGenBuffers(1, &Light_route.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Light_route.v_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bin), bin, GL_STATIC_DRAW);
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

	cout << " n: 육면체 / 호박 그리기" << endl <<
		" m : 조명 켜기 / 끄기" << endl <<
		" y : 객체를 y축에 대하여 회전(제자리에서 자전)" << endl <<
		" a / d : 조명을 객체의 중심 y축에 대하여 양 / 음 방향으로 공전시키기" << endl <<
		" 조명의 공전 궤도를 원으로 그린다." << endl <<
		" z / Z : 조명을 객체에 가깝게 / 멀게 이동하기" << endl <<
		" 조명의 위치가 바뀌게 되면 공전 궤도의 크기도 같이 바뀐다." << endl <<
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