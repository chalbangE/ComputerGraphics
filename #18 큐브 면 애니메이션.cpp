#include "stdafx.h"
#include "GLShapes.h"
#include "GLRect.h"
#include "GLLIne.h"
#include "GLTri.h"

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

float winSizex = 800, winSizey = 800;
GLuint vao;

std::vector <GLRect> rect;
std::vector <GLTri> tri;
GLLine lineObj;

bool Back_Cull = true;
bool Tmod = false, Fmod = false, Smod = false, 
		Bmod = false, Omod = false, Rmod = false, Ymod = false;
int Mod = 1, Rcnt = 0, Projection_Mod = 0;
glm::vec3 Fmod_Theta_plus = glm::vec3{ 0.0f, 0.0f, -2.0f };
glm::vec3 Smod_Pos_plus = glm::vec3{ 0.0f, 0.005f, 0.0f };
glm::vec3 Bmod_Scale_plus = glm::vec3{ -0.01f, -0.01f, -0.01f };
glm::vec3 Omod_Theta_plus[4]{
	{ 2.0f, 0.0f, 0.0f }, { -2.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -2.0f }, { 0.0f, 0.0f, 2.0f }
};

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#18 면 애니메이션");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Init();
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutTimerFunc(10, TimerFunction, 1);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene()
{
	glUseProgram(shaderProgramID);

	// 카메라 변환
	glm::mat4 Camera_Mat = glm::lookAt(glm::vec3 { 1.f, 1.f, 1.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f });
	
	// 투영 변환
	glm::mat4 Projection_Mat = glm::mat4(1.0f);
	{
		if (Projection_Mod == 0) {
			Projection_Mat = glm::ortho(-2.f, 2.f, -2.f, 2.f, 0.1f, 50.f);
		}
		else if (Projection_Mod == 1) {
			// Camera_Mat = glm::translate(Camera_Mat, glm::vec3{ 0.f, 0.f, -1.f });
			Projection_Mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
		}
	}
	unsigned int ProjectionLocation = glGetUniformLocation(shaderProgramID, "Projection_trans");


	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	unsigned int WorldTransLocation = glGetUniformLocation(shaderProgramID, "World_trans");
	unsigned int CameraLocation = glGetUniformLocation(shaderProgramID, "Camera_trans");
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glEnableVertexAttribArray(WorldTransLocation);


	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, &Projection_Mat[0][0]);
	glUniformMatrix4fv(CameraLocation, 1, GL_FALSE, glm::value_ptr(Camera_Mat));

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (Back_Cull) {
		glEnable(GL_DEPTH_TEST);
		// glEnable(GL_CULL_FACE);
	}
	// X축 Y축 그리기
	lineObj.Update();
	lineObj.draw_prepare(PosLocation, "Pos");
	lineObj.draw_prepare(ColorLocation, "Color");
	lineObj.draw_prepare(WorldTransLocation, "World");
	lineObj.draw();

	// std::cout << (int)glIsEnabled(GL_CULL_FACE) << std::endl;

	if (Mod == 0) {
		for (int i = 0; i < rect.size(); ++i) {
			rect[i].Update();
			rect[i].draw_prepare(PosLocation, "Pos");
			rect[i].draw_prepare(ColorLocation, "Color");
			rect[i].draw_prepare(WorldTransLocation, "World");
			rect[i].draw("solid");
			// std::cout << rect[i].midpos.x << "  " << rect[i].midpos.y << "  " << rect[i].midpos.z << std::endl;
		}
	}
	else if (Mod == 1) {
		for (int i = 0; i < tri.size(); ++i) {
			tri[i].Update();
			tri[i].draw_prepare(PosLocation, "Pos");
			tri[i].draw_prepare(ColorLocation, "Color");
			tri[i].draw_prepare(WorldTransLocation, "World");
			tri[i].draw("solid");
			// std::cout << rect[i].midpos.x << "  " << rect[i].midpos.y << "  " << rect[i].midpos.z << std::endl;
		}
	}

	if (Back_Cull) {
		glDisable(GL_DEPTH_TEST);
		// glDisable(GL_CULL_FACE);
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
		if (Mod == 0) {
			if (Tmod) 
				rect[4].rotate_theta += glm::vec3{ 0.0f, 0.0f, 2.0f };

			if (Fmod) {
				rect[2].revolve_theta += Fmod_Theta_plus;
				if (rect[2].revolve_theta.z == -90.f || rect[2].revolve_theta.z == 0.f) {
					Fmod = false;
					Fmod_Theta_plus *= -1;
				}
			}

			if (Smod) {
				rect[0].pos += Smod_Pos_plus;
				rect[1].pos += Smod_Pos_plus;
				//std::cout << rect[0].pos.y << std::endl;
				if (rect[0].pos.y >= 0.5f || rect[0].pos.y <= 0.f) {
					Smod = false;
					Smod_Pos_plus *= -1;
					rect[0].pos += Smod_Pos_plus;
					rect[1].pos += Smod_Pos_plus;
				}
			}

			if (Bmod) {
				rect[3].scale += Bmod_Scale_plus;
				if (rect[3].scale.x >= 1.0f || rect[3].scale.x <= 0.f) {
					Bmod = false;
					Bmod_Scale_plus *= -1;
				}
			}

			if (Ymod) {
				for (int i = 0; i < rect.size(); ++i) {
					rect[i].trun_theta += glm::vec3{ 0.f, 2.f, 0.f };
				}
			}
		}
		else if (Mod == 1) {
			if (Omod) {
				for (int i = 0; i < tri.size() - 2; ++i) {
					tri[i].rotate_theta += Omod_Theta_plus[i];
				}
				if (tri[0].rotate_theta.x >= 233.0f || tri[0].rotate_theta.x <= 0.f) {
					Omod = false;
					for (int i = 0; i < tri.size() - 2; ++i) {
						Omod_Theta_plus[i] *= -1;
						tri[i].rotate_theta += Omod_Theta_plus[i] / glm::vec3{ 2.0f,2.0f,2.0f };
					}
				}
			}

			if (Rmod) {
				static int cnt = 0;
				tri[Rcnt].rotate_theta += Omod_Theta_plus[Rcnt];
				cnt++;
				if (cnt >= 50) {
					Rmod = false;
					Omod_Theta_plus[Rcnt] *= -1;
					Rcnt++;
					cnt = 0;
				}
			}

			if (Ymod) {
				for (int i = 0; i < tri.size(); ++i) {
					tri[i].revolve_theta += glm::vec3{ 0.f, 2.f, 0.f };
				}
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
	// 좌 / 우 / 상 / 하로 객체를 이동한다. (x축, y축으로 이동)
	//	s : 초기 위치로 리셋(자전 애니메이션도 멈추기)
	switch (key)
	{
	case 'c':
	case 'C': {
		if (Mod == 0) Mod = 1;
		else Mod = 0;
		break;
	}
	case 'h':
	case 'H': {
		if (Back_Cull) Back_Cull = false;
		else Back_Cull = true;
		break;
	}
	case 't':
	case 'T': {
		if (Tmod == 0) Tmod = 1;
		else Tmod = 0;
		break;
	}
	case 'f':
	case 'F': {
		if (Fmod == 0) Fmod = 1;
		else Fmod = 0;
		break;
	}
	case 's':
	case 'S': {
		if (Smod == 0) Smod = 1;
		else Smod = 0;
		break;
	}
	case 'b':
	case 'B': {
		if (Bmod == 0) Bmod = 1;
		else Bmod = 0;
		break;
	}
	case 'o':
	case 'O': {
		if (Omod == 0) Omod = 1;
		else Omod = 0;
		break;
	}
	case 'r':
	case 'R': {
		if (Rmod == 0)  {
			Rmod = 1;
			if (Rcnt >= tri.size() - 2)
				Rcnt = 0;
		}
		else Rmod = 0;
		break;
	}
	case 'p':
	case 'P': {
		if (Projection_Mod == 0) Projection_Mod = 1;
		else Projection_Mod = 0;
		break;
	}
	case 'y':
	case 'Y': {
		if (Ymod == 0) Ymod = 1;
		else Ymod = 0;
		break;
	}	
	//	// 자전키
	//	{
	//case 'j':
	//case 'J': {
	//	Theta_plus = glm::vec3{ 0.0f, -2.0f, 0.0f };
	//	if (Rotation_Mod == 1) {
	//		Rotation_Mod = 0;
	//		Theta_plus = { 0.0f, 0.0f, 0.0f };
	//	}
	//	else Rotation_Mod = 1;
	//	break;
	//}
	//case 'l':
	//case 'L': {
	//	Theta_plus = glm::vec3{ 0.0f, 2.0f, 0.0f };
	//	if (Rotation_Mod == 2) {
	//		Rotation_Mod = 0;
	//		Theta_plus = { 0.0f, 0.0f, 0.0f };
	//	}
	//	else  Rotation_Mod = 2;
	//	break;
	//}
	//case 'i':
	//case 'I': {
	//	Theta_plus = glm::vec3{ 2.0f, 0.0f, 0.0f };
	//	if (Rotation_Mod == 3) {
	//		Rotation_Mod = 0;
	//		Theta_plus = { 0.0f, 0.0f, 0.0f };
	//	}
	//	else Rotation_Mod = 3;
	//	break;
	//}
	//case 'k':
	//case 'K': {
	//	Theta_plus = glm::vec3{ -2.0f, 0.0f, 0.0f };
	//	if (Rotation_Mod == 4) {
	//		Rotation_Mod = 0;
	//		Theta_plus = { 0.0f, 0.0f, 0.0f };
	//	}
	//	else Rotation_Mod = 4;
	//	break;
	//}
	//	}
	//	// 리셋
	//case 's':
	//case 'S': {
	//	Init();
	//	Rotation_Mod = 0;
	//	Theta_plus = { 0.0f, 0.0f, 0.0f };
	//}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void Init()
{
	glBindVertexArray(vao);

	// 육면체 Cube
	{
		{ // z축 기준 옆
			rect.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 cube[6]{
				{ -0.25f, -0.25f, 0.25f }, { 0.25f, -0.25f, 0.25f }, { -0.25f, 0.25f, 0.25f },
				{ -0.25f, 0.25f, 0.25f }, { 0.25f, -0.25f, 0.25f }, { 0.25f, 0.25f, 0.25f }
			};
			glGenBuffers(1, &rect.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, rect.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				rect.back().midpos += cube[i];
			rect.back().midpos /= 6.f;
		}
		{ // z축 기준 옆
			rect.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 cube[6]{
				{ 0.25f, -0.25f, -0.25f }, { -0.25f, -0.25f, -0.25f }, { 0.25f, 0.25f, -0.25f },
				{ 0.25f, 0.25f, -0.25f }, { -0.25f, -0.25f, -0.25f }, { -0.25f, 0.25f, -0.25f }
			};
			glGenBuffers(1, &rect.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, rect.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				rect.back().midpos += cube[i];
			rect.back().midpos /= 6.f;
		}
		{ // x축 기준 옆
			rect.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 cube[6]{
				{ 0.25f, -0.25f, 0.25f }, { 0.25f, -0.25f, -0.25f }, { 0.25f, 0.25f, 0.25f },
				{ 0.25f, 0.25f, 0.25f }, { 0.25f, -0.25f, -0.25f }, { 0.25f, 0.25f, -0.25f }
			};
			glGenBuffers(1, &rect.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, rect.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				rect.back().midpos += cube[i];
			rect.back().midpos /= 6.f;
			rect.back().Anipos = { 0.25f, -0.25f, 0.f };
		}
		{ // x축 기준 옆
			rect.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 cube[6]{
				{ -0.25f, -0.25f, -0.25f }, { -0.25f, -0.25f, 0.25f }, { -0.25f, 0.25f, -0.25f },
				{ -0.25f, 0.25f, -0.25f }, { -0.25f, -0.25f, 0.25f }, { -0.25f, 0.25f, 0.25f }
			};
			glGenBuffers(1, &rect.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, rect.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				rect.back().midpos += cube[i];
			rect.back().midpos /= 6.f;
			rect.back().midpos.y = 0.25f;
		}
		{  // y축 기준 옆
			rect.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 cube[6]{
				{ -0.25f, 0.25f, 0.25f }, { 0.25f, 0.25f, 0.25f }, { -0.25f, 0.25f, -0.25f },
				{ -0.25f, 0.25f, -0.25f }, { 0.25f, 0.25f, 0.25f }, { 0.25f, 0.25f, -0.25f }
			};
			glGenBuffers(1, &rect.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, rect.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				rect.back().midpos += cube[i];
			rect.back().midpos /= 6.f;
		}
		{ // y축 기준 옆
			rect.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 cube[6]{
				{ -0.25f, -0.25f, 0.25f }, { -0.25f, -0.25f, -0.25f }, { 0.25f, -0.25f, 0.25f },
				{ -0.25f, -0.25f, -0.25f }, { 0.25f, -0.25f, -0.25f }, { 0.25f, -0.25f, 0.25f }
			};
			glGenBuffers(1, &rect.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, rect.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
			for (int i = 0; i < 6; ++i)
				rect.back().midpos += cube[i];
			rect.back().midpos /= 6.f;
		}
		for (int i = 0; i < rect.size(); ++i) {
			//rect[i].rotate_theta.x = 30.0f;
			//rect[i].rotate_theta.y = -30.0f;
			rect[i].scale = glm::vec3{ 1.0f, 1.0f, 1.0f };
		}
	}
	// 사각뿔 Pyra
	{
		{ // z축 앞면
			tri.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });
			glm::vec3 pyra[3]{
				{ 0.0f, 0.25f, 0.0f }, { -0.25f, -0.25f, 0.25f }, { 0.25f, -0.25f, 0.25f }
			};

			glGenBuffers(1, &tri.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, tri.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyra), pyra, GL_STATIC_DRAW);

			tri.back().midpos = { 0.f, -0.25f, 0.25f };
		}
		{ // z축 뒷면
			tri.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 pyra[3]{
				{ 0.0f, 0.25f, 0.0f }, { 0.25f, -0.25f, -0.25f }, { -0.25f, -0.25f, -0.25f }
			};

			glGenBuffers(1, &tri.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, tri.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyra), pyra, GL_STATIC_DRAW);

			tri.back().midpos = { 0.f, -0.25f, -0.25f };
		}
		{ // x축 앞면
			tri.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 pyra[3]{
				{ 0.0f, 0.25f, 0.0f }, { 0.25f, -0.25f, 0.25f }, { 0.25f, -0.25f, -0.25f }
			};

			glGenBuffers(1, &tri.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, tri.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyra), pyra, GL_STATIC_DRAW);

			tri.back().midpos = { 0.25f, -0.25f, 0.f };
		}
		{ // x축 뒷면
			tri.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 pyra[3]{
				{ 0.0f, 0.25f, 0.0f }, { -0.25f, -0.25f, -0.25f }, { -0.25f, -0.25f, 0.25f }
			};

			glGenBuffers(1, &tri.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, tri.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyra), pyra, GL_STATIC_DRAW);

			tri.back().midpos = { -0.25f, -0.25f, 0.f };
		}
		{
			tri.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 pyra[3]{
				{ -0.25f, -0.25f, 0.25f }, { -0.25f, -0.25f, -0.25f }, { 0.25f, -0.25f, 0.25f }
			};

			glGenBuffers(1, &tri.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, tri.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyra), pyra, GL_STATIC_DRAW);

			for (int i = 0; i < 3; ++i)
				tri.back().midpos += pyra[i];
			tri.back().midpos /= 3.f;
		}
		{
			tri.emplace_back(glm::vec3{0.0f, 0.0f, 0.0f});
			glm::vec3 pyra[3]{
				{ -0.25f, -0.25f, -0.25f }, { 0.25f, -0.25f, -0.25f }, { 0.25f, -0.25f, 0.25f }
			};

			glGenBuffers(1, &tri.back().v_pos);
			glBindBuffer(GL_ARRAY_BUFFER, tri.back().v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyra), pyra, GL_STATIC_DRAW);

			for (int i = 0; i < 3; ++i)
				tri.back().midpos += pyra[i];
			tri.back().midpos /= 3.f;
		}

		for (int i = 0; i < tri.size(); ++i) {
			tri[i].scale = glm::vec3{ 1.0f, 1.0f, 1.0f };
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

		//lineObj.rotate_theta.x = 30.0f;
		//lineObj.rotate_theta.y = -30.0f;
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