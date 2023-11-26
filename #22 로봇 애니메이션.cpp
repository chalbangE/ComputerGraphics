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

float winSizex = 800, winSizey = 800;
GLuint vao;

std::vector <GLRect> Room;
std::vector <GLObj> Robot;
std::vector <GLObj> Block;
GLLine lineObj;
GLCamera Camera;

bool Back_Cull = true;
bool Omod = false, Walking = false, Jump_on = false;
GLchar Robot_Dis = 'w';
GLfloat Jump = 0.f;
glm::vec3 whirling{ 1.f, 0.f, 0.f };

static std::uniform_real_distribution<GLclampf> blockRd(-0.6f, 0.6f);

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#22 로봇");
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

	if (Back_Cull) {
		glEnable(GL_DEPTH_TEST);
	}

	lineObj.draw_prepare(PosLocation, "Pos");
	lineObj.draw_prepare(ColorLocation, "Color");
	lineObj.draw_prepare(WorldTransLocation, "World");
	lineObj.draw();

	for (int i = 0; i < Room.size(); ++i) {
		Room[i].draw_prepare(PosLocation, "Pos");
		Room[i].draw_prepare(ColorLocation, "Color");
		Room[i].draw_prepare(WorldTransLocation, "World");
		Room[i].draw("solid");
	}

	for (int i = 0; i < Robot.size(); ++i) {
		Robot[i].draw_prepare(PosLocation, "Pos");
		Robot[i].draw_prepare(ColorLocation, "Color");
		Robot[i].draw_prepare(WorldTransLocation, "World");
		Robot[i].draw("solid");
	}

	for (int i = 0; i < Block.size(); ++i) {
		Block[i].draw_prepare(PosLocation, "Pos");
		Block[i].draw_prepare(ColorLocation, "Color");
		Block[i].draw_prepare(WorldTransLocation, "World");
		Block[i].draw("solid");
	}

	if (Back_Cull) {
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
		glm::vec3 Robot_Location;
		lineObj.Update();
		for (int i = 0; i < Robot.size(); ++i)
			Robot[i].Update22();
		for (int i = 0; i < Room.size(); ++i)
			Room[i].Update();
		for (int i = 0; i < Block.size(); ++i)
			Block[i].Update();

		if (Omod) {
			if (Room[0].revolve_theta.y <= -120.f)
				Omod = false;

			Room[0].revolve_theta += glm::vec3{ 0.f, -1.f, 0.f };
			Room[1].revolve_theta += glm::vec3{ 0.f, 1.f, 0.f };
		}

		if (Walking) {
			switch (Robot_Dis)
			{
			case 'w': {
				for (int i = 0; i < Robot.size(); ++i) {
					Robot[i].pos += glm::vec3{ 0.f, 0.f, -0.003f };
					Robot[i].Ani_mid += glm::vec3{ 0.f, 0.f, -0.003f };
				}
				if (Robot[0].pos.z < -0.7f) {
					Robot_Dis = 's';
					for (int i = 0; i < Robot.size(); ++i) {
						Robot[i].rotate_theta = glm::vec3{ 0.f, 0.f, 0.f };
						Robot[i].revolve_theta.z = 0.f;
					}
				}
				break;
			}
			case 's': {
				for (int i = 0; i < Robot.size(); ++i) {
					Robot[i].pos += glm::vec3{ 0.f, 0.f, 0.003f };
					Robot[i].Ani_mid += glm::vec3{ 0.f, 0.f, 0.003f };
				}
				if (Robot[0].pos.z > 0.7f) {
					Robot_Dis = 'w';
					for (int i = 0; i < Robot.size(); ++i) {
						Robot[i].rotate_theta = glm::vec3{ 0.f, 180.f, 0.f };
						Robot[i].revolve_theta.z = 0.f;
					}
				}
				break;
			}
			case 'a': {
				for (int i = 0; i < Robot.size(); ++i) {
					Robot[i].pos += glm::vec3{ -0.003f, 0.f, 0.f };
					Robot[i].Ani_mid += glm::vec3{ -0.003f, 0.f, 0.f };
				}
				if (Robot[0].pos.x < -0.7f) {
					Robot_Dis = 'd';
					for (int i = 0; i < Robot.size(); ++i) {
						Robot[i].rotate_theta = glm::vec3{ 0.f, 90.f, 0.f };
						Robot[i].revolve_theta.x = 0.f;
					}
				}
				break;
			}
			case 'd': {
				for (int i = 0; i < Robot.size(); ++i) {
					Robot[i].pos += glm::vec3{ 0.003f, 0.f, 0.f };
					Robot[i].Ani_mid += glm::vec3{ 0.003f, 0.f, 0.f };
				}
				if (Robot[0].pos.x > 0.7f) {
					Robot_Dis = 'a';
					for (int i = 0; i < Robot.size(); ++i) {
						Robot[i].rotate_theta = glm::vec3{ 0.f, -90.f, 0.f };
						Robot[i].revolve_theta.x = 0.f;
					}
				}
				break;
			}
			}

			Robot_Location = Robot[0].pos;
			Robot_Location.y = Robot[3].pos.y - (Robot[3].scale.y / 2.f) + 0.01f;
			for (int i = 0; i < Block.size(); ++i) {
				if (Robot_Location.y < Block[i].pos.y + (Block[i].scale.y / 4.f)) {
					if ((Robot_Location.x >= Block[i].pos.x - (Block[i].scale.x / 2.f) && Robot_Location.x <= Block[i].pos.x + (Block[i].scale.x / 2.f))
						&& (Robot_Location.z >= Block[i].pos.z - (Block[i].scale.z / 2.f) && Robot_Location.z <= Block[i].pos.z + (Block[i].scale.z / 2.f))) {
						switch (Robot_Dis)
						{
						case 'w': {
							Robot_Dis = 's';
							for (int i = 0; i < Robot.size(); ++i) {
								Robot[i].rotate_theta = glm::vec3{ 0.f, 0.f, 0.f };
								Robot[i].revolve_theta.z = 0.f;
							}
							break;
						}
						case 's': {
							Robot_Dis = 'w';
							for (int i = 0; i < Robot.size(); ++i) {
								Robot[i].rotate_theta = glm::vec3{ 0.f, 180.f, 0.f };
								Robot[i].revolve_theta.z = 0.f;
							}
							break;
						}
						case 'a': {
							Robot_Dis = 'd';
							for (int i = 0; i < Robot.size(); ++i) {
								Robot[i].rotate_theta = glm::vec3{ 0.f, 90.f, 0.f };
								Robot[i].revolve_theta.x = 0.f;
							}
							break;
						}
						case 'd': {
							Robot_Dis = 'a';
							for (int i = 0; i < Robot.size(); ++i) {
								Robot[i].rotate_theta = glm::vec3{ 0.f, -90.f, 0.f };
								Robot[i].revolve_theta.x = 0.f;
							}
							break;
						}
						}
					}
				}
				else if (Robot_Location.y > Block[i].pos.y + (Block[i].scale.y / 4.f) && !Jump_on) {
					if ((Robot_Location.x >= Block[i].pos.x - (Block[i].scale.x / 2.f) && Robot_Location.x <= Block[i].pos.x + (Block[i].scale.x / 2.f))
						&& (Robot_Location.z >= Block[i].pos.z - (Block[i].scale.z / 2.f) && Robot_Location.z <= Block[i].pos.z + (Block[i].scale.z / 2.f))) {}
					else {
						Jump_on = true;
						Jump = -0.01f;
					}
				}
			}

			Robot[2].revolve_theta += whirling;
			Robot[3].revolve_theta -= whirling;
			Robot[4].revolve_theta += whirling;
			Robot[5].revolve_theta -= whirling;

			if (Robot[2].revolve_theta.x >= 45.f && whirling.x > 0)
				whirling *= -1;
			else if (Robot[2].revolve_theta.x <= -45.f && whirling.x < 0)
				whirling *= -1;
			
			if (Robot[2].revolve_theta.z >= 45.f && whirling.z > 0)
				whirling *= -1;
			else if (Robot[2].revolve_theta.z <= -45.f && whirling.z < 0)
				whirling *= -1;

			//std::cout << Robot[0].pos.z << std::endl;
		}

		if (Jump_on) {
			for (int i = 0; i < Robot.size(); ++i) {
				Robot[i].pos.y += Jump;
				Robot[i].Ani_mid.y += Jump;
			}
			Robot_Location = Robot[0].pos;
			Robot_Location.y = Robot[3].pos.y - (Robot[3].scale.y / 2.f);
			for (int i = 0; i < Block.size(); ++i) {
				if ((Robot_Location.x >= Block[i].pos.x - (Block[i].scale.x / 2.f) && Robot_Location.x <= Block[i].pos.x + (Block[i].scale.x / 2.f))
					&& (Robot_Location.z >= Block[i].pos.z - (Block[i].scale.z / 2.f) && Robot_Location.z <= Block[i].pos.z + (Block[i].scale.z / 2.f))) {
					if (Robot_Location.y + (2 * Jump) < Block[i].pos.y + (Block[i].scale.y / 4.f)) {
						std::cout << Robot_Location.y + (2 * Jump) << "\t" << Block[i].pos.y + (Block[i].scale.y / 4.f) << std::endl;
						for (int k = 0; k < Robot.size(); ++k) {
							Robot[k].pos.y += (Block[i].pos.y + (Block[i].scale.y / 2.f)) - Robot_Location.y;
							Robot[k].Ani_mid.y += (Block[i].pos.y + (Block[i].scale.y / 2.f)) - Robot_Location.y;
						}
						std::cout << Robot[3].pos.y + Robot[3].scale.y << "\t" << Block[i].pos.y + Block[i].scale.y << std::endl;
						Jump_on = false;
						break;
					}
				}
			}

			if (Robot_Location.y < -0.7f && Jump_on) {
				Jump_on = false;
				for (int i = 0; i < Robot.size(); ++i) {
					Robot[i].pos.y -= Robot_Location.y - -0.7f;
					Robot[i].Ani_mid.y -= Robot_Location.y - -0.7;
				}
			}
			Jump -= 0.001f;
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
		Camera.revolve_theta.y -= 5.f;
		break;
	}
	case 'Y': {
		Camera.revolve_theta.y += 5.f;
		break;
	}
	case 'o': 
	case 'O': {
		if (Omod) Omod = false;
		else Omod = true;
		break;
	}
	case 'c': 
	case 'C': {
		if (!Jump_on) {
			Jump_on = true;
			Jump = 0.03f;
		}
		break;
	}
	case 'w': 
	case 'W': {
		if (Walking && Robot_Dis == 'w') Walking = false;
		else {
			Walking = true;
			Robot_Dis = 'w';
			for (int i = 0; i < Robot.size(); ++i) {
				Robot[i].rotate_theta = glm::vec3{0.f, 180.f, 0.f};
				Robot[i].revolve_theta.z = 0.f;
			}
			whirling = glm::vec3{ 1.f, 0.f, 0.f };
		}
		break;
	}
	case 's': 
	case 'S': {
		if (Walking && Robot_Dis == 's') Walking = false;
		else {
			Walking = true;
			Robot_Dis = 's';
			for (int i = 0; i < Robot.size(); ++i) {
				Robot[i].rotate_theta = glm::vec3{0.f, 0.f, 0.f};
				Robot[i].revolve_theta.z = 0.f;
			}
			whirling = glm::vec3{ 1.f, 0.f, 0.f };
		}
		break;
	}
	case 'd': 
	case 'D': {
		if (Walking && Robot_Dis == 'd') Walking = false;
		else {
			Walking = true;
			Robot_Dis = 'd';
			for (int i = 0; i < Robot.size(); ++i) {
				Robot[i].rotate_theta = glm::vec3{0.f, 90.f, 0.f};
				Robot[i].revolve_theta.x = 0.f;
			}
			whirling = glm::vec3{ 0.f, 0.f, 1.f };
		}
		break;
	}
	case 'a': 
	case 'A': {
		if (Walking && Robot_Dis == 'a') Walking = false;
		else {
			Walking = true;
			Robot_Dis = 'a';
			for (int i = 0; i < Robot.size(); ++i) {
				Robot[i].rotate_theta = glm::vec3{0.f, -90.f, 0.f};
				Robot[i].revolve_theta.x = 0.f;
			}
			whirling = glm::vec3{ 0.f, 0.f, 1.f };
		}
		break;
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
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
		}
	}
	else {
	}
}

