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

float winSizex = 800, winSizey = 800;
GLuint vao;
class Collision_box {
public:
	glm::vec3 pos;
	glm::vec3 size;
};

class Robot : public GLObj {
public:
	std::vector <GLObj> body;
	Collision_box collision_box;

	Robot() {};
	Robot(glm::vec3 size) {
		scale = size;

		// 머가리
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			body.emplace_back();

			if (inputFile.is_open())
				body.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			body.back().pos = glm::vec3{ 0.f, 0.14f, 0.0f };
			body.back().scale = glm::vec3{ 0.07f, 0.07f, 0.07f };
			body.back().Object_mid = glm::vec3{ 0.f, 0.14f, 0.0f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < body.back().face_cnt * 3; ++i) {
				glm::vec3 a{ 0.784, 0.537, 1.f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &body.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, body.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 몸통
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			body.emplace_back();

			if (inputFile.is_open())
				body.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			body.back().pos = glm::vec3{ 0.f, 0.f, 0.0f };
			body.back().scale = glm::vec3{ 0.15f, 0.2f, 0.05f };
			body.back().Object_mid = glm::vec3{ 0.f, 0.f, 0.0f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < body.back().face_cnt * 3; ++i) {
				glm::vec3 a{ 0.466, 0.345, 0.647 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &body.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, body.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 왼쪽 다리
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			body.emplace_back();

			if (inputFile.is_open())
				body.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			body.back().pos = glm::vec3{ -0.04f, -0.2f, 0.0f };
			body.back().Object_mid = glm::vec3{ -0.04f, -0.2f, 0.0f };
			body.back().Ani_mid = glm::vec3{ 0.0f, -0.1f, 0.0f };
			body.back().scale = glm::vec3{ 0.06f, 0.2f, 0.05f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < body.back().face_cnt * 3; ++i) {
				glm::vec3 a{ 0.566, 0.345, 0.647 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &body.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, body.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 오른쪽 다리
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			body.emplace_back();

			if (inputFile.is_open())
				body.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			body.back().pos = glm::vec3{ 0.04f, -0.2f, 0.0f };
			body.back().Object_mid = glm::vec3{ 0.04f, -0.2f, 0.0f };
			body.back().Ani_mid = glm::vec3{ 0.f, -0.1f, 0.0f };
			body.back().scale = glm::vec3{ 0.06f, 0.2f, 0.05f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < body.back().face_cnt * 3; ++i) {
				glm::vec3 a{ 0.566, 0.345, 0.647 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &body.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, body.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 왼쪽 팔
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			body.emplace_back();

			if (inputFile.is_open())
				body.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			body.back().pos = glm::vec3{ -0.1f, 0.f, 0.0f };
			body.back().Object_mid = glm::vec3{ -0.1f, 0.f, 0.0f };
			body.back().Ani_mid = glm::vec3{ 0.f, 0.1f, 0.0f };
			body.back().scale = glm::vec3{ 0.05f, 0.2f, 0.05f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < body.back().face_cnt * 3; ++i) {
				glm::vec3 a{ 0.566, 0.345, 0.647 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &body.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, body.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 오른쪽 팔
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			body.emplace_back();

			if (inputFile.is_open())
				body.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			body.back().pos = glm::vec3{ 0.1f, 0.f, 0.0f };
			body.back().Object_mid = glm::vec3{ 0.1f, 0.f, 0.0f };
			body.back().Ani_mid = glm::vec3{ 0.f, 0.1f, 0.0f };
			body.back().scale = glm::vec3{ 0.05f, 0.2f, 0.05f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < body.back().face_cnt * 3; ++i) {
				glm::vec3 a{ 0.566, 0.345, 0.647 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &body.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, body.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 코
		{
			std::ifstream inputFile("./OBJ/heart.obj");
			body.emplace_back();

			if (inputFile.is_open())
				body.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			body.back().pos = glm::vec3{ 0.f, 0.14f, 0.035f };
			body.back().scale = glm::vec3{ 0.003f, 0.003f, 0.003f };
			body.back().Object_mid = glm::vec3{ 0.f, 0.14f, 0.035 };

			std::vector<glm::vec3> color;
			for (int i = 0; i < body.back().face_cnt * 3; ++i) {
				glm::vec3 a{ 0.f, 0.f, 0.f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &body.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, body.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}

		for (int i = 0; i < body.size(); ++i) {
			if (max.x < body[i].pos.x + (body[i].max.x * body[i].scale.x))
				max.x = body[i].pos.x + (body[i].max.x * body[i].scale.x);
			if (max.y < body[i].pos.y + (body[i].max.y * body[i].scale.y))
				max.y = body[i].pos.y + (body[i].max.y * body[i].scale.y);
			if (max.z < body[i].pos.z + (body[i].max.z * body[i].scale.z))
				max.z = body[i].pos.z + (body[i].max.z * body[i].scale.z);

			if (min.x > body[i].pos.x + (body[i].min.x * body[i].scale.x))
				min.x = body[i].pos.x + (body[i].min.x * body[i].scale.x);
			if (min.y > body[i].pos.y + (body[i].min.y * body[i].scale.y))
				min.y = body[i].pos.y + (body[i].min.y * body[i].scale.y);
			if (min.z > body[i].pos.z + (body[i].min.z * body[i].scale.z))
				min.z = body[i].pos.z + (body[i].min.z * body[i].scale.z);
		}

		max *= size;
		min *= size;
		pos = (max + min) / 2.f;
	}

	void update_collision_box() {
		collision_box.pos = pos;
		collision_box.size = abs((max - min) / 3.f);
		collision_box.size.y = abs((max.y - min.y) / 1.6f);
	}

	void Update24()
	{
		World_mat = glm::mat4(1.0);

		World_mat = glm::translate(World_mat, pos);
		World_mat = glm::scale(World_mat, scale);
	}

	void Update24_mini(glm::vec3 robot)
	{
		World_mat = glm::mat4(1.0);

		World_mat = glm::translate(World_mat, pos);
		World_mat = glm::scale(World_mat, scale);
	}
};

bool CheckCollision(const Collision_box& a, const Collision_box& b) {
	return (std::abs(a.pos.x - b.pos.x) < (a.size.x + b.size.x) &&
		std::abs(a.pos.y - b.pos.y) < (a.size.y + b.size.y) &&
		std::abs(a.pos.z - b.pos.z) < (a.size.z + b.size.z));
}

Robot robot;
std::vector <Robot> minirobot;
std::vector <GLObj> Floor;
std::vector <GLObj> Pillar;
std::vector <Collision_box> Floor_Collision_box;
GLLine lineObj;
GLCamera Camera;
GLObj strawberry;

bool Back_Cull = true;
bool Omod = false, Walking = false, Jump_on = false, Floor_on = false, Lbt = false;
GLchar Robot_Dis = 'w';
GLfloat Jump = 0.001f;
glm::vec3 whirling{ 1.f, 0.f, 0.f };
glm::vec3 gravity{ 0.f, -0.001f, 0.f };
glm::vec3 click_mouse{ 0.f, 0.f, 0.f };
vector <int> Floor_type;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#24 로봇 애니메이션 2");
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

	lineObj.Update();
	lineObj.draw_prepare(PosLocation, "Pos");
	lineObj.draw_prepare(ColorLocation, "Color");
	lineObj.draw_prepare(WorldTransLocation, "World");
	lineObj.draw();

	for (int i = 0; i < Floor.size(); ++i) {
		Floor[i].Update();
		Floor[i].draw_prepare(PosLocation, "Pos");
		Floor[i].draw_prepare(ColorLocation, "Color");
		Floor[i].draw_prepare(WorldTransLocation, "World");
		Floor[i].draw("solid");
	}

	for (int i = 0; i < Pillar.size(); ++i) {
		Pillar[i].Update();
		Pillar[i].draw_prepare(PosLocation, "Pos");
		Pillar[i].draw_prepare(ColorLocation, "Color");
		Pillar[i].draw_prepare(WorldTransLocation, "World");
		Pillar[i].draw("solid");
	}

	robot.Update24();
	for (int i = 0; i < robot.body.size(); ++i) {
		robot.body[i].Update22();
		robot.body[i].draw_prepare(PosLocation, "Pos");
		robot.body[i].draw_prepare(ColorLocation, "Color");
		robot.body[i].World_mat = robot.World_mat * robot.body[i].World_mat;
		robot.body[i].draw_prepare(WorldTransLocation, "World");
		robot.body[i].draw("solid");
	}
	
	for (int k = 0; k < minirobot.size(); ++k) {
		minirobot[k].Update24_mini(robot.pos);
		for (int i = 0; i < robot.body.size(); ++i) {
			minirobot[k].body[i].Update22();
			minirobot[k].body[i].draw_prepare(PosLocation, "Pos");
			minirobot[k].body[i].draw_prepare(ColorLocation, "Color");
			minirobot[k].body[i].World_mat = minirobot[k].World_mat * minirobot[k].body[i].World_mat;
			minirobot[k].body[i].draw_prepare(WorldTransLocation, "World");
			minirobot[k].body[i].draw("solid");
		}
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
		if (Walking) {
			switch (Robot_Dis)
			{
			case 'w': {
				robot.pos += glm::vec3{ 0.f, 0.f, -0.003f };
				robot.update_collision_box();
				for (int k = 0; k < Floor_Collision_box.size(); ++k) {
					if (CheckCollision(robot.collision_box, Floor_Collision_box[k])) {
						Robot_Dis = 's';
						robot.pos += glm::vec3{ 0.f, 0.f, 0.003f };
						for (int i = 0; i < robot.body.size(); ++i) {
							robot.body[i].rotate_theta = glm::vec3{ 0.f, 0.f, 0.f };
							robot.body[i].revolve_theta.z = 0.f;
						}
					}
				}
				break;
			}
			case 's': {
				robot.pos += glm::vec3{ 0.f, 0.f, 0.003f };
				robot.update_collision_box();
				for (int k = 0; k < Floor_Collision_box.size(); ++k) {
					if (CheckCollision(robot.collision_box, Floor_Collision_box[k])) {
						Robot_Dis = 'w';
						robot.pos += glm::vec3{ 0.f, 0.f, -0.003f };
						for (int i = 0; i < robot.body.size(); ++i) {
							robot.body[i].rotate_theta = glm::vec3{ 0.f, 180.f, 0.f };
							robot.body[i].revolve_theta.z = 0.f;
						}
					}
				}
				break;
			}
			case 'a': {
				robot.update_collision_box();
				Collision_box Fu_collision_box = robot.collision_box;
				Fu_collision_box.pos += glm::vec3{ -0.003f, 0.f, 0.f };
				for (int k = 0; k < Floor_Collision_box.size(); ++k) {
					if (CheckCollision(Fu_collision_box, Floor_Collision_box[k])) {
						Robot_Dis = 'd';
						for (int i = 0; i < robot.body.size(); ++i) {
							robot.body[i].rotate_theta = glm::vec3{ 0.f, 90.f, 0.f };
							robot.body[i].revolve_theta.x = 0.f;
						}
					}
				}
				if (Robot_Dis == 'a')
					robot.pos += glm::vec3{ -0.003f, 0.f, 0.f };
				break;
			}
			case 'd': {
				robot.pos += glm::vec3{ 0.003f, 0.f, 0.f };
				robot.update_collision_box();
				for (int k = 0; k < Floor_Collision_box.size(); ++k) {
					if (CheckCollision(robot.collision_box, Floor_Collision_box[k])) {
						Robot_Dis = 'a';
						robot.pos += glm::vec3{ -0.003f, 0.f, 0.f };
						for (int i = 0; i < robot.body.size(); ++i) {
							robot.body[i].rotate_theta = glm::vec3{ 0.f, -90.f, 0.f };
							robot.body[i].revolve_theta.x = 0.f;
						}
					}
				}
				break;
			}
			}

			robot.body[2].revolve_theta += whirling;
			robot.body[3].revolve_theta -= whirling;
			robot.body[4].revolve_theta += whirling;
			robot.body[5].revolve_theta -= whirling;

			if (robot.body[2].revolve_theta.x >= 45.f && whirling.x > 0)
				whirling *= -1;
			else if (robot.body[2].revolve_theta.x <= -45.f && whirling.x < 0)
				whirling *= -1;

			if (robot.body[2].revolve_theta.z >= 45.f && whirling.z > 0)
				whirling *= -1;
			else if (robot.body[2].revolve_theta.z <= -45.f && whirling.z < 0)
				whirling *= -1;
		}

		Floor_on = false;
		robot.update_collision_box();
		Collision_box Fu_collision_box = robot.collision_box;
		Fu_collision_box.pos.y += robot.velocity.y;

		for (int k = 0; k < Floor_Collision_box.size(); ++k) {
			if (CheckCollision(Fu_collision_box, Floor_Collision_box[k])) {
				robot.velocity.y = 0.f;

				if (Floor_type[k] == 1 && Floor_Collision_box[k].pos.y > -0.79f) {
					Floor_Collision_box[k].pos.y += -0.01f;
					Floor[k].pos.y += -0.01f;
				}
				else if (Floor_type[k] == 2 && Floor_Collision_box[k].pos.y > -0.99f) {
					Floor_Collision_box[k].pos.y += -0.01f;
					Floor[k].pos.y += -0.01f;
				}
			}

			for (int i = 0; i < minirobot.size(); ++i) {
				minirobot[i].update_collision_box();
				Collision_box Fu_mini_collision_box = minirobot[i].collision_box;
				Fu_mini_collision_box.pos.y += minirobot[i].velocity.y;
				if (CheckCollision(Fu_mini_collision_box, Floor_Collision_box[k])) {
					minirobot[i].velocity.y = 0.f;
				}
			}
		}

		robot.pos += robot.velocity;
		robot.velocity += gravity;

		for (int i = 0; i < minirobot.size(); ++i) {
			minirobot[i].pos.x = robot.pos.x;
			minirobot[i].pos.z = robot.pos.z;
			if (Robot_Dis == 's')
				minirobot[i].pos.z = (robot.pos.z + ((i + 1) * (-0.15f)));
			else if (Robot_Dis == 'w')
				minirobot[i].pos.z = (robot.pos.z + ((i + 1) * (0.15f)));
			else if (Robot_Dis == 'a')
				minirobot[i].pos.x = (robot.pos.x + ((i + 1) * (0.15f)));
			else if (Robot_Dis == 'd')
				minirobot[i].pos.x = (robot.pos.x + ((i + 1) * (-0.15f)));

			for (int k = 0; k < robot.body.size(); ++k) 
				minirobot[i].body[k] = robot.body[k];
			minirobot[i].pos += minirobot[i].velocity;
			minirobot[i].velocity += gravity;
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
		robot.velocity.y = 0.03f;
		break;
	}
	case 'f':
	case 'F': {
		for (int i = 0; i < minirobot.size(); ++i)
			minirobot[i].velocity.y = 0.02f;
		break;
	}
	case 't':
	case 'T': {
		minirobot.emplace_back(Robot({ 0.35f, 0.35f, 0.35f }));
		if (minirobot.size() > 3) {
			minirobot.clear();
		}
		break;
	}
	case 'w':
	case 'W': {
		if (Walking && Robot_Dis == 'w') Walking = false;
		else {
			Walking = true;
			Robot_Dis = 'w';
			for (int i = 0; i < robot.body.size(); ++i) {
				robot.body[i].rotate_theta = glm::vec3{ 0.f, 180.f, 0.f };
				robot.body[i].revolve_theta.z = 0.f;
			}
			for (int i = 0; i < minirobot.size(); ++i) {
				minirobot[i].revolve_theta = glm::vec3{ 0.f, 180.f, 0.f };
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
			for (int i = 0; i < robot.body.size(); ++i) {
				robot.body[i].rotate_theta = glm::vec3{ 0.f, 0.f, 0.f };
				robot.body[i].revolve_theta.z = 0.f;
			}
			for (int i = 0; i < minirobot.size(); ++i) {
				minirobot[i].revolve_theta = glm::vec3{ 0.f, 0.f, 0.f };
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
			for (int i = 0; i < robot.body.size(); ++i) {
				robot.body[i].rotate_theta = glm::vec3{ 0.f, 90.f, 0.f };
				robot.body[i].revolve_theta.x = 0.f;
			}
			for (int i = 0; i < minirobot.size(); ++i) {
				minirobot[i].revolve_theta = glm::vec3{ 0.f, 90.f, 0.f };
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
			for (int i = 0; i < robot.body.size(); ++i) {
				robot.body[i].rotate_theta = glm::vec3{ 0.f, -90.f, 0.f };
				robot.body[i].revolve_theta.x = 0.f;
			}
			for (int i = 0; i < minirobot.size(); ++i) {
				minirobot[i].revolve_theta = glm::vec3{ 0.f, -90.f, 0.f };
			}
			whirling = glm::vec3{ 0.f, 0.f, 1.f };
		}
		break;
	}
	case 'q':
	case 'Q': {
		exit(829);
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

	robot = Robot({ 1.f, 1.f, 1.f });

	// 카메라
	Camera.pos = glm::vec3{ 0.f, 0.f, 3.f };

	// 문제 출력
	{
		std::cout << "o/O: 앞면이 좌우로 열린다." << std::endl <<
			"w/a/s/d: 로봇이 앞/뒤/좌/우 방향으로 이동 방향을 바꿔서 걷는다." << std::endl <<
			"+/-: 걷는 속도가 빨라지거나/느려진다." << std::endl <<
			"스페이스바: 로봇이 제자리에서 점프한다." << std::endl <<
			"i: 모든 변환을 리셋하고 다시 시작" << std::endl <<
			"z/Z: 앞뒤로 이동" << std::endl <<
			"x/X: 좌우로 이동" << std::endl <<
			"y/Y: 카메라가 현재 위치에서 화면 중심 y축을 기준으로 공전" << std::endl <<
			"q/Q: 종료" << std::endl <<
			"c/C: 점프!" << std::endl;
	}

	static std::uniform_int_distribution<int> FloorUprd(0, 80);
	static std::uniform_int_distribution<int> Floortyperd(1, 2);
	static std::uniform_int_distribution<int> NFloorrd(-4, 4);

	// 바닥
	{
		int floor_color = 0;
		for (int x = -4; x < 5; ++x) {
			for (int z = -4; z < 5; ++z) {
				std::ifstream inputFile("./OBJ/cube.obj");
				Floor.emplace_back();

				if (inputFile.is_open())
					Floor.back().objLoad(inputFile);
				else
					std::cerr << "Failed to obj file" << std::endl;

				Floor.back().pos = glm::vec3{ x * 0.2f, -0.8, z * 0.2f };
				Floor.back().scale = glm::vec3{ 0.2f, 0.2f, 0.2f };

				std::vector<glm::vec3> color;
				for (int i = 0; i < Floor.back().face_cnt * 3; ++i) {
					glm::vec3 a;
					if (floor_color == 0)
						a = glm::vec3{ 0.505, 0.729, 0.482 };
					else if (floor_color == 1) 
						a = glm::vec3{ 0.345, 0.447, 0.647 };
					color.emplace_back(a);
				}

				glGenBuffers(1, &Floor.back().v_color);
				glBindBuffer(GL_ARRAY_BUFFER, Floor.back().v_color);
				glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);

				if (floor_color == 0)
					floor_color = 1;
				else if (floor_color == 1)
					floor_color = 0;
			}
		}

		Floor[FloorUprd(rd)].pos.y = -0.6f;
		Floor[FloorUprd(rd)].pos.y = -0.6f;
		Floor[FloorUprd(rd)].pos.y = -0.6f;
		Floor[FloorUprd(rd)].pos.y = -0.6f;

		for (int i = 0; i < Floor.size(); ++i) {
			Floor_Collision_box.emplace_back();
			Floor_type.emplace_back(0);

			if (Floor[i].pos.y == -0.6f)
				Floor_type.back() = Floortyperd(rd);
			
			Floor_Collision_box.back().pos = Floor[i].pos;
			Floor_Collision_box.back().size = glm::vec3{ 0.1, 0.1f, 0.1 };
		}
	}

	// 기둥
	{
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			Floor.emplace_back();

			if (inputFile.is_open())
				Floor.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Floor.back().pos = glm::vec3{ -4 * 0.2f, -0.5, 0.f };
			Floor.back().scale = glm::vec3{ 0.2f, 0.4f, 0.2f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Floor.back().face_cnt * 3; ++i) {
				glm::vec3 a;
				a = glm::vec3{ 0.605, 0.829, 0.582 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Floor.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Floor.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);

			Floor_Collision_box.emplace_back();
			Floor_type.emplace_back(0);

			Floor_Collision_box.back().pos = Floor.back().pos;
			Floor_Collision_box.back().size = Floor.back().scale * 0.5f;
		}
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			Floor.emplace_back();

			if (inputFile.is_open())
				Floor.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Floor.back().pos = glm::vec3{ -2 * 0.2f, -0.5, 0.f };
			Floor.back().scale = glm::vec3{ 0.2f, 0.4f, 0.2f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Floor.back().face_cnt * 3; ++i) {
				glm::vec3 a;
				a = glm::vec3{ 0.605, 0.829, 0.582 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Floor.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Floor.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);

			Floor_Collision_box.emplace_back();
			Floor_type.emplace_back(0);

			Floor_Collision_box.back().pos = Floor.back().pos;
			Floor_Collision_box.back().size = Floor.back().scale * 0.5f;
		}
		{
			std::ifstream inputFile("./OBJ/cube.obj");
			Floor.emplace_back();

			if (inputFile.is_open())
				Floor.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Floor.back().pos = glm::vec3{ -3 * 0.2f, -0.4f, 0.f };
			Floor.back().scale = glm::vec3{ 0.2f, 0.2f, 0.2f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Floor.back().face_cnt * 3; ++i) {
				glm::vec3 a;
				a = glm::vec3{ 0.605, 0.829, 0.582 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Floor.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Floor.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);

			Floor_Collision_box.emplace_back();
			Floor_type.emplace_back(0);

			Floor_Collision_box.back().pos = Floor.back().pos;
			Floor_Collision_box.back().size = Floor.back().scale * 0.5f;
		}
	}

	// Pillar
	{
		{
			std::ifstream inputFile("./OBJ/cylinder.obj");
			Pillar.emplace_back();

			if (inputFile.is_open())
				Pillar.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Pillar.back().pos = glm::vec3{ -4 * 0.2f, -0.4f, -4 * 0.2f };
			Pillar.back().scale = glm::vec3{ 0.1f, 0.5f, 0.1f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Pillar.back().face_cnt * 3; ++i) {
				glm::vec3 a = glm::vec3{ 0.505, 0.505, 0.505 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Pillar.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Pillar.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		{
			std::ifstream inputFile("./OBJ/cylinder.obj");
			Pillar.emplace_back();

			if (inputFile.is_open())
				Pillar.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Pillar.back().pos = glm::vec3{ 4 * 0.2f, -0.4f, -4 * 0.2f };
			Pillar.back().scale = glm::vec3{ 0.1f, 0.5f, 0.1f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Pillar.back().face_cnt * 3; ++i) {
				glm::vec3 a = glm::vec3{ 0.505, 0.505, 0.505 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Pillar.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Pillar.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		{
			std::ifstream inputFile("./OBJ/cylinder.obj");
			Pillar.emplace_back();

			if (inputFile.is_open())
				Pillar.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Pillar.back().pos = glm::vec3{ -4 * 0.2f, -0.4f, 4 * 0.2f };
			Pillar.back().scale = glm::vec3{ 0.1f, 0.5f, 0.1f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Pillar.back().face_cnt * 3; ++i) {
				glm::vec3 a = glm::vec3{ 0.505, 0.505, 0.505 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Pillar.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Pillar.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		{
			std::ifstream inputFile("./OBJ/cylinder.obj");
			Pillar.emplace_back();

			if (inputFile.is_open())
				Pillar.back().objLoad(inputFile);
			else
				std::cerr << "Failed to obj file" << std::endl;

			Pillar.back().pos = glm::vec3{ 4 * 0.2f, -0.4f, 4 * 0.2f };
			Pillar.back().scale = glm::vec3{ 0.1f, 0.5f, 0.1f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Pillar.back().face_cnt * 3; ++i) {
				glm::vec3 a = glm::vec3{ 0.505, 0.505, 0.505 };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Pillar.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Pillar.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		for (int i = 0; i < Pillar.size(); ++i) {
			Floor_Collision_box.emplace_back();

			Floor_Collision_box.back().pos = Pillar[i].pos;
			Floor_Collision_box.back().size = glm::vec3{ 0.1, 0.5f, 0.1 };
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