#include "stdafx.h"
#include "GLObj.h"
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

float winSizex = 800, winSizey = 800;
GLuint vao;

GLObj Floor;
std::vector <GLObj> Tank;
GLLine lineObj;
GLLight Light;
GLCamera Camera;
glm::mat4 Camera_Mat;

bool Amod = false, mmod = false, Mmod = false, Tmod = false;
bool Lbt = false;
glm::vec3 click_mouse{};
glm::vec3 Camera_revolve_plus{ 0.f, 0.f, 0.f };
glm::vec3 Light_revolve_plus{ 0.f, 0.f, 0.f };

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#27 탱크 조명");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Init();
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
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

	Floor.draw_prepare(PosLocation, "Pos");
	Floor.draw_prepare(ColorLocation, "Color");
	Floor.draw_prepare(NormalLocation, "Normal");
	Floor.draw_prepare(WorldTransLocation, "World");
	Floor.draw("solid");

	for (int i = 0; i < Tank.size(); ++i) {
		Tank[i].draw_prepare(PosLocation, "Pos");
		Tank[i].draw_prepare(ColorLocation, "Color");
		Tank[i].draw_prepare(NormalLocation, "Normal");
		Tank[i].draw_prepare(WorldTransLocation, "World");
		Tank[i].draw("solid");
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

	glutSwapBuffers();
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		Camera.revolve_theta += Camera_revolve_plus;
		Light.revolve_theta += Light_revolve_plus;

		if (mmod) {
			Tank[1].rotate_theta.y += 1.f;
			Tank[4].revolve_theta.y += 1.f;
			Tank[5].revolve_theta.y += 1.f;
		}
		else if (Mmod) {
			Tank[1].rotate_theta.y -= 1.f;
			Tank[4].revolve_theta.y -= 1.f;
			Tank[5].revolve_theta.y -= 1.f;
		}

		if (Tmod) {
			static glm::vec3 TmodTheta{ 0.f, 0.f, 1.f };
			Tank[4].revolve_theta += TmodTheta;
			Tank[5].revolve_theta -= TmodTheta;
			if (Tank[4].revolve_theta.z == 30.f)
				TmodTheta *= -1;
			else if (Tank[4].revolve_theta.z == -30.f)
				TmodTheta *= -1;
		}

		Floor.Update();
		for (int i = 0; i < Tank.size(); ++i)
			Tank[i].Update20();
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
	case 'i':
	case 'I': {
		Camera.pos.z -= 0.05f;
		break;
	}
	case 'k':
	case 'K': {
		Camera.pos.z += 0.05f;
		break;
	}
	case 'j':
	case 'J': {
		Camera.pos.x -= 0.05f;
		break;
	}
	case 'l':
	case 'L': {
		Camera.pos.x += 0.05f;
		break;
	}
	case 'a':
	case 'A': {
		Camera_revolve_plus = glm::vec3{ 0.f, 1.f, 0.f };
		break;
	}
	case 'd':
	case 'D': {
		Camera_revolve_plus = glm::vec3{ 0.f, -1.f, 0.f };
		break;
	}
	case 'y': {
		Light_revolve_plus = glm::vec3{ 0.f, 1.f, 0.f };
		break;
	}
	case 'Y': {
		Light_revolve_plus = glm::vec3{ 0.f, -1.f, 0.f };
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
		exit(3);
		break;
	}
	case 'f':
	case 'F': {
		if (Light.L_color.r == 1.f) Light.L_color = glm::vec3{ 0.1f, 0.1f, 0.1f };
		else Light.L_color = glm::vec3{ 1.f, 1.f, 1.f };
		break;
	}
	case 'b': {
		for (int i = 0; i < Tank.size(); ++i) {
			Tank[i].pos.x -= 0.02;
		}
		break;
	}
	case 'B': {
		for (int i = 0; i < Tank.size(); ++i) {
			Tank[i].pos.x += 0.02;
		}
		break;
	}
	case 'm': {
		if (!mmod) {
			mmod = true;
			Mmod = false;
		}
		else mmod = false;
		break;
	}
	case 't':
	case 'T': {
		if (!Tmod) Tmod = true;
		else Tmod = false;
		break;
	}
	case 'M': {
		if (!Mmod) {
			Mmod = true;
			mmod = false;
		}
		else Mmod = false;
		break;
	}
	case 's':
	case 'S': {
		Camera_revolve_plus = glm::vec3{ 0.f, 0.f, 0.f };
		Light_revolve_plus = glm::vec3{ 0.f, 0.f, 0.f };
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

	// 문제 출력
	{
		std::cout << "b / B: 크레인의 아래 몸체가 x축 방향으로 양 / 음 방향으로 이동한다.다시 누르면 멈춘다." << std::endl <<
			"아래 몸체가 이동하면 중앙 몸체와 맨 위의 팔은 같이 이동한다." << std::endl <<
			"m / M : 크레인의 중앙 몸체가 y축에 대하여 양 / 음 방향으로 회전한다.다시 누르면 멈춘다." << std::endl <<
			"중앙 몸체가 회전하면 맨 위의 팔도 같이 회전한다." << std::endl <<
			"f / F : 주변 조명을 켜기/끄기" << std::endl <<
			"i / j / k / l : 카메라 위치 이동" << std::endl <<
			"y / Y : 조명이 화면 중심의 y축 기준으로 양/음 방향으로 회전하기 (공전)" << std::endl <<
			"a / D : 화면의 중심의 y축에 대하여 카메라가 회전(중점에 대하여 공전)" << std::endl <<
			"c / C : 조명 색을 다른 색으로 바뀌도록 한다. 3종류의 다른 색을 적용해본다." << std::endl <<
			"t / T : 귀여운거" << std::endl <<
			"s / S : 모든 움직임 멈추기" << std::endl <<
			"Q : 프로그램 종료하기" << std::endl;
	}

	// 카메라
	Camera.pos = glm::vec3{ 1.f, 1.f, 3.f };

	// Light
	{
		std::ifstream inputFile("./OBJ/sphere.obj");

		if (inputFile.is_open())
			Light.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Light.pos = glm::vec3{ 1.f, 0.7f, -1.f };
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

	// 제일 밑 네모 0
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Tank.emplace_back();

		if (inputFile.is_open())
			Tank.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Tank.back().pos = glm::vec3{ 0.f, 0.05f, 0.0f };
		Tank.back().scale = glm::vec3{ 0.2f, 0.1f, 0.2f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Tank.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Tank.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 머리 네모 1
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Tank.emplace_back();

		if (inputFile.is_open())
			Tank.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Tank.back().pos = glm::vec3{ 0.f, 0.125f, 0.0f };
		Tank.back().Object_mid = glm::vec3{ 0.f, 0.125f, 0.0f };
		Tank.back().scale = glm::vec3{ 0.1f, 0.05f, 0.1f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Tank.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Tank.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 앞대포 왼쪽 네모 2
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Tank.emplace_back();

		if (inputFile.is_open())
			Tank.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Tank.back().pos = glm::vec3{ 0.1f, 0.05f, 0.05f };
		Tank.back().Object_mid = glm::vec3{ 0.1f, 0.05f, 0.05f };
		Tank.back().scale = glm::vec3{ 0.2f, 0.03f, 0.03f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Tank.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Tank.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 앞대포 오른쪽 네모 3
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Tank.emplace_back();

		if (inputFile.is_open())
			Tank.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Tank.back().pos = glm::vec3{ 0.1f, 0.05f, -0.05f };
		Tank.back().Object_mid = glm::vec3{ 0.1f, 0.05f, -0.05f };
		Tank.back().scale = glm::vec3{ 0.2f, 0.03f, 0.03f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Tank.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Tank.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 왼쪽 더듬이 4
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Tank.emplace_back();

		if (inputFile.is_open())
			Tank.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Tank.back().pos = glm::vec3{ 0.f, 0.175f, 0.03f };
		Tank.back().Object_mid = glm::vec3{ 0.f, 0.035f, 0.03f };
		Tank.back().scale = glm::vec3{ 0.015f, 0.07f, 0.015f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Tank.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Tank.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 오른쪽 더듬이 5
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Tank.emplace_back();

		if (inputFile.is_open())
			Tank.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Tank.back().pos = glm::vec3{ 0.f, 0.175f, -0.03f };
		Tank.back().Object_mid = glm::vec3{ 0.f, 0.035f, -0.03f };
		Tank.back().scale = glm::vec3{ 0.015f, 0.07f, 0.015f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Tank.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Tank.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}

	// Floor
	{
		std::ifstream inputFile("./OBJ/cube.obj");

		if (inputFile.is_open())
			Floor.objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Floor.pos = glm::vec3{ 0.f, 0.f, 0.f };
		Floor.scale = glm::vec3{ 2.f, 0.001f, 2.f };

		std::vector<glm::vec3> color;
		glm::vec3 a{ 188.f / 255.f, 229.f / 255.f, 92.f / 255.f };
		for (int i = 0; i < Floor.face_cnt * 3; ++i) {
			color.emplace_back(a);
		}

		glGenBuffers(1, &Floor.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Floor.v_color);
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

		//lineObj.rotate_theta.x = 30.0f;
		//lineObj.rotate_theta.y = -30.0f;
	}
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	//glViewport(300, 300, 300, 300);
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