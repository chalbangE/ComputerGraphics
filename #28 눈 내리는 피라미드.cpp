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

void Sierpinski();

using namespace std;

float winSizex = 800, winSizey = 800;
GLuint vao;

vector <GLObj> Pyramid;
vector <GLObj> Snow;
vector <GLObj> Planet;
GLLine lineObj;
GLCamera Camera;
GLLight Light;
GLObj Floor;

int P_cnt = 0;
bool Lbt = false, Snow_mod = false;
glm::vec3 click_mouse{};

static std::uniform_real_distribution<GLfloat> Size(0.0f, 0.01f);
static std::uniform_real_distribution<GLfloat> Pos(-1.f, 1.f);

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#28 눈 내리는 피라미드");
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
	unsigned int NormalTransLocation = glGetUniformLocation(shaderProgramID, "Normal_trans");
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glEnableVertexAttribArray(NormalLocation);
	glEnableVertexAttribArray(WorldTransLocation);
	glEnableVertexAttribArray(NormalTransLocation);

	// 프래그먼트 쉐이더에게 전달
	int LightPosLocation = glGetUniformLocation(shaderProgramID, "Light_Pos");
	unsigned int LightColorLocation = glGetUniformLocation(shaderProgramID, "Light_Color");
	unsigned int ViewPosLocation = glGetUniformLocation(shaderProgramID, "View_Pos");
	unsigned int DistanceLocation = glGetUniformLocation(shaderProgramID, "Distance");
	glEnableVertexAttribArray(LightPosLocation);
	glEnableVertexAttribArray(LightColorLocation);
	glEnableVertexAttribArray(ViewPosLocation);
	glEnableVertexAttribArray(DistanceLocation);

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

	Floor.Update();
	Floor.Normal_Update();
	Floor.draw_prepare(PosLocation, "Pos");
	Floor.draw_prepare(ColorLocation, "Color");
	Floor.draw_prepare(WorldTransLocation, "World");
	Floor.draw_prepare(NormalLocation, "Normal");
	Floor.draw("solid");

	for (int i = 0; i < Planet.size(); ++i) {
		Planet[i].Update();
		Planet[i].Normal_Update();
		Planet[i].draw_prepare(PosLocation, "Pos");
		Planet[i].draw_prepare(ColorLocation, "Color");
		Planet[i].draw_prepare(WorldTransLocation, "World");
		Planet[i].draw_prepare(NormalTransLocation, "Normal_mat");
		Planet[i].draw_prepare(NormalLocation, "Normal");
		glUniform1f(DistanceLocation, distance(Light.pos, Planet[i].pos));
		Planet[i].draw("solid");
	}

	for (int i = 0; i < Pyramid.size(); ++i) {
		Pyramid[i].Update();
		Pyramid[i].Normal_Update();
		Pyramid[i].draw_prepare(PosLocation, "Pos");
		Pyramid[i].draw_prepare(ColorLocation, "Color");
		Pyramid[i].draw_prepare(WorldTransLocation, "World");
		Pyramid[i].draw_prepare(NormalTransLocation, "Normal_mat");
		Pyramid[i].draw_prepare(NormalLocation, "Normal");
		glUniform1f(DistanceLocation, distance(Light.pos, Pyramid[i].pos));
		Pyramid[i].draw("solid");
	}

	for (int i = 1; i < Snow.size(); ++i) {
		Snow[i].Update();
		Snow[i].draw_prepare(PosLocation, "Pos");
		Snow[i].draw_prepare(ColorLocation, "Color");
		Snow[i].draw_prepare(WorldTransLocation, "World");
		Snow[i].draw_prepare(NormalLocation, "Normal");
		Snow[i].draw("solid");
	}

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
	glDisableVertexAttribArray(NormalTransLocation);
	glDisableVertexAttribArray(DistanceLocation);

	glutSwapBuffers();
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		Planet[0].revolve_theta += glm::vec3{ 0.f, 0.f, 1.f };
		Planet[1].revolve_theta += glm::vec3{ 0.f, 1.f, 0.f };
		Planet[2].revolve_theta += glm::vec3{ 1.f, 0.f, 0.f };


		if (Snow_mod) {
			//  Snow 생성
			{
				Snow.emplace_back(*Snow.begin());

				GLfloat size = Size(rd);
				Snow.back().pos = glm::vec3{ Pos(rd), 1.3f, Pos(rd) };
				Snow.back().scale = glm::vec3{ size, size, size };

				std::vector<glm::vec3> color;
				glm::vec3 a{ 1.f, 1.f, 1.f };
				for (int i = 0; i < Snow.back().face_cnt * 3; ++i) {
					color.emplace_back(a);
				}

				glGenBuffers(1, &Snow.back().v_color);
				glBindBuffer(GL_ARRAY_BUFFER, Snow.back().v_color);
				glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
			}

			for (int i = 1; i < Snow.size(); ++i) {
				if (Snow[i].pos.y <= -0.38f && Snow.size() > 1000) {
					Snow.erase(Snow.begin() + i);
					--i;
				}
				else if (Snow[i].pos.y < -0.38f)
					Snow[i].pos.y = -0.38f;

				Snow[i].pos.y -= 0.003f;
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
	case 'a':
	case 'A': {
		Light.revolve_theta.y -= 5.f;
		break;
	}
	case '+':
	case '=': {
		Sierpinski();
		break;
	}
	case 'P':
	case 'p': {
		if (P_cnt == 0)
			Light.pos = glm::vec3{ 1.f, 0.f, 0.f };
		else if (P_cnt == 1)
			Light.pos = glm::vec3{ -1.f, 0.f, 0.f };
		else if (P_cnt == 2)
			Light.pos = glm::vec3{ 0.f, 0.f, 1.f };
		else if (P_cnt == 3)
			Light.pos = glm::vec3{ 0.f, 0.f, -1.f };

		P_cnt++;
		if (P_cnt == 4)
			P_cnt = 0;
		break;
	}
	case '1':	// 피라미드 초기화
	case '!': {
		//  Pyramid
		{
			Pyramid.clear();
			std::ifstream inputFile("./OBJ/pyramid.obj");
			Pyramid.emplace_back();

			if (inputFile.is_open())
				Pyramid.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Pyramid.back().pos = glm::vec3{ 0.f, 0.f, 0.f };
			Pyramid.back().scale = glm::vec3{ 0.5f, 0.5f, 0.5f };

			std::vector<glm::vec3> color;
			glm::vec3 a{ 103 / 255.f, 153 / 255.f, 1.f };
			for (int i = 0; i < Pyramid.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Pyramid.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Pyramid.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		break;
	}
	case 's':
	case 'S': {
		if (Snow_mod) Snow_mod = false;
		else Snow_mod = true;
		break;
	}
	case 'n':
	case 'N': {
		if (Light.pos.z > 0.5f)
			Light.pos += glm::vec3{ 0.0f, 0.f, -0.02f };
		break;
	}
	case 'f':
	case 'F': {
		if (Light.pos.z < 2.f)
			Light.pos += glm::vec3{ 0.0f, 0.f, 0.02f };
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
	case 'm':
	case 'M': {
		if (Light.L_color.r > 0.1f) Light.L_color = glm::vec3{ 0.1f, 0.1f, 0.1f };
		else Light.L_color = glm::vec3{ 1.f, 1.f, 1.f };
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

void Sierpinski()
{
	int cnt = Pyramid.size();

	for (int k = 0; k < cnt; ++k) {
		for (int i = 0; i < 5; ++i) {
			Pyramid.emplace_back(*Pyramid.begin());
			Pyramid.back().scale = Pyramid.begin()->scale / 2.f;

			if (i == 0){ // 위에 놈
				Pyramid.back().pos = Pyramid.begin()->pos;
				Pyramid.back().pos.y += ((Pyramid.begin()->scale.y * 10.f) / 4.f);
			}
			else if (i == 1) { // 왼쪽 위 놈
				Pyramid.back().pos = Pyramid.begin()->pos;
				Pyramid.back().pos.x -= ((Pyramid.begin()->scale.x * 10.f) / 4.f);
				Pyramid.back().pos.y -= ((Pyramid.begin()->scale.y * 10.f) / 4.f);
				Pyramid.back().pos.z -= ((Pyramid.begin()->scale.z * 10.f) / 4.f);
			}
			else if (i == 2) { // 오른쪽 위 놈
				Pyramid.back().pos = Pyramid.begin()->pos;
				Pyramid.back().pos.x += ((Pyramid.begin()->scale.x * 10.f) / 4.f);
				Pyramid.back().pos.y -= ((Pyramid.begin()->scale.y * 10.f) / 4.f);
				Pyramid.back().pos.z -= ((Pyramid.begin()->scale.z * 10.f) / 4.f);
			}
			else if (i == 3) { // 왼쪽 아래 놈
				Pyramid.back().pos = Pyramid.begin()->pos;
				Pyramid.back().pos.x -= ((Pyramid.begin()->scale.x * 10.f) / 4.f);
				Pyramid.back().pos.y -= ((Pyramid.begin()->scale.y * 10.f) / 4.f);
				Pyramid.back().pos.z += ((Pyramid.begin()->scale.z * 10.f) / 4.f);
			}
			else if (i == 4) { // 오른쪽 아래 놈
				Pyramid.back().pos = Pyramid.begin()->pos;
				Pyramid.back().pos.x += ((Pyramid.begin()->scale.x * 10.f) / 4.f);
				Pyramid.back().pos.y -= ((Pyramid.begin()->scale.y * 10.f) / 4.f);
				Pyramid.back().pos.z += ((Pyramid.begin()->scale.z * 10.f) / 4.f);
			}
		}
		Pyramid.erase(Pyramid.begin());
	}
}

void Special_Keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: {
		if (Light.L_color.x < 2.f)
			Light.L_color += glm::vec3{ 0.1f, 0.1f, 0.1f };
		break;
	}
	case GLUT_KEY_DOWN: {
		if (Light.L_color.x > 0.3f)
			Light.L_color -= glm::vec3{ 0.1f, 0.1f, 0.1f };
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

		Light.pos = glm::vec3{ 0.f, 0.7f, 1.f };
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
	//  Pyramid
	{
		std::ifstream inputFile("./OBJ/pyramid.obj");
		Pyramid.emplace_back();

		if (inputFile.is_open())
			Pyramid.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Pyramid.back().pos = glm::vec3{ 0.f, 0.f, 0.f };
		Pyramid.back().midpos = glm::vec3{ 0.f, 0.f, 0.f };
		Pyramid.back().scale = glm::vec3{ 0.08f, 0.08f, 0.08f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ 103 / 255.f, 153 / 255.f, 1.f };
		for (int i = 0; i < Pyramid.back().face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Pyramid.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Pyramid.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	//  Planet
	{
		{
			std::ifstream inputFile("./OBJ/sphere.obj");
			Planet.emplace_back();

			if (inputFile.is_open())
				Planet.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Planet.back().pos = glm::vec3{ 0.f, 1.0f, 0.f };
			Planet.back().scale = glm::vec3{ 0.08f, 0.08f, 0.08f };

			std::vector<glm::vec3> color;
			glm::vec3 a{ 255 / 255.f, 203 / 255.f, 203 / 255.f };
			for (int i = 0; i < Planet.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Planet.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Planet.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}

		{
			Planet.emplace_back(*Planet.begin());

			Planet.back().pos = glm::vec3{ 0.f, 0.f, 1.f };
			Planet.back().revolve_theta = glm::vec3{ 0.f, 90.f, 0.f };
			Planet.back().scale = glm::vec3{ 0.08f, 0.08f, 0.08f };

			std::vector<glm::vec3> color;
			glm::vec3 a{ 206 / 255.f, 242 / 255.f, 121 / 255.f };
			for (int i = 0; i < Planet.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Planet.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Planet.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}

		{
			Planet.emplace_back(*Planet.begin());

			Planet.back().pos = glm::vec3{ 0.f, 0.f, 0.8f };
			Planet.back().revolve_theta = glm::vec3{ 0.f, 180.f, 0.f };
			Planet.back().scale = glm::vec3{ 0.08f, 0.08f, 0.08f };

			std::vector<glm::vec3> color;
			glm::vec3 a{ 71 / 255.f, 66 / 255.f, 219 / 255.f };
			for (int i = 0; i < Planet.back().face_cnt * 3; ++i) {
				color.emplace_back(a);
			}

			glGenBuffers(1, &Planet.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Planet.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
	}

	{
		std::ifstream inputFile("./OBJ/sphere.obj");
		Snow.emplace_back();
		Snow.back().objLoad(inputFile);
	}
	//  Floor
	{
		std::ifstream inputFile("./OBJ/cube.obj");

		if (inputFile.is_open())
			Floor.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Floor.pos = glm::vec3{ 0.f, -0.4f, 0.f };
		Floor.scale = glm::vec3{ 2.f, 0.01f, 2.f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ 0.4f, 0.4f, 0.4f };
		for (int i = 0; i < Floor.face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Floor.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Floor.v_color);
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
		" a / d : 조명의 위치를 중심의 구의 y축에 대하여 양/음 방향으로 회전한다" << endl <<
		" n / f: 조명이 가까워지기/멀어지기" << endl << // 이거 더 해야함 28번처럼
		" 화살표키 위 / 아래: 조명의 세기 높아지기/낮춰지기" << endl <<
		" c: 조명 색을 다른 색으로 바뀌도록 한다. 3종류의 다른 색을 적용해본다." << endl <<
		" m: 조명을 켜기/끄기" << endl <<
		// " p: 조명을 (1, 0, 0) (-1, 0, 0) (0, 0, 1) (0, 0, -1) 쪽의 위치로 옮긴다." << endl <<
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
	vertexSource = filetobuf("3d_vertex_v4_light.glsl");
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
	fragmentSource = filetobuf("light_fragment_v2.glsl");
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