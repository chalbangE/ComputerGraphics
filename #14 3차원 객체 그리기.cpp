#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm.hpp>
#include <ext.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <random>

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<GLclampf> colorRd(0.0f, 1.0f);
std::uniform_int_distribution<int> plus1Rd(0, 1);
std::uniform_int_distribution<int> CubeStartPosRd(0, 5);
std::uniform_int_distribution<int> TetraStartPosRd(0, 3);

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
GLuint V_pos_Cube, V_pos_Tetra, V_pos_line[2];

class Shapes
{
public:
	glm::vec3 theta{ 0.0f, 0.0f, 0.0f }; // 회전값 x y z
	glm::vec3 pos; // 객체의 중점
	glm::vec3 scale; // 확대축소용
	glm::mat4 World_mat;
	GLuint v_color{};

	Shapes() {}
	Shapes(glm::vec3 m) : pos(m) { }

	void Update() {
		World_mat = glm::mat4(1.0);

		// World_mat = glm::translate(World_mat, glm::vec3(1.0f, 0.0f, 0.0f));
		World_mat = glm::scale(World_mat, glm::vec3(scale.x, scale.y, scale.z));

		World_mat = glm::rotate(World_mat, glm::radians(theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
		World_mat = glm::rotate(World_mat, glm::radians(theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
	}
};
class Cube :
	public Shapes
{
public:
	Cube() {};
	Cube(glm::vec3 m) : Shapes(m) {
		glm::vec3 color[36];
		for (int i = 0; i < 36; i += 6)  {
			glm::vec3 c = { colorRd(gen), colorRd(gen), colorRd(gen) };
			color[i] = c;
			color[i + 1] = c;
			color[i + 2] = c;
			color[i + 3] = c;
			color[i + 4] = c;
			color[i + 5] = c;
		}

		glGenBuffers(1, &v_color);
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	}

	void draw(int start_pos, int count_pos) {
		glDrawArrays(GL_TRIANGLES, start_pos, count_pos);
	}

	void draw_prepare(int Location, std::string Location_str) {

		if ("Pos" == Location_str) {
			glBindBuffer(GL_ARRAY_BUFFER, V_pos_Cube);
			glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		else if ("Color" == Location_str) {
			glBindBuffer(GL_ARRAY_BUFFER, v_color);
			glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		else if ("World" == Location_str) {
			glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(World_mat));
		}
		//else if ("Tex" == str) {
		//	glBindBuffer(GL_ARRAY_BUFFER, m_TexVBO);
		//	glVertexAttribPointer(Loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		//}

	}
};
class Tetrahedron :
	public Shapes
{
public:
	Tetrahedron() {};
	Tetrahedron(glm::vec3 m) : Shapes(m) {
		glm::vec3 color[12];
		for (int i = 0; i < 12; i += 3) {
			glm::vec3 c = { colorRd(gen), colorRd(gen), colorRd(gen) };
			color[i] = c;
			color[i + 1] = c;
			color[i + 2] = c;
		}

		glGenBuffers(1, &v_color);
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	}

	void draw(int start_pos, int count_pos) {
		glDrawArrays(GL_TRIANGLES, start_pos, count_pos);
	}

	void draw_prepare(int Location, std::string Location_str) {

		if ("Pos" == Location_str) {
			glBindBuffer(GL_ARRAY_BUFFER, V_pos_Tetra);
			glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		else if ("Color" == Location_str) {
			glBindBuffer(GL_ARRAY_BUFFER, v_color);
			glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		else if ("World" == Location_str) {
			glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(World_mat));
		}
		//else if ("Tex" == str) {
		//	glBindBuffer(GL_ARRAY_BUFFER, m_TexVBO);
		//	glVertexAttribPointer(Loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		//}

	}
};

Cube cubeObj;
Tetrahedron tetraObj;

int Mod = 0; // 기본 육면체 출력, 1은 사면체 출력
bool Full_draw = true;
int start_point = 0, count_point = 36;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#14");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Init();
	//Clear();
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	//glutTimerFunc(10, TimerFunction, 1);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene()
{
	glUseProgram(shaderProgramID);
	glm::mat4 model = glm::mat4(1.0f);

	// Location 번호 저장
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	unsigned int WorldTransLocation = glGetUniformLocation(shaderProgramID, "World_trans");
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation); 
	glEnableVertexAttribArray(WorldTransLocation);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// X축 Y축 그리기
	glUniformMatrix4fv(WorldTransLocation, 1, GL_FALSE, glm::value_ptr(model));
	glBindBuffer(GL_ARRAY_BUFFER, V_pos_line[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, V_pos_line[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 4);

	if (Full_draw) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}
	// std::cout << (int)glIsEnabled(GL_CULL_FACE) << std::endl;

	if (Mod == 0) {
		cubeObj.Update();
		cubeObj.draw_prepare(PosLocation, "Pos");
		cubeObj.draw_prepare(ColorLocation, "Color");
		cubeObj.draw_prepare(WorldTransLocation, "World");
		cubeObj.draw(start_point, count_point);
	}
	else if (Mod == 1) {
		tetraObj.Update();
		tetraObj.draw_prepare(PosLocation, "Pos");
		tetraObj.draw_prepare(ColorLocation, "Color");
		tetraObj.draw_prepare(WorldTransLocation, "World");
		tetraObj.draw(start_point, count_point);
	}
	else if (Mod == 2) {
		cubeObj.Update();
		cubeObj.draw_prepare(PosLocation, "Pos");
		cubeObj.draw_prepare(ColorLocation, "Color");
		cubeObj.draw_prepare(WorldTransLocation, "World");
		int a = CubeStartPosRd(gen) * 6;
		int b = CubeStartPosRd(gen) * 6;

		while (a == b) {
			b = CubeStartPosRd(gen) * 6;
		}
		cubeObj.draw(a, 6);
		cubeObj.draw(b, 6);
	}
	else if (Mod == 3) {
		tetraObj.Update();
		tetraObj.draw_prepare(PosLocation, "Pos");
		tetraObj.draw_prepare(ColorLocation, "Color");
		tetraObj.draw_prepare(WorldTransLocation, "World");
		int a = TetraStartPosRd(gen) * 3;
		int b = TetraStartPosRd(gen) * 3;
		
		while (a == b) {
			b = TetraStartPosRd(gen) * 3;
		}

		tetraObj.draw(a, 3);
		tetraObj.draw(b, 3);
	}

	if (Full_draw) {
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
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6': {
		Mod = 0;
		Full_draw = false;
		start_point = (key - '1') * 6;
		count_point = 6;
		break;
	}
	case '7':
	case '8':
	case '9':
	case '0': {
		Mod = 1;
		Full_draw = false;
		start_point = (key - '7') * 3;
		count_point = 3;

		if (key == '0') {
			start_point = 9;
		}
		break;
	}
	case 'c':
	case 'C': {
		Mod = 2;
		Full_draw = false;
		break;
	}
	case 't':
	case 'T': {
		Mod = 3;
		Full_draw = false;
		break;
	}
	case '+':
	case '=': {
		Mod = 0;
		Full_draw = true;
		start_point = 0;
		count_point = 36;
		break;
	}
	case '-':
	case '_': {
		Mod = 1;
		Full_draw = true;
		start_point = 0;
		count_point = 12;
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void Init()
{
	glBindVertexArray(vao);

	// 육면체
	{
		glm::vec3 cube[36]{
			{ -0.1f, -0.1f, 0.1f }, { 0.1f, -0.1f, 0.1f }, { -0.1f, 0.1f, 0.1f },
			{ -0.1f, 0.1f, 0.1f }, { 0.1f, -0.1f, 0.1f }, { 0.1f, 0.1f, 0.1f },

			{ 0.1f, -0.1f, 0.1f }, { 0.1f, -0.1f, -0.1f }, { 0.1f, 0.1f, 0.1f },
			{ 0.1f, 0.1f, 0.1f }, { 0.1f, -0.1f, -0.1f }, { 0.1f, 0.1f, -0.1f },

			{ 0.1f, -0.1f, -0.1f }, { -0.1f, -0.1f, -0.1f }, { 0.1f, 0.1f, -0.1f },
			{ 0.1f, 0.1f, -0.1f }, { -0.1f, -0.1f, -0.1f }, { -0.1f, 0.1f, -0.1f },

			{ -0.1f, -0.1f, -0.1f }, { -0.1f, -0.1f, 0.1f }, { -0.1f, 0.1f, -0.1f },
			{ -0.1f, 0.1f, -0.1f }, { -0.1f, -0.1f, 0.1f }, { -0.1f, 0.1f, 0.1f },

			{ -0.1f, 0.1f, 0.1f }, { 0.1f, 0.1f, 0.1f }, { -0.1f, 0.1f, -0.1f },
			{ -0.1f, 0.1f, -0.1f }, { 0.1f, 0.1f, 0.1f }, { 0.1f, 0.1f, -0.1f },

			{ -0.1f, -0.1f, 0.1f }, { -0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, 0.1f },
			{ -0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, -0.1f }, { 0.1f, -0.1f, 0.1f }
		};

		glGenBuffers(1, &V_pos_Cube);
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_Cube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

		cubeObj = Cube({ 0.0f, 0.0f, 0.0f });
		cubeObj.theta.x = cubeObj.theta.y = 30.0f;
		cubeObj.scale = glm::vec3{ 2.0f, 2.0f, 2.0f };
	}
	// 사면체
	{
		glm::vec3 tetra[18]{
			{0.1f, -0.1f * std::sqrt(3) / 3.f, 0.1f * std::sqrt(3) / 3.f},
			{-0.1f, -0.1f * std::sqrt(3) / 3.f, 0.1f * std::sqrt(3) / 3.f},
			{0.f, -0.1f * std::sqrt(3) / 3.f, -0.1f * std::sqrt(3) * 2.f / 3.f},

			{0.1f, -0.1f * std::sqrt(3) / 3.f, 0.1f * std::sqrt(3) / 3.f},
			{0.f, 0.1f * std::sqrt(3) * 2.f / 3.f, 0.f},
			{-0.1f, -0.1f * std::sqrt(3) / 3.f, 0.1f * std::sqrt(3) / 3.f},

			{0.f, -0.1f * std::sqrt(3) / 3.f, -0.1f * std::sqrt(3) * 2.f / 3.f},
			{0.f, 0.1f * std::sqrt(3) * 2.f / 3.f, 0.f},
			{0.1f, -0.1f * std::sqrt(3) / 3.f, 0.1f * std::sqrt(3) / 3.f},

			{-0.1f, -0.1f * std::sqrt(3) / 3.f, 0.1f * std::sqrt(3) / 3.f},
			{0.f, 0.1f * std::sqrt(3) * 2.f / 3.f, 0.f},
			{0.f, -0.1f * std::sqrt(3) / 3.f, -0.1f * std::sqrt(3) * 2.f / 3.f}
		};

		glGenBuffers(1, &V_pos_Tetra);
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_Tetra);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tetra), tetra, GL_STATIC_DRAW);

		tetraObj = Tetrahedron({ 0.0f, 0.0f, 0.0f });
		tetraObj.theta.x = tetraObj.theta.y = 30.0f;
		tetraObj.scale = glm::vec3{ 2.0f, 2.0f, 2.0f };
	}
	// X축 Y축
	{
		glm::vec3 line[4]{
			{1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
		};

		glGenBuffers(1, &V_pos_line[0]);
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_line[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

		glm::vec3 black_Line{ 0.0f, 0.0f, 0.0f };
		glGenBuffers(1, &V_pos_line[1]);
		glBindBuffer(GL_ARRAY_BUFFER, V_pos_line[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(black_Line), &black_Line, GL_STATIC_DRAW);
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
	vertexSource = filetobuf("3d_vertex.glsl");
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