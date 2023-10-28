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

float winSizex = 1200, winSizey = 800;
GLuint vao;

GLRect Floor;
std::vector <GLObj> Tank;
GLLine lineObj;
GLCamera Camera;
glm::mat4 Camera_Mat;

bool Back_Cull = true, LbtDown = false;
bool Amod = false, mmod = false, Mmod = false, Fmod1 = false, Fmod = false, emod = false, Emod = false, Tmod = false;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#20 탱크");
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
	glViewport(0, 0, 800, 800);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	unsigned int WorldTransLocation = glGetUniformLocation(shaderProgramID, "World_trans");
	unsigned int CameraLocation = glGetUniformLocation(shaderProgramID, "Camera_trans");
	unsigned int ProjectionLocation = glGetUniformLocation(shaderProgramID, "Projection_trans");
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glEnableVertexAttribArray(WorldTransLocation);

	// 카메라 변환
	Camera.pos = glm::vec3{ 1.f, 1.f, 1.f };
	Camera.Update();

	// 투영 변환
	glm::mat4 Projection_Mat = glm::mat4(1.0f);
	Projection_Mat = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 50.f);
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, &Projection_Mat[0][0]);
	glUniformMatrix4fv(CameraLocation, 1, GL_FALSE, glm::value_ptr(Camera.Camera_Mat));

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (Back_Cull) {
		glEnable(GL_DEPTH_TEST);
	}

	// std::cout << (int)glIsEnabled(GL_CULL_FACE) << std::endl;
	Floor.draw_prepare(PosLocation, "Pos");
	Floor.draw_prepare(ColorLocation, "Color");
	Floor.draw_prepare(WorldTransLocation, "World");
	Floor.draw("solid");

	for (int i = 0; i < Tank.size(); ++i) {
		Tank[i].draw_prepare(PosLocation, "Pos");
		Tank[i].draw_prepare(ColorLocation, "Color");
		Tank[i].draw_prepare(WorldTransLocation, "World");
		Tank[i].draw("solid");
	}
			
	if (Back_Cull) {
		glDisable(GL_DEPTH_TEST);
	}

	{
		glViewport(800, 400, 400, 400);
		// 카메라 변환
		Camera_Mat = glm::lookAt(glm::vec3{ 0.f, 1.f, 0.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 0.f, 1.f });
		glUniformMatrix4fv(CameraLocation, 1, GL_FALSE, glm::value_ptr(Camera_Mat));

		// 투영 변환
		Projection_Mat = glm::mat4(1.0f);
		Projection_Mat = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 50.f);
		glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, &Projection_Mat[0][0]);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (Back_Cull) {
			glEnable(GL_DEPTH_TEST);
		}

		Floor.draw_prepare(PosLocation, "Pos");
		Floor.draw_prepare(ColorLocation, "Color");
		Floor.draw_prepare(WorldTransLocation, "World");
		Floor.draw("solid");

		for (int i = 0; i < Tank.size(); ++i) {
			Tank[i].draw_prepare(PosLocation, "Pos");
			Tank[i].draw_prepare(ColorLocation, "Color");
			Tank[i].draw_prepare(WorldTransLocation, "World");
			Tank[i].draw("solid");
		}

		if (Back_Cull) {
			glDisable(GL_DEPTH_TEST);
		}
	}

	{
		glViewport(800, 0, 400, 400);
		// 카메라 변환
		Camera_Mat = glm::lookAt(glm::vec3{ 0.f, 0.f, 1.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f });
		glUniformMatrix4fv(CameraLocation, 1, GL_FALSE, glm::value_ptr(Camera_Mat));

		// 투영 변환
		Projection_Mat = glm::mat4(1.0f);
		Projection_Mat = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 50.f);
		glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, &Projection_Mat[0][0]);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (Back_Cull) {
			glEnable(GL_DEPTH_TEST);
		}

		// std::cout << (int)glIsEnabled(GL_CULL_FACE) << std::endl;
		Floor.draw_prepare(PosLocation, "Pos");
		Floor.draw_prepare(ColorLocation, "Color");
		Floor.draw_prepare(WorldTransLocation, "World");
		Floor.draw("solid");

		for (int i = 0; i < Tank.size(); ++i) {
			Tank[i].draw_prepare(PosLocation, "Pos");
			Tank[i].draw_prepare(ColorLocation, "Color");
			Tank[i].draw_prepare(WorldTransLocation, "World");
			Tank[i].draw("solid");
		}

		if (Back_Cull) {
			glDisable(GL_DEPTH_TEST);
		}
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
		if (Amod) {
			Camera.revolve_theta.y += 1.f;
		}

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

		if (Fmod1) {
			Tank[2].revolve_theta.y -= 1.f;
			Tank[3].revolve_theta.y += 1.f;
		}
		else if (Fmod) {
			Tank[2].revolve_theta.y += 1.f;
			Tank[3].revolve_theta.y -= 1.f;
		}					

		if (emod) {
			if (Tank[2].revolve_theta.y > 0.f) {
				Tank[2].revolve_theta.y -= 1.f;
				Tank[3].revolve_theta.y += 1.f;
			}
			else if (Tank[2].revolve_theta.y < 0.f) {
				Tank[2].revolve_theta.y += 1.f;
				Tank[3].revolve_theta.y -= 1.f;
			}
			else if (Tank[2].Object_mid.z > 0.f) {
				Tank[2].Object_mid.z -= 0.005f;
				Tank[3].Object_mid.z += 0.005f;
				Tank[2].pos.z -= 0.005f;
				Tank[3].pos.z += 0.005f;
			}
		}
		else if (Emod) {
			if (Tank[2].revolve_theta.y > 0.f) {
				Tank[2].revolve_theta.y -= 1.f;
				Tank[3].revolve_theta.y += 1.f;
			}
			else if (Tank[2].revolve_theta.y < 0.f) {
				Tank[2].revolve_theta.y += 1.f;
				Tank[3].revolve_theta.y -= 1.f;
			}
			else if (Tank[2].Object_mid.z < 0.05f) {
				Tank[2].Object_mid.z += 0.005f;
				Tank[3].Object_mid.z -= 0.005f;
				Tank[2].pos.z += 0.005f;
				Tank[3].pos.z -= 0.005f;
			}
		}				

		else if (Tmod) {
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
	case 'z': {
		Camera.pos.z -= 0.05f;
		break;
	}
	case 'Z': {
		Camera.pos.z += 0.05f;
		break;
	}
	case 'x': {
		Camera.pos.x -= 0.05f;
		break;
	}
	case 'X': {
		Camera.pos.x += 0.05f;
		break;
	}
	case 'y': {
		Camera.rotate_theta.y -= 5.f;
		break;
	}
	case 'Y': {
		Camera.rotate_theta.y += 5.f;
		break;
	}
	case 'r': {
		Camera.revolve_theta.y -= 5.f;
		break;
	}
	case 'R': {
		Camera.revolve_theta.y += 5.f;
		break;
	}
	case 'a': {
		if (!Amod) Amod = true;
		else Amod = false;
		break;
	}
	case 'q':
	case 'Q': {
		exit(3);
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
	case 'M': {
		if (!Mmod) {
			Mmod = true;
			mmod = false;
		}
		else Mmod = false;
		break;
	}
	case 'f': {
		if (!Fmod1) {
			Fmod1 = true;
			Fmod = false;
		}
		else Fmod1 = false;
		break;
	}
	case 'F': {
		if (!Fmod) {
			Fmod = true;
			Fmod1 = false;
		}
		else Fmod = false;
		break;
	}
	case 'e': {
		if (!emod) {
			emod = true;
			Emod = false;
		}
		else emod = false;
		break;
	}
	case 'E': {
		if (!Emod) {
			Emod = true;
			emod = false;
		}
		else Emod = false;
		break;
	}
	case 't': 
	case 'T': {
		if (!Tmod) Tmod = true;
		else Tmod = false;
		break;
	}
	case 'c': 
	case 'C': {	
		Tank.clear();
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
			for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
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
			for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
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
			for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
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
			for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
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
			for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
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
			for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Tank.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Tank.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
	}
	case 's': 
	case 'S': {
		Amod = mmod = Mmod = Fmod1 = Fmod = emod = Emod = Tmod = false;
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			LbtDown = true;
		}
	}
	else {
		LbtDown = false;
	}
}

