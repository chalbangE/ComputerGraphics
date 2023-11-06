#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include "stdafx.h"
#include "GLSlice.h"
#include "GLLine.h"
#include <algorithm>

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

#define pie 3.141592653589793238

void make_shaderProgram();void make_vertexShaders();void make_fragmentShaders();void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);GLvoid drawScene();GLvoid Reshape(int w, int h);void Init();void InitBuffer();char* filetobuf(const char*);void Motion(int x, int y);
int Cross_Line(glm::vec3 p1, glm::vec3 p2, glm::vec3 m1, glm::vec3 m2, std::vector <glm::vec3>& return_crossPos);
void Cut_Polygon(std::vector<glm::vec3>& crossPos, int who, int crossWhere1, int crossWhere2);

class Basket {
public:
	GLuint v_pos;
	GLuint v_color;

	glm::vec3 firstpos[4]{ { 0.3f, -0.7f, 1.f }, { -0.3f, -0.7f, 1.f }, { -0.3f, -0.8f, 1.f }, { 0.3f, -0.8f, 1.f } };
	glm::vec3 dis{ -0.01f, 0.f, 0.f };
	
	Basket(){}

	Basket(int i) {
		glGenBuffers(1, &v_pos);
		glBindBuffer(GL_ARRAY_BUFFER, v_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(firstpos), firstpos, GL_STATIC_DRAW);
		glm::vec3 color[4]{ { 0.4f, 0.6f, 1.f }, { 0.4f, 0.6f, 1.f }, { 0.4f, 0.6f, 1.f }, { 0.4f, 0.6f, 1.f } };
		glGenBuffers(1, &v_color);
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	}

	void draw() {
		glDrawArrays(GL_POLYGON, 0, 4);
	}

	void Update(std::vector <GLSlice>& Basket_In_Polygon) {
		for (int i = 0; i < 4; ++i) {
			firstpos[i] += dis;
		}

		if (firstpos[1].x <= -0.9f) {
			dis.x *= -1;

			for (int i = 0; i < Basket_In_Polygon.size(); ++i) {
				Basket_In_Polygon[i].dis.x *= -1;
			}
		}
		else if (firstpos[0].x >= 0.9f) {
			dis.x *= -1;

			for (int i = 0; i < Basket_In_Polygon.size(); ++i) {
				Basket_In_Polygon[i].dis.x *= -1;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, v_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(firstpos), firstpos, GL_STATIC_DRAW);
	}

	void draw_prepare(int Location, std::string Location_str) {
		if ("Pos" == Location_str) {
			glBindBuffer(GL_ARRAY_BUFFER, v_pos);
			glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		else if ("Color" == Location_str) {
			glBindBuffer(GL_ARRAY_BUFFER, v_color);
			glVertexAttribPointer(Location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
	}
};

GLuint vao;
float winSizex = 800, winSizey = 800;

std::vector <GLSlice> Polygon1;
std::vector <GLSlice> Basket_In_Polygon;
GLLine MOUSE;
glm::vec3 mouse[2];
bool Lbt = false, Route = false;
std::string Draw_Mod = "solid";
GLfloat Speed = 1.f;
Basket basket;


int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("Let's slice");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	Init();
	make_shaderProgram();
	InitBuffer();
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(10, TimerFunction, 1);
	glutTimerFunc(10, TimerFunction, 2);
	glutMainLoop();
}

GLvoid drawScene()
{
	// 배경색 설정
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// Location 번호 저장
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation); // Vertex Position 정보 영역 시작 (Enable)

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (Route) {
		for (int i = 0; i < Polygon1.size(); ++i) {
			if (!Polygon1[i].cut) {
				glBindBuffer(GL_ARRAY_BUFFER, Polygon1[i].line_pos);
				glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glBindBuffer(GL_ARRAY_BUFFER, Polygon1[i].line_color);
				glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

				glDrawArrays(GL_LINES, 0, Polygon1[i].line_cnt);
			}
		}
	}

	for (int i = 0; i < Basket_In_Polygon.size(); ++i) {
		Basket_In_Polygon[i].Update();
		Basket_In_Polygon[i].draw_prepare(PosLocation, "Pos");
		Basket_In_Polygon[i].draw_prepare(ColorLocation, "Color");
		Basket_In_Polygon[i].draw(Draw_Mod);
	}

	for (int i = 0; i < Polygon1.size(); ++i) {
		Polygon1[i].Update(Speed, basket.firstpos);
		Polygon1[i].dis.y -= 0.0003f * Speed;
		Polygon1[i].draw_prepare(PosLocation, "Pos");
		Polygon1[i].draw_prepare(ColorLocation, "Color");
		Polygon1[i].draw(Draw_Mod);
		if (!Polygon1[i].on) {
			glDeleteBuffers(1, &Polygon1[i].v_pos);
			glDeleteBuffers(1, &Polygon1[i].v_color);
			glDeleteBuffers(1, &Polygon1[i].line_pos);
			glDeleteBuffers(1, &Polygon1[i].line_color);
			Polygon1.erase(Polygon1.begin() + i);
			--i;
		}
		else if (Polygon1[i].basket_in) {
			glDeleteBuffers(1, &Polygon1[i].line_pos);
			glDeleteBuffers(1, &Polygon1[i].line_color);
			Basket_In_Polygon.push_back(Polygon1[i]);
			Basket_In_Polygon.back().dis = basket.dis;
			Polygon1.erase(Polygon1.begin() + i);
			--i;
		}
	}

	basket.Update(Basket_In_Polygon);
	basket.draw_prepare(PosLocation, "Pos");
	basket.draw_prepare(ColorLocation, "Color");
	basket.draw();

	if (Lbt) {
		MOUSE.draw_prepareMouse(PosLocation, "Pos");
		MOUSE.draw_prepareMouse(ColorLocation, "Color");
		MOUSE.drawMouse();
	}
	
	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);

	glutSwapBuffers(); //--- 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	case 'D': {
		if (Draw_Mod == "solid")
			Draw_Mod = "line";
		else Draw_Mod = "solid";
		break;
	}
	case 'l':
	case 'L': {
		if (Route) Route = false;
		else Route = true;
		break;
	}
	case '+':
	case '=': {
		Speed += 0.1f;
		if (Speed > 2.f)
			Speed = 2.f;
		break;
	}
	case '-':
	case '_': {
		Speed -= 0.1f;
		if (Speed < 0.5f)
			Speed = 0.5f;
		break;
	}
	case 'c':
	case 'C': {
		for (int i = 0; i < Basket_In_Polygon.size(); ++i) {
			glDeleteBuffers(1, &Basket_In_Polygon[i].v_pos);
			glDeleteBuffers(1, &Basket_In_Polygon[i].v_color);
		}
		Basket_In_Polygon.clear();
		break;
	}
	case 'q':
	case 'Q': {
		exit(526); // 배고파
	}
	}
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		glutTimerFunc(10, TimerFunction, 1);
		break;
	}
	case 2: {
		Polygon1.emplace_back();
		glutTimerFunc(800, TimerFunction, 2);
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void Mouse(int button, int state, int x, int y)
{
	GLfloat n = 0.015f;

	glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };

	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			mouse[0] = glm::vec3{ m.x, m.y, 1.f };
			mouse[1] = glm::vec3{ m.x, m.y, 1.f };
			Lbt = true;
			glBindBuffer(GL_ARRAY_BUFFER, MOUSE.v_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(mouse), mouse, GL_STATIC_DRAW);
		}
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON) {
			Lbt = false;
			int size = Polygon1.size();

			for (int i = 0; i < size; ++i) {
				int cnt = 0, whereCross[3];
				std::vector <glm::vec3> cross;

				for (int k = 0; k < Polygon1[i].summit_cnt - 1; ++k) {
					glm::vec3 pos1 = Polygon1[i].firstpos[k];
					glm::vec3 pos2 = Polygon1[i].firstpos[k + 1];

					int a = cnt;
					cnt += Cross_Line(pos1, pos2, mouse[0], mouse[1], cross);
					if (a < cnt) {
						if (cnt == 1)
							whereCross[0] = k;
						else if (cnt == 2)
							whereCross[1] = k;
					}
				}
				glm::vec3 pos1 = Polygon1[i].firstpos[0];
				glm::vec3 pos2 = Polygon1[i].firstpos.back();

				int a = cnt;
				cnt += Cross_Line(pos1, pos2, mouse[0], mouse[1], cross);

				if (a < cnt) {
					whereCross[1] = Polygon1[i].summit_cnt - 1;
				}

				if (cnt == 2) {
					Cut_Polygon(cross, i, whereCross[0], whereCross[1]);
				}
			}

			mouse[1] = mouse[0];
		}
	}
}void Motion(int x, int y)
{
	glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };

	if (Lbt) {
		mouse[1] = glm::vec3{ m.x, m.y, 1.f };
		
		glBindBuffer(GL_ARRAY_BUFFER, MOUSE.v_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mouse), mouse, GL_STATIC_DRAW);
	}
}

int Cross_Line(glm::vec3 p1, glm::vec3 p2, glm::vec3 ms1, glm::vec3 ms2, std::vector <glm::vec3>& return_crossPos) {
	glm::vec3 min[2], max[2];

	if (p1.x < p2.x) {
		min[0].x = p1.x; 
		max[0].x = p2.x;
	}
	else {
		min[0].x = p2.x;
		max[0].x = p1.x;
	}

	if (p1.y < p2.y) {
		min[0].y = p1.y; 
		max[0].y = p2.y;
	}
	else {
		min[0].y = p2.y;
		max[0].y = p1.y;
	}

	if (ms1.x < ms2.x) {
		min[1].x = ms1.x;
		max[1].x = ms2.x;
	}
	else {
		min[1].x = ms2.x;
		max[1].x = ms1.x;
	}

	if (ms1.y < ms2.y) {
		min[1].y = ms1.y; 
		max[1].y = ms2.y;
	}
	else {
		min[1].y = ms2.y;
		max[1].y = ms1.y;
	}

	GLfloat m1 = (p2.y - p1.y) / (p2.x - p1.x);
	GLfloat m2 = (ms2.y - ms1.y) / (ms2.x - ms1.x);

	if (m1 == m2) {
		return 0;
	}
	else {
		GLfloat b1 = p1.y - (m1 * p1.x);
		GLfloat b2 = ms1.y - (m2 * ms1.x);
		
		glm::vec3 cross_pos = { ((b2 - b1) / (m1 - m2)), m1 * ((b2 - b1) / (m1 - m2)) + b1, 1.f };

		max[0].x = ceil(max[0].x * 1000) / 1000;
		max[1].x = ceil(max[1].x * 1000) / 1000;
		min[0].x = floor(min[0].x * 1000) / 1000;
		min[1].x = floor(min[1].x * 1000) / 1000;
		max[0].y = ceil(max[0].y * 1000) / 1000;
		max[1].y = ceil(max[1].y * 1000) / 1000;
		min[0].y = floor(min[0].y * 1000) / 1000;
		min[1].y = floor(min[1].y * 1000) / 1000;

		if (min[0].x > cross_pos.x) 
			return 0;
		else if (max[0].x < cross_pos.x)
			return 0;
		else if (min[0].y > cross_pos.y) 
			return 0;
		else if (max[0].y < cross_pos.y)
			return 0;
		else if (min[1].x > cross_pos.x) 
			return 0;
		else if (max[1].x < cross_pos.x) 
			return 0;
		else if (min[1].y > cross_pos.y) 
			return 0;
		else if (max[1].y < cross_pos.y) 
			return 0;

		return_crossPos.push_back(cross_pos);
		// std::cout << cross_pos.x << "\t" << cross_pos.y << std::endl;
		return 1;
	}
}

void Cut_Polygon(std::vector <glm::vec3>& crossPos, int who, int crossWhere1, int crossWhere2) {
	GLSlice newPolygon1 = GLSlice(1);
	GLSlice newPolygon2 = GLSlice(1);
	std::vector <glm::vec3> Save;

	int save_crossWhere1 = 0, save_crossWhere2 = 0;
	for (int i = 0; i < Polygon1[who].firstpos.size(); ++i) {

		Save.push_back(Polygon1[who].firstpos[i]);

		if (crossWhere1 == i) {
			Save.push_back(crossPos[0]);
			save_crossWhere1 = Save.size();
		}
		if (crossWhere2 == i) {
			Save.push_back(crossPos[1]);
			save_crossWhere2 = Save.size();
		}
	}

	for (int i = save_crossWhere1 - 1; i < save_crossWhere2; ++i) {
		newPolygon1.firstpos.push_back(Save[i]);
	}

	for (int i = save_crossWhere2 - 1; i < Save.size() + save_crossWhere1; ++i) {
		if (i >= Save.size())
			newPolygon2.firstpos.push_back(Save[i - (Save.size())]);
		else
			newPolygon2.firstpos.push_back(Save[i]);
	}

	Polygon1[who].on = false;
	Polygon1.emplace_back(newPolygon1.firstpos, 0.005f);
	Polygon1.emplace_back(newPolygon2.firstpos, -0.005f);
}

void Init()
{ 
	glm::vec3 color[2]{
		{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
	};
	glGenBuffers(1, &MOUSE.v_color);
	glBindBuffer(GL_ARRAY_BUFFER, MOUSE.v_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	
	glGenBuffers(1, &MOUSE.v_pos);

	basket = Basket(1);
}

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
	vertexSource = filetobuf("vertex.glsl");
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