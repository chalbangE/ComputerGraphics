#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <math.h>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>#include <glm.hpp>
#include <ext.hpp>
#include <gtc/matrix_transform.hpp>
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<GLclampf> colorRd(0.0f, 1.0f);
std::uniform_int_distribution<int> HowManyPos(1, 5);
std::uniform_int_distribution<int> StateRd(1, 2);
std::uniform_real_distribution<GLfloat> make(-0.8f, 0.8f);
std::uniform_real_distribution<GLfloat> ThetaRd(0.0f, 360.0f);
std::uniform_real_distribution<GLfloat> dxdy(-0.015f, 0.015f);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

#define pie 3.141592653589793238

struct StructVBO
{
	GLuint pos, color;
	int posCnt = 0;
};
struct Points
{
	glm::vec3 move;
	glm::vec3 mid{ make(gen), make(gen), 1.0f };
	glm::vec3 dis{ dxdy(gen), dxdy(gen), 0.0f };
	int posCnt = HowManyPos(gen);
	int state = 0;
	glm::vec3 color{ colorRd(gen), colorRd(gen), colorRd(gen) };
	Points() {}
	Points(glm::vec3 m) : move(m) { }
};

void make_shaderProgram();void make_vertexShaders();void make_fragmentShaders();void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);GLvoid drawScene();GLvoid Reshape(int w, int h);void Clear();void InitBuffer();char* filetobuf(const char*);void Motion(int x, int y);bool PointsInTrue(Points mouse, std::vector < Points > Pos);void SortPoints(std::vector <glm::vec3> &move, std::vector <glm::vec3> &color, std::vector < Points > Pos);