GLvoid Motion(int x, int y)
{

}

void Init()
{
	glBindVertexArray(vao);

	// 문제 출력
	{
		std::cout << "o/O: 앞면이 좌우로 열린다." << std::endl <<
			"w/a/s/d: 로봇이 앞/뒤/좌/우 방향으로 이동 방향을 바꿔서 걷는다." << std::endl <<
			"+/-: 걷는 속도가 빨라지거나/느려진다." << std::endl <<
			"스페이스바: 로봇이 제자리에서 점프한다." << std::endl <<
			"i: 모든 변환을 리셋하고 다시 시작" << std::endl <<
			"z/Z: 앞뒤로 이동" << std::endl <<
			"x/X: 좌우로 이동" << std::endl <<
			"y/Y: 카메라가 현재 위치에서 화면 중심 y축을 기준으로 공전" << std::endl;
	}

	// 카메라
	Camera.pos = glm::vec3{ 0.f, 0.f, 3.f };

	// 머가리
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Robot.emplace_back();

		if (inputFile.is_open())
			Robot.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Robot.back().pos = glm::vec3{ 0.f, -0.26f, 0.0f };
		Robot.back().scale = glm::vec3{ 0.07f, 0.07f, 0.07f };
		Robot.back().Object_mid = glm::vec3{ 0.f, -0.26f, 0.0f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < Robot.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
			color.emplace_back(a);
		}

		glGenBuffers(1, &Robot.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Robot.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 몸통
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Robot.emplace_back();

		if (inputFile.is_open())
			Robot.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Robot.back().pos = glm::vec3{ 0.f, -0.4f, 0.0f };
		Robot.back().scale = glm::vec3{ 0.15f, 0.2f, 0.05f };
		Robot.back().Object_mid = glm::vec3{ 0.f, -0.4f, 0.0f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < Robot.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
			color.emplace_back(a);
		}

		glGenBuffers(1, &Robot.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Robot.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 왼쪽 다리
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Robot.emplace_back();

		if (inputFile.is_open())
			Robot.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Robot.back().pos = glm::vec3{ -0.04f, -0.6f, 0.0f };
		Robot.back().Object_mid = glm::vec3{ -0.04f, -0.6f, 0.0f };
		Robot.back().Ani_mid = glm::vec3{ 0.0f, -0.5f, 0.0f };
		Robot.back().scale = glm::vec3{ 0.06f, 0.2f, 0.05f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < Robot.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
			color.emplace_back(a);
		}

		glGenBuffers(1, &Robot.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Robot.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 오른쪽 다리
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Robot.emplace_back();

		if (inputFile.is_open())
			Robot.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Robot.back().pos = glm::vec3{ 0.04f, -0.6f, 0.0f }; 
		Robot.back().Object_mid = glm::vec3{ 0.04f, -0.6f, 0.0f };
		Robot.back().Ani_mid = glm::vec3{ 0.f, -0.5f, 0.0f };
		Robot.back().scale = glm::vec3{ 0.06f, 0.2f, 0.05f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < Robot.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen) + 0.3f, colorRd(gen) + 0.3f, 1.f };
			color.emplace_back(a);
		}

		glGenBuffers(1, &Robot.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Robot.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 왼쪽 팔
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Robot.emplace_back();

		if (inputFile.is_open())
			Robot.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Robot.back().pos = glm::vec3{ -0.1f, -0.4f, 0.0f };
		Robot.back().Object_mid = glm::vec3{ -0.1f, -0.4f, 0.0f };
		Robot.back().Ani_mid = glm::vec3{ 0.f, -0.3f, 0.0f };
		Robot.back().scale = glm::vec3{ 0.05f, 0.2f, 0.05f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < Robot.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
			color.emplace_back(a);
		}

		glGenBuffers(1, &Robot.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Robot.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 오른쪽 팔
	{
		std::ifstream inputFile("./OBJ/cube.obj");
		Robot.emplace_back();

		if (inputFile.is_open())
			Robot.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Robot.back().pos = glm::vec3{ 0.1f, -0.4f, 0.0f };
		Robot.back().Object_mid = glm::vec3{ 0.1f, -0.4f, 0.0f };
		Robot.back().Ani_mid = glm::vec3{ 0.f, -0.3f, 0.0f };
		Robot.back().scale = glm::vec3{ 0.05f, 0.2f, 0.05f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < Robot.back().face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
			color.emplace_back(a);
		}

		glGenBuffers(1, &Robot.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Robot.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 코
	{
		std::ifstream inputFile("./OBJ/heart.obj");
		Robot.emplace_back();

		if (inputFile.is_open())
			Robot.back().objLoad(inputFile);
		else
			std::cerr << "Failed to obj file" << std::endl;

		Robot.back().pos = glm::vec3{ 0.f, -0.26f, 0.035f };
		Robot.back().scale = glm::vec3{ 0.003f, 0.003f, 0.003f };
		Robot.back().Object_mid = glm::vec3{ 0.f, -0.26f, 0.035 };

		std::vector<glm::vec3> color;
		for (int i = 0; i < Robot.back().face_cnt * 3; ++i) {
			glm::vec3 a{ 1.f, colorRd(gen), colorRd(gen) };
			color.emplace_back(a);
		}

		glGenBuffers(1, &Robot.back().v_color);
		glBindBuffer(GL_ARRAY_BUFFER, Robot.back().v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}

	// 장애물
	{
		for (int i = 0; i < 3; ++i) {
			std::ifstream inputFile("./OBJ/cube.obj");
			Block.emplace_back();

			if (inputFile.is_open())
				Block.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Block.back().pos = glm::vec3{ blockRd(rd), -0.65, blockRd(rd) };
			Block.back().scale = glm::vec3{ 0.2f, 0.15f, 0.2f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Block.back().face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), 1.f, colorRd(gen) };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Block.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Block.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}

	}

	// Room
	{
		{ // 왼쪽 문
			Room.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
			glm::vec3 cube[6]{
				{ -0.7f, -0.7f, 0.7f }, { 0.7f, -0.7f, 0.7f }, { -0.7f, 0.7f, 0.7f },
				{ -0.7f, 0.7f, 0.7f }, { 0.7f, -0.7f, 0.7f }, { 0.7f, 0.7f, 0.7f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
			Room.back().scale = glm::vec3{ 0.5f, 1.f, 1.f };
			Room.back().pos = glm::vec3{ -0.35f, 0.f, 0.f };
			Room.back().Anipos = glm::vec3{ -0.7f, 0.f, 0.7f };
		}
		{ // 오른쪽 문
			Room.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
			glm::vec3 cube[6]{
				{ -0.7f, -0.7f, 0.7f }, { 0.7f, -0.7f, 0.7f }, { -0.7f, 0.7f, 0.7f },
				{ -0.7f, 0.7f, 0.7f }, { 0.7f, -0.7f, 0.7f }, { 0.7f, 0.7f, 0.7f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
			Room.back().scale = glm::vec3{ 0.5f, 1.f, 1.f };
			Room.back().pos = glm::vec3{ 0.35f, 0.f, 0.f };
			Room.back().Anipos = glm::vec3{ 0.7f, 0.f, 0.7f };
		}
		{ // z축 기준 옆
			Room.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
			glm::vec3 cube[6]{
				{ 0.7f, -0.7f, -0.7f }, { -0.7f, -0.7f, -0.7f }, { 0.7f, 0.7f, -0.7f },
				{ 0.7f, 0.7f, -0.7f }, { -0.7f, -0.7f, -0.7f }, { -0.7f, 0.7f, -0.7f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
		{ // x축 기준 옆
			Room.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
			glm::vec3 cube[6]{
				{ 0.7f, -0.7f, 0.7f }, { 0.7f, -0.7f, -0.7f }, { 0.7f, 0.7f, 0.7f },
				{ 0.7f, 0.7f, 0.7f }, { 0.7f, -0.7f, -0.7f }, { 0.7f, 0.7f, -0.7f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
		{ // x축 기준 옆
			Room.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
			glm::vec3 cube[6]{
				{ -0.7f, -0.7f, -0.7f }, { -0.7f, -0.7f, 0.7f }, { -0.7f, 0.7f, -0.7f },
				{ -0.7f, 0.7f, -0.7f }, { -0.7f, -0.7f, 0.7f }, { -0.7f, 0.7f, 0.7f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
			Room.back().midpos.y = 0.7f;
		}
		{  // y축 기준 옆
			Room.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
			glm::vec3 cube[6]{
				{ -0.7f, 0.7f, 0.7f }, { 0.7f, 0.7f, 0.7f }, { -0.7f, 0.7f, -0.7f },
				{ -0.7f, 0.7f, -0.7f }, { 0.7f, 0.7f, 0.7f }, { 0.7f, 0.7f, -0.7f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
		{ // y축 기준 옆
			Room.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
			glm::vec3 cube[6]{
				{ -0.7f, -0.7f, 0.7f }, { -0.7f, -0.7f, -0.7f }, { 0.7f, -0.7f, 0.7f },
				{ -0.7f, -0.7f, -0.7f }, { 0.7f, -0.7f, -0.7f }, { 0.7f, -0.7f, 0.7f }
			};
			glGenBuffers(1, &Room.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, Room.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				Room.back().midpos += cube[i];
			Room.back().midpos /= 6.f;
		}
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