GLvoid Motion(int x, int y)
{
	//if (LbtDown) {
	//	y_angle = x - ox;
	//	x_angle = y - oy;
	//	x_angle += pre_x_angle;
	//	y_angle += pre_y_angle;

	//	y_angle /= 2;
	//	x_angle /= 2;
	//}
	//glutPostRedisplay();
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
			"f / F : 포신이 y축에 대하여 양 / 음 방향으로 회전하는데, 두 포신이 서로 반대방향으로 회전한다.다시 누르면 멈춘다." << std::endl <<
			"e / E : 2개 포신이 조금씩 이동해서 한 개가 된다 / 다시 제자리로 이동해서 2개가 된다." << std::endl <<
			"포신이 회전되어 있다면, 원래 자리까지 회전된 후 이동한다." << std::endl <<
			"t / T : 크레인의 맨 위 2개의 팔이 z축에 대하여 양 / 음 방향으로 서로 반대방향으로 회전한다.다시 누르면 멈춘다." << std::endl <<
			"z / Z : 카메라가 z축 양 / 음 방향으로 이동" << std::endl <<
			"x / X : 카메라가 x축 양 / 음 방향으로 이동" << std::endl <<
			"y / Y : 카메라 기준 y축에 대하여 회전" << std::endl <<
			"r / R : 화면의 중심의 y축에 대하여 카메라가 회전(중점에 대하여 공전)" << std::endl <<
			"a / A : r 명령어와 같이 화면의 중심의 축에 대하여 카메라가 회전하는 애니메이션을 진행한다 / 멈춘다." << std::endl <<
			"s / S : 모든 움직임 멈추기" << std::endl <<
			"c / C : 모든 움직임이 초기화된다." << std::endl <<
			"Q : 프로그램 종료하기" << std::endl;
	}

	// 카메라
	{
		Camera.pos = glm::vec3{ 1.f, 1.f, 1.f };
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
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
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
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
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
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
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
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
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
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
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
		for (int i = 0; i < Tank.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
			color.emplace_back(a);
		}

		glGenBuffers(1, &Tank.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Tank.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}

	// Floor
	{
		glm::vec3 pos[6]{
			{ -1.f, 0.f, 1.f }, { 1.f, 0.f, 1.f }, { -1.f, 0.f, -1.f },
			{ -1.f, 0.f, -1.f }, { 1.f, 0.f, 1.f }, { 1.f, 0.f, -1.f }
		};

		glGenBuffers(1, &Floor.v_pos);
		glBindBuffer(GL_ARRAY_BUFFER, Floor.v_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

		glm::vec3 color[6];
		color[0] = { 188.f / 255.f, 229.f / 255.f, 92.f / 255.f };
		for (int i = 1; i < 6; ++i) {
			color[i] = color[0];
		}

		glGenBuffers(1, &Floor.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Floor.v_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
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