float LengthPts(int x1, int y1, int x2, int y2);
bool InCircle(int x1, int y1, int x2, int y2);
GLuint vao;
std::vector <StructVBO> Vbo;
float winSizex = 800, winSizey = 800;
std::vector <std::vector < Points >> Pos;
int seclect = 999;
Points mouse;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#12");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	Clear();
	make_shaderProgram();
	InitBuffer();
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(10, TimerFunction, 1);
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

	for (int i = 0; i < Vbo.size(); ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].color); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

		if (Vbo[i].posCnt == 1) {
			glPointSize(7.0f);
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else if (Vbo[i].posCnt == 2) {
			glPointSize(3.0f);
			glDrawArrays(GL_LINES, 0, 2);
		}
		else if (Vbo[i].posCnt >= 3)
			glDrawArrays(GL_TRIANGLES, 0, (Vbo[i].posCnt - 2) * 3);
	}

	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);
	 
	glutSwapBuffers(); //--- 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'C':
	case 'c': {
		Clear();
		break;
	}
	}
	glutPostRedisplay(); // 화면 재 출력
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		// std::cout << Pos.size() << std::endl;
		for (int i = 0; i < Pos.size(); ++i) {
			if (Pos[i].begin()->state == 1) {
				bool turnx = false, turny = false;

				for (int k = 0; k < Pos[i].size(); ++k) {
					Pos[i][k].move += Pos[i].begin()->dis;
					Pos[i][k].mid += Pos[i].begin()->dis;

					if (Pos[i][k].move.x <= -1.0f || Pos[i][k].move.x >= 1.0f)
						turnx = true;
					else if (Pos[i][k].move.y <= -1.0f || Pos[i][k].move.y >= 1.0f)
						turny = true;
				}

				if (turnx) {
					Pos[i].begin()->dis.x *= -1;
				}
				else if (turny) {
					Pos[i].begin()->dis.y *= -1;
				}

				std::vector <glm::vec3> move, color;
				glm::vec3 movef[9], colorf[9];

				SortPoints(move, color, Pos[i]);

				for (int p = 0; p < move.size(); ++p) {
					movef[p].x = move[p].x;
					movef[p].y = move[p].y;
					movef[p].z = move[p].z;
				}

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(movef), movef, GL_STATIC_DRAW);
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

void Mouse(int button, int state, int x, int y)
{
	GLfloat n = 0.015f;

	glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };
	mouse = Points{m};

	if (state == GLUT_DOWN) {
		// 좌클릭 했을 때
		if (button == GLUT_LEFT_BUTTON) {
			for (int i = 0; i < Pos.size(); ++i) {
				if (PointsInTrue(mouse, Pos[i])) {
					seclect = i;
					Pos[seclect].begin()->state = 0;
					// std::cout << seclect << std::endl;
					break;
				}
			}
		}
		// 우클릭 했을 때
		else if (button == GLUT_RIGHT_BUTTON) {
		}
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON) {
			if (seclect != 999) {
				for (int k = 0; k < Pos[seclect].size(); ++k) {
					for (int i = 0; i < Pos.size(); ++i) {
						if (i != seclect && PointsInTrue(Pos[seclect][k], Pos[i])) {
							Points save;
							save.posCnt = Pos[seclect].begin()->posCnt + Pos[i].back().posCnt;
							if (save.posCnt > 5)
								save.posCnt -= 5;
							std::cout << save.posCnt << std::endl;
							save.mid = Pos[i].begin()->mid;

							Pos[seclect].clear();
							Pos[seclect].emplace_back(save);

							std::vector <glm::vec3> move, color;
							glm::vec3 movef[9], colorf[9];
							GLfloat Theta = ThetaRd(gen);
							GLfloat radian = Theta * 0.0174533f;
							GLfloat r = 0.15f;

							// 점 찍기
							for (int p = 0; p < Pos[seclect].begin()->posCnt; ++p) {
								radian = Theta * 0.0174533f;
								glm::vec3 m = { 0.0f, 0.0f, 0.0f };
								m.x = Pos[seclect].begin()->mid.x + r * cos(radian);
								m.y = Pos[seclect].begin()->mid.y + r * sin(radian);
								if (p == 0)
									Pos[seclect].begin()->move = m;
								else {
									Pos[seclect].emplace_back();
									Pos[seclect].back().posCnt = Pos[seclect].begin()->posCnt;
									Pos[seclect].back().move = m;
								}
								Theta += 360.0f / Pos[seclect].begin()->posCnt;
							}

							SortPoints(move, color, Pos[seclect]);

							for (int p = 0; p < move.size(); ++p) {
								movef[p].x = move[p].x;
								movef[p].y = move[p].y;
								movef[p].z = move[p].z;
								colorf[p].r = color[p].r;
								colorf[p].g = color[p].g;
								colorf[p].b = color[p].b;
							}
							Vbo[seclect].posCnt = Pos[seclect].begin()->posCnt;
							Pos[seclect].begin()->state = 1;

							glBindBuffer(GL_ARRAY_BUFFER, Vbo[seclect].pos);
							glBufferData(GL_ARRAY_BUFFER, sizeof(movef), movef, GL_STATIC_DRAW);

							Pos.erase(Pos.begin() + i);
							Vbo.erase(Vbo.begin() + i);
							seclect = 999;
							return;
						}
					}
				}
			}
			seclect = 999;
		}
	}
}void Motion(int x, int y)
{
	glm::vec3 m = { (x - (winSizex / 2)) / (winSizex / 2), -(y - (winSizey / 2)) / (winSizey / 2), 0.0f };
	Points mm = Points{ m };

	if (seclect != 999) {
		glm::vec3 dxdy = mouse.move - mm.move;

		glm::vec3 movef[9], colorf[9];
		for (int i = 0; i < Pos[seclect].size(); ++i) {
			std::vector <glm::vec3> move, color;

			Pos[seclect][i].move -= dxdy;
			Pos[seclect][i].mid -= dxdy;

			SortPoints(move, color, Pos[seclect]);

			for (int k = 0; k < move.size(); ++k) {
				// std::cout << move.size() << "\t" << k << std::endl;
				movef[k].x = move[k].x;
				movef[k].y = move[k].y;
				movef[k].z = move[k].z;
				colorf[k].r = color[k].r;
				colorf[k].g = color[k].g;
				colorf[k].b = color[k].b;
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[seclect].pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(movef), movef, GL_STATIC_DRAW);

		mouse = mm;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void Clear()
{
	for (int i = 0; i < Vbo.size(); ++i) {
		glDeleteBuffers(1, &Vbo[i].pos);
		glDeleteBuffers(1, &Vbo[i].color);
	}
	Vbo.clear();
	Pos.clear();

	GLfloat r = 0.15f;

	for (int i = 0; i < 10; ++i) {
		Pos.emplace_back();
		Pos[i].emplace_back();
	}

	std::vector <glm::vec3> move, color;
	glm::vec3 movef[9], colorf[9];
	for (int i = 0; i < Pos.size(); ++i) {
		GLfloat Theta = ThetaRd(gen);
		GLfloat radian = Theta * 0.0174533f;
		move.clear();
		color.clear();
		// std::cout << Pos[i][0].posCnt << std::endl;
		// 점 찍기
		for (int k = 0; k < Pos[i].begin()->posCnt; ++k) {
			radian = Theta * 0.0174533f;
			glm::vec3 m = { 0.0f, 0.0f, 0.0f };
			m.x = Pos[i].begin()->mid.x + r * cos(radian);
			m.y = Pos[i].begin()->mid.y + r * sin(radian);
			if (k == 0)
				Pos[i].begin()->move = m;
			else {
				Pos[i].emplace_back();
				Pos[i].back().posCnt = Pos[i].begin()->posCnt;
				Pos[i].back().move = m;
			}
			Theta += 360.0f / Pos[i].begin()->posCnt;
		}

		SortPoints(move, color, Pos[i]);

		for (int k = 0; k < move.size(); ++k) {
			// std::cout << move.size() << "\t" << k << std::endl;
			movef[k].x = move[k].x;
			movef[k].y = move[k].y;
			movef[k].z = move[k].z;
			colorf[k].r = color[k].r;
			colorf[k].g = color[k].g;
			colorf[k].b = color[k].b;
		}
		Vbo.emplace_back();
		Vbo.back().posCnt = Pos[i].back().posCnt;

		glGenBuffers(1, &Vbo.back().pos);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo.back().pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(movef), movef, GL_STATIC_DRAW);
		glGenBuffers(1, &Vbo.back().color);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo.back().color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colorf), colorf, GL_STATIC_DRAW);
	}
}

