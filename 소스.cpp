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

class Robot : GLObj {
public:
	std::vector <GLObj> body;
	Collision_box collision_box;

	Robot() {};
	Robot(glm::vec3 size) {
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
			body[i].Oscale = size;
		}
	}

	void update_collision_box() {
		glm::vec3 max;
		max = (body[0].pos + (body[0].scale /** glm::vec3{ 0.5, 0.5, 0.5 }*/)) * body[0].Oscale;
		//cout << "max.y : " << max.y << "    머리.y : " << body[0].pos.y << endl;

		glm::vec3 min;
		min = (body[0].pos + (body[0].scale /** glm::vec3{ 0.5, 0.5, 0.5 }*/)) * body[0].Oscale;
		min.y = (body[3].pos.y - (body[3].scale.y * 0.5)) * body[0].Oscale.y;
		//cout << "min.y : " << min.y << "    다리.y : " << body[3].pos.y << endl;

		collision_box.pos = (max + min) / glm::vec3{ 2.f, 2.f, 2.f };
		collision_box.size = abs(max - collision_box.pos);
	}

	void Update24() {0
		World_mat = glm::mat4(1.0);

		World_mat = glm::translate(World_mat, pos);
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
std::vector <Collision_box> Floor_Collision_box;
GLLine lineObj;
GLCamera Camera;

bool Back_Cull = true;
bool Omod = false, Walking = false, Jump_on = true, Floor_on = false;
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

	for (int i = 0; i < robot.body.size(); ++i) {
		robot.body[i].Update22();
		robot.body[i].draw_prepare(PosLocation, "Pos");
		robot.body[i].draw_prepare(ColorLocation, "Color");
		robot.body[i].draw_prepare(WorldTransLocation, "World");
		robot.body[i].draw("solid");
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
				break;
			}
			case 's': {
				break;
			}
			case 'a': {
				break;
			}
			case 'd': {
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
		Fu_collision_box.pos.y += Jump;
		for (int k = 0; k < Floor_Collision_box.size(); ++k) {
			if (CheckCollision(Fu_collision_box, Floor_Collision_box[k])) {
				Floor_on = true;
				Jump_on = false;
				for (int i = 0; i < robot.body.size(); ++i) {
					robot.body[i].pos.y -= (robot.collision_box.pos.y - robot.collision_box.size.y) - (Floor_Collision_box[k].pos.y + Floor_Collision_box[k].size.y);
					robot.body[i].Ani_mid.y -= (robot.collision_box.pos.y - robot.collision_box.size.y) - (Floor_Collision_box[k].pos.y + Floor_Collision_box[k].size.y);
				}
			}
			//if (k >= 10 && k <= 16) {
			//	cout << Floor_Collision_box[k].pos.y << "    " << Floor_on << endl;
			//	cout << Fu_collision_box.pos.y << "    " << Jump << "    " << Floor_on << endl;
			//}
		}


		if (Jump_on || !Floor_on) {
			for (int i = 0; i < robot.body.size(); ++i) {
				robot.body[i].pos.y += Jump;
				robot.body[i].Ani_mid.y += Jump;
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
			for (int i = 0; i < robot.body.size(); ++i) {
				robot.body[i].rotate_theta = glm::vec3{ 0.f, 180.f, 0.f };
				robot.body[i].revolve_theta.z = 0.f;
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

	robot = Robot({ 1.f, 1.f, 1.f });

	// 카메라
	Camera.pos = glm::vec3{ 0.f, 0.f, 3.f };

	static std::uniform_int_distribution<int> FloorUprd(0, 80);
	// Floor
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

		for (int i = 0; i < Floor.size(); ++i) {
			Floor_Collision_box.emplace_back();

			glm::vec3 max;
			max = (Floor[i].pos + (Floor[i].scale * glm::vec3{ 0.5, 0.5, 0.5 }));

			glm::vec3 min;
			min = (Floor[i].pos - (Floor[i].scale * glm::vec3{ 0.5, 0.5, 0.5 }));

			Floor_Collision_box.back().pos = (max + min) / glm::vec3{ 2.f, 2.f, 2.f };
			Floor_Collision_box.back().size.x = abs(max.x - Floor_Collision_box.back().pos.x);
			Floor_Collision_box.back().size = abs(max - Floor_Collision_box.back().pos);
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