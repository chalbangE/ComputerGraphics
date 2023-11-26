#include "stdafx.h"
#include "GLShapes.h"
#include "GLRect.h"
#include "GLObj.h"
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
static std::uniform_real_distribution<GLclampf> velRd(-0.03f, 0.03);

float winSizex = 800, winSizey = 800;
GLuint vao;

bool CheckCollision(const GLObj& a, const GLObj& b) {
	return (std::abs(a.pos.x - b.pos.x) < (a.size.x + b.size.x) &&
		std::abs(a.pos.y - b.pos.y) < (a.size.y + b.size.y) &&
		std::abs(a.pos.z - b.pos.z) < (a.size.z + b.size.z));
}

GLLine lineObj;
GLCamera Camera;
vector <GLRect> Room;
vector <GLObj> Box;
vector <GLObj> Ball;
glm::vec3 gravity{ 0.f, -0.001f, 0.f };
glm::vec3 All_revolve_rotate{ 0.f, 0.f, 0.f };

bool Back_Cull = true, Lbt = false;
glm::vec3 click_mouse { 0.f, 0.f, 0.f };

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#23 중력");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Init();
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special_Keyboard);
	glutTimerFunc(10, TimerFunction, 1);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene()
{
	glUseProgram(shaderProgramID);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	unsigned int WorldTransLocation = glGetUniformLocation(shaderProgramID, "World_trans");
	unsigned int CameraLocation = glGetUniformLocation(shaderProgramID, "Camera_trans");
	unsigned int ProjectionLocation = glGetUniformLocation(shaderProgramID, "Projection_trans");
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glEnableVertexAttribArray(WorldTransLocation);

	// 카메라 변환
	Camera.Update();

	// 투영 변환
	glm::mat4 Projection_Mat = glm::mat4(1.0f);
	Projection_Mat = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 50.f);
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, &Projection_Mat[0][0]);
	glUniformMatrix4fv(CameraLocation, 1, GL_FALSE, glm::value_ptr(Camera.Camera_Mat));

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (Back_Cull)  {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	lineObj.Update();
	lineObj.draw_prepare(PosLocation, "Pos");
	lineObj.draw_prepare(ColorLocation, "Color");
	lineObj.draw_prepare(WorldTransLocation, "World");
	lineObj.draw();

	for (int i = 0; i < Room.size(); ++i){
		Room[i].revolve_theta = All_revolve_rotate;
		Room[i].Update();
		Room[i].draw_prepare(PosLocation, "Pos");
		Room[i].draw_prepare(ColorLocation, "Color");
		Room[i].draw_prepare(WorldTransLocation, "World");
		Room[i].draw("solid");
	}

	for (int i = 0; i < Box.size(); ++i){
		Box[i].revolve_theta = All_revolve_rotate;
		Box[i].Update();
		Box[i].draw_prepare(PosLocation, "Pos");
		Box[i].draw_prepare(ColorLocation, "Color");
		Box[i].draw_prepare(WorldTransLocation, "World");
		Box[i].draw("polygon");
	}

	for (int i = 0; i < Ball.size(); ++i){
		Ball[i].revolve_theta = All_revolve_rotate;
		Ball[i].Update();
		Ball[i].draw_prepare(PosLocation, "Pos");
		Ball[i].draw_prepare(ColorLocation, "Color");
		Ball[i].draw_prepare(WorldTransLocation, "World");
		Ball[i].draw("polygon");
	}

	if (Back_Cull) 
	{
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
		for (int i = 0; i < Box.size(); ++i) {
			if ((Box[i].pos.x - Box[i].size.x) + Box[i].velocity.x < -1.f || (Box[i].pos.x + Box[i].size.x) + Box[i].velocity.x > 1.f)
				Box[i].velocity.x = 0.f;
			if (((Box[i].pos.y - Box[i].size.y) + Box[i].velocity.y < -1.f || (Box[i].pos.y + Box[i].size.y) + Box[i].velocity.y > 1.f) && Room.size() == 6)
				Box[i].velocity.y = 0.f;
			if ((Box[i].pos.z - Box[i].size.z) + Box[i].velocity.z < -1.f || (Box[i].pos.z + Box[i].size.z) + Box[i].velocity.z > 1.f)
				Box[i].velocity.z = 0.f;

			Box[i].pos += Box[i].velocity;
			Box[i].velocity += gravity;
		}

		for (int i = 0; i < Ball.size(); ++i) {
			if ((Ball[i].pos.x - Ball[i].size.x) + Ball[i].velocity.x < -1.f || (Ball[i].pos.x + Ball[i].size.x) + Ball[i].velocity.x > 1.f)
				Ball[i].velocity.x *= -1.f;
			if ((Ball[i].pos.y - Ball[i].size.y) + Ball[i].velocity.y < -1.f || (Ball[i].pos.y + Ball[i].size.y) + Ball[i].velocity.y > 1.f)
				Ball[i].velocity.y *= -1.f;
			if ((Ball[i].pos.z - Ball[i].size.z) + Ball[i].velocity.z < -1.f || (Ball[i].pos.z + Ball[i].size.z) + Ball[i].velocity.z > 1.f)
				Ball[i].velocity.z *= -1.f;

			Ball[i].pos += Ball[i].velocity;
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
	case 'a':
	case 'A': {
		All_revolve_rotate.y += 1.f;
		break;
	}
	case 'o':
	case 'O': {
		Room.erase(Room.begin() + 5);
		break;
	}
	case 'd':
	case 'D': {
		All_revolve_rotate.y -= 1.f;
		break;
	}
	case 'b':
	case 'B': {
		{
			std::ifstream inputFile("./OBJ/sphere.obj");
			Ball.emplace_back();

			if (inputFile.is_open())
				Ball.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Ball.back().pos = glm::vec3{ 0.f, 0.f, 0.5f };
			Ball.back().scale = glm::vec3{ 0.1f, 0.1f, 0.1f };
			Ball.back().size *= Ball.back().scale;

			Ball.back().velocity = glm::vec3{ velRd(rd), velRd(rd), velRd(rd) };

			std::vector<glm::vec3> color;
			glm::vec3 a{ colorRd(rd), colorRd(rd), colorRd(rd) };
			for (int i = 0; i < Ball.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Ball.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Ball.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		if (Ball.size() > 5) {
			Ball.erase(Ball.begin());
		}
		break;
	}
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

		if (m.x < click_mouse.x) {
			All_revolve_rotate.z += 1.5f;
		}
		else if (m.x > click_mouse.x) {
			All_revolve_rotate.z += -1.5f;
		}

		glm::mat4 g_mat(1.f);
		g_mat = glm::rotate(g_mat, glm::radians(-All_revolve_rotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec4 g{ 0.f, -0.001f, 0.f, 1.f };
		g = g_mat * g;
		gravity = glm::vec3{ g.x, g.y, g.z };
		click_mouse = m;
	}
}

void Init()
{
	glBindVertexArray(vao);

	// 카메라
	Camera.pos = glm::vec3{ 0.f, 0.f, 4.f };
	// Room
	{
		{ // z축 기준 옆
			Room.emplace_back(GLRect(1));
			glm::vec3 cube[6]{
				{ -1.f, 1.f, -1.f }, { -1.f, -1.f, -1.f }, { 1.f, 1.f, -1.f },
				{ 1.f, 1.f, -1.f }, { -1.f, -1.f, -1.f }, { 1.f, -1.f, -1.f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
		{ // z축 기준 옆
			Room.emplace_back(GLRect(1));
			glm::vec3 cube[6]{
				{ 1.f, -1.f, 1.f }, { -1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f },
				{ 1.f, 1.f, 1.f }, { -1.f, -1.f, 1.f },{ -1.f, 1.f, 1.f } 
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
		{ // x축 기준 옆
			Room.emplace_back(GLRect(1));
			glm::vec3 cube[6]{
				{ 1.f, 1.f, -1.f }, { 1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f },
				 { 1.f, 1.f, 1.f }, { 1.f, -1.f, -1.f }, { 1.f, -1.f, 1.f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
		{ // x축 기준 옆
			Room.emplace_back(GLRect(1));
			glm::vec3 cube[6]{
				{ -1.f, 1.f, 1.f }, { -1.f, -1.f, 1.f }, { -1.f, 1.f, -1.f },
				{ -1.f, 1.f, -1.f }, { -1.f, -1.f, 1.f }, { -1.f, -1.f, -1.f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
			Room.back().midpos.y = 1.f;
		}
		{  // y축 기준 옆
			Room.emplace_back(GLRect(1));
			glm::vec3 cube[6]{
				{ 1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f }, { -1.f, 1.f, -1.f },
				{ -1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f }, { -1.f, 1.f, 1.f } 
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
		{ // y축 기준 옆
			Room.emplace_back(GLRect(1));
			glm::vec3 cube[6]{
				{ -1.f, -1.f, 1.f }, { 1.f, -1.f, 1.f }, { -1.f, -1.f, -1.f },
				{ -1.f, -1.f, -1.f }, { 1.f, -1.f, 1.f }, { 1.f, -1.f, -1.f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
	}

	// Box
	{
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			Box.emplace_back();

			if (inputFile.is_open())
				Box.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Box.back().pos = glm::vec3{ 0.f, 0.f, 0.5f };
			Box.back().scale = glm::vec3{ 0.1f, 0.1f, 0.1f };
			Box.back().size *= Box.back().scale;

			std::vector<glm::vec3> color;
			glm::vec3 a{ colorRd(rd), colorRd(rd), colorRd(rd) };
			for (int i = 0; i < Box.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Box.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Box.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			Box.emplace_back();

			if (inputFile.is_open())
				Box.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Box.back().pos = glm::vec3{ 0.f, 0.f, 0.f };
			Box.back().scale = glm::vec3{ 0.2f, 0.2f, 0.2f };
			Box.back().size *= Box.back().scale;

			std::vector<glm::vec3> color;
			glm::vec3 a{ colorRd(rd), colorRd(rd), colorRd(rd) };
			for (int i = 0; i < Box.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Box.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Box.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			Box.emplace_back();

			if (inputFile.is_open())
				Box.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Box.back().pos = glm::vec3{ 0.f, 0.f, -0.5f };
			Box.back().scale = glm::vec3{ 0.3f, 0.3f, 0.3f };
			Box.back().size *= Box.back().scale;

			std::vector<glm::vec3> color;
			glm::vec3 a{ colorRd(rd), colorRd(rd), colorRd(rd) };
			for (int i = 0; i < Box.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Box.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Box.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
	}

	// Ball
	{
		{
			std::ifstream inputFile("./OBJ/sphere.obj");
			Ball.emplace_back();

			if (inputFile.is_open())
				Ball.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Ball.back().pos = glm::vec3{ 0.f, 0.f, 0.5f };
			Ball.back().scale = glm::vec3{ 0.1f, 0.1f, 0.1f };
			Ball.back().size *= Ball.back().scale;

			Ball.back().velocity = glm::vec3{ velRd(rd), velRd(rd), velRd(rd) };

			std::vector<glm::vec3> color;
			glm::vec3 a{ colorRd(rd), colorRd(rd), colorRd(rd) };
			for (int i = 0; i < Ball.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Ball.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Ball.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		{
			std::ifstream inputFile("./OBJ/sphere.obj");
			Ball.emplace_back();

			if (inputFile.is_open())
				Ball.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Ball.back().pos = glm::vec3{ 0.f, 0.f, 0.5f };
			Ball.back().scale = glm::vec3{ 0.1f, 0.1f, 0.1f };
			Ball.back().size *= Ball.back().scale;

			Ball.back().velocity = glm::vec3{ velRd(rd), velRd(rd), velRd(rd) };

			std::vector<glm::vec3> color;
			glm::vec3 a{ colorRd(rd), colorRd(rd), colorRd(rd) };
			for (int i = 0; i < Ball.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Ball.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Ball.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
	}

	cout << "a/d: 화면의 Y축에 대하여 시계/반시계 방향으로 회전" << endl << 
		 "o/O: 바닥 열리기" << endl << 
		"B : 볼이 새로 생겨서 튕기기 시작한다(최대 5개" << endl;

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
	vertexSource = filetobuf("3d_vertex_v2.glsl");
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