bool PointsInTrue(Points mouse, std::vector < Points > Pos){	int cnt = 0;
	GLfloat n = 0.01f;
	//std::cout << mouse.move.x << "\t" << mouse.move.y << std::endl;
	for (int i = 0; i < Pos.size(); ++i) {
		if (Pos.size() >= 3 && i + 1 < Pos.size()) {
			glm::vec3 pos1, pos2;

			pos1 = Pos[i].move;
			pos2 = Pos[i + 1].move;
			if (pos1.y < pos2.y) {
				glm::vec3 temp;
				temp = pos1;
				pos1 = pos2;
				pos2 = temp;
			}

			if (pos1.x == pos2.x /*두 점의 직선의 방정식이 세로로 곧을 때*/) {
				if (mouse.move.y <= pos1.y && mouse.move.y >= pos2.y
					&& mouse.move.x <= pos1.x) {
					cnt++;
				}
			}
			else {
				GLfloat m = ((pos2.y - pos1.y) / (pos2.x - pos1.x));

				if (mouse.move.x <= (mouse.move.y - pos1.y + (m * pos1.x)) / m
					&& mouse.move.y <= pos1.y && mouse.move.y >= pos2.y) {
					cnt++;
				}
			}
		}
		else if (Pos.size() >= 3) {
			glm::vec3 pos1, pos2;

			pos1 = Pos[i].move;
			pos2 = Pos.begin()->move;
			if (pos1.y < pos2.y) {
				glm::vec3 temp;
				temp = pos1;
				pos1 = pos2;
				pos2 = temp;
			}

			if (pos1.x == pos2.x /*두 점의 직선의 방정식이 세로로 곧을 때*/) {
				if (mouse.move.y <= pos1.y && mouse.move.y >= pos2.y
					&& mouse.move.x <= pos1.x) {
					cnt++;
					// std::cout << cnt << std::endl;
				}
			}
			else {
				GLfloat m = ((pos2.y - pos1.y) / (pos2.x - pos1.x));

				if (mouse.move.x <= (mouse.move.y - pos1.y + (m * pos1.x)) / m
					&& mouse.move.y <= pos1.y && mouse.move.y >= pos2.y) {
					cnt++;
				}
			}
		}	}
	if (Pos.size() == 2) {
		glm::vec3 pos1, pos2;
		n = 0.02f;

		pos1 = Pos.begin()->move;
		pos2 = Pos.back().move;
		if (pos1.y < pos2.y) {
			glm::vec3 temp;
			temp = pos1;
			pos1 = pos2;
			pos2 = temp;
		}

		if (pos1.x == pos2.x /*두 점의 직선의 방정식이 세로로 곧을 때*/) {
			if (mouse.move.y <= pos1.y && mouse.move.y >= pos2.y && mouse.move.x <= pos1.x + n &&  mouse.move.x >= pos1.x - n) {
				cnt++;
			}
		}
		else {
			GLfloat m = ((pos2.y - pos1.y) / (pos2.x - pos1.x));

			if (mouse.move.x <= (mouse.move.y - pos1.y + (m * pos1.x)) / m + n &&
				mouse.move.x >= (mouse.move.y - pos1.y + (m * pos1.x)) / m - n  
				&& mouse.move.y <= pos1.y && mouse.move.y >= pos2.y) {
				cnt++;
			}
		}	}
	else if (Pos.size() == 1) {		if (mouse.move.x >= Pos.back().move.x - n && mouse.move.x <= Pos.back().move.x + n
			&& mouse.move.y >= Pos.back().move.y - n && mouse.move.y <= Pos.back().move.y + n) {
			cnt++;
		}	}
	if (cnt % 2 == 1 /*마우스 포인터가 도형 안에 위치하면*/)
		return true;	else		return false;}

void SortPoints(std::vector<glm::vec3> &move, std::vector<glm::vec3> &color, std::vector<Points> Pos)
{
	// 점을 순서에 맞게 배치
	if (Pos.begin()->posCnt >= 3) {
		for (int k = 1; k < Pos.size() - 1; ++k) {
			move.push_back(Pos.begin()->move);
			color.push_back(Pos.begin()->color);

			move.push_back(Pos[k].move);
			color.push_back(Pos[k].color);

			move.push_back(Pos[k + 1].move);
			color.push_back(Pos[k + 1].color);
		}
	}
	else if (Pos.begin()->posCnt == 2) {
		move.push_back(Pos.begin()->move);
		color.push_back(Pos.begin()->color);

		move.push_back(Pos.back().move);
		color.push_back(Pos.back().color);
	}
	else {
		move.push_back(Pos.begin()->move);
		color.push_back(Pos.begin()->color);
		// std::cout << move.back().x << "\t" << move.back().y << std::endl;
	}
}


//--- (x1, y1)과 (x2, y2)간의 길이
float LengthPts(int x1, int y1, int x2, int y2)
{
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}
//--- (x1, y1)과 (x2, y2)의 길이가 반지름보다 짧으면 true, 아니면 false
bool InCircle(int x1, int y1, int x2, int y2)
{
	if (LengthPts(x1, y1, x2, y2) < 0.15f) //--- BSIZE: 반지름
		return true;
	else
		return false;
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