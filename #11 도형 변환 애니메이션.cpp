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
std::uniform_real_distribution<GLfloat> make(0.05f, 0.2f);
std::uniform_real_distribution<GLfloat> dxdy(-0.015f, 0.015f);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

#define pie 3.141592653589793238
GLfloat OneGo = 80.0f;

void make_shaderProgram();void make_vertexShaders();void make_fragmentShaders();void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);GLvoid drawScene();GLvoid Reshape(int w, int h);void Clear();void InitBuffer();char* filetobuf(const char*);
struct StructVBO
{
	GLuint pos, color;
};
struct Points
{
	glm::vec3 move;
	glm::vec3 color{ colorRd(gen), colorRd(gen), colorRd(gen)};
	glm::vec3 dis;
	Points() {}
	Points(glm::vec3 m, glm::vec3 g) : move(m), dis((m - g) / OneGo * -1.0f){ }
};


GLuint vao;
std::vector <StructVBO> Vbo;
float winSizex = 800, winSizey = 800;
glm::vec3 MxMy[4]{ {-0.5f, 0.5f, 1.0f},{0.5f, 0.5f, 1.0f},{-0.5f, -0.5f, 1.0f},{0.5f, -0.5f, 1.0f} };
Points LineToTri[3];
Points TriToRect[6];
Points RectToPen[9];
Points Pentagon[9];int Timecnt = 0, Mod = 0;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#11");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Clear();
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
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
	// std::cout << LineToTri[0].move.x << "  " << LineToTri[0].move.y << std::endl;

	if (Mod == 0) {
		for (int i = 5; i >= 0; --i) {
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].pos); // VBO Bind
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[i].color); // VBO Bind
			glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			// std::cout << i << std::endl;

			if (i == 3 || i == 2 || i == 1) {
				glDrawArrays(GL_TRIANGLES, 0, 3 * (i + 1));
				glPointSize(2.0f);
				glDrawArrays(GL_POINTS, 0, 3 * (i + 1));
			}
			else {
				glDrawArrays(GL_LINES, 0, 3);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}
		}
	}
	else if (Mod == 1) {
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].pos); // VBO Bind
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].color); // VBO Bind
			glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			// std::cout << i << std::endl;


			glDrawArrays(GL_LINES, 0, 3);
			glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	else if (Mod == 2) {
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].pos); // VBO Bind
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].color); // VBO Bind
			glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			// std::cout << i << std::endl;


			glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else if (Mod == 3) {
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].pos); // VBO Bind
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].color); // VBO Bind
			glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			// std::cout << i << std::endl;


			glDrawArrays(GL_TRIANGLES, 0, 9);
	}
	else if (Mod == 4) {
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[3].pos); // VBO Bind
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[3].color); // VBO Bind
			glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			// std::cout << i << std::endl;


			glDrawArrays(GL_TRIANGLES, 0, 9);
			glPointSize(2.0f);
			glDrawArrays(GL_POINTS, 0, 9);
	}

	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);

	glutSwapBuffers(); //--- 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'I':
	case 'i': {
		Mod = 1;
		break;
	}
	case 'T':
	case 't': {
		Mod = 2;
		break;
	}
	case 'R':
	case 'r': {
		Mod = 3;
		break;
	}
	case 'P':
	case 'p': {
		Mod = 4;
		break;
	}
	case 'A':
	case 'a': {
		Mod = 0;
		break;
	}
	}
	Clear();
	glutPostRedisplay(); // 화면 재 출력
}

void TimerFunction(int value)
{
	bool on = true;

	switch (value)
	{
	case 1: {		
		if (Mod == 0 || Mod == 1) {			// LineToTri
			{				for (int i = 0; i < 3; ++i) {
					LineToTri[i].move.x += LineToTri[i].dis.x;
					LineToTri[i].move.y += LineToTri[i].dis.y;
				}

				glm::vec3 move1[3], color1[3];
				for (int i = 0; i < 3; ++i) {
					move1[i] = LineToTri[i].move;
					color1[i] = LineToTri[i].color;
				}

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(move1), move1, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].color);
				glBufferData(GL_ARRAY_BUFFER, sizeof(color1), color1, GL_DYNAMIC_DRAW);			}		}		if (Mod == 0 || Mod == 2) {			// TriToRect
			{
				for (int i = 0; i < 6; ++i) {
					TriToRect[i].move.x += TriToRect[i].dis.x;
					TriToRect[i].move.y += TriToRect[i].dis.y;
				}

				glm::vec3 move2[6], color2[6];
				for (int i = 0; i < 6; ++i) {
					move2[i] = TriToRect[i].move;
					color2[i] = TriToRect[i].color;
				}

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(move2), move2, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].color);
				glBufferData(GL_ARRAY_BUFFER, sizeof(color2), color2, GL_DYNAMIC_DRAW);
			}		}		if (Mod == 0 || Mod == 3) {			// RectToPen
			{
				for (int i = 0; i < 9; ++i) {
					RectToPen[i].move.x += RectToPen[i].dis.x;
					RectToPen[i].move.y += RectToPen[i].dis.y;
				}

				glm::vec3 move3[9], color3[9];
				for (int i = 0; i < 9; ++i) {
					move3[i] = RectToPen[i].move;
					color3[i] = RectToPen[i].color;
				}

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(move3), move3, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].color);
				glBufferData(GL_ARRAY_BUFFER, sizeof(color3), color3, GL_DYNAMIC_DRAW);
			}		}		if (Mod == 0 || Mod == 4) {
			// Pentagon
			{
				for (int i = 0; i < 9; ++i) {
					Pentagon[i].move.x += Pentagon[i].dis.x;
					Pentagon[i].move.y += Pentagon[i].dis.y;
				}

				glm::vec3 move4[9], color4[9];
				for (int i = 0; i < 9; ++i) {
					move4[i] = Pentagon[i].move;
					color4[i] = Pentagon[i].color;
				}

				glBindBuffer(GL_ARRAY_BUFFER, Vbo[3].pos);
				glBufferData(GL_ARRAY_BUFFER, sizeof(move4), move4, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, Vbo[3].color);
				glBufferData(GL_ARRAY_BUFFER, sizeof(color4), color4, GL_DYNAMIC_DRAW);
			}
		}

		Timecnt++;
		if (Timecnt == OneGo)
			on = false;
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
	if (on) {
		glutTimerFunc(10, TimerFunction, 1);
	}
}

void Mouse(int button, int state, int x, int y)
{
	GLfloat mx = 0.0f, my = 0.0f;
	StructVBO temp;

	mx = (x - (winSizex / 2)) / (winSizex / 2);
	my = -(y - (winSizey / 2)) / (winSizey / 2);

	if (state == GLUT_DOWN) {
		// 좌클릭 했을 때
		if (button == GLUT_LEFT_BUTTON) {
			Clear();
			glutTimerFunc(10, TimerFunction, 1);
		}
	}
}

void Clear()
{
	for (int i = 0; i < Vbo.size(); ++i) {
		glDeleteBuffers(1, &Vbo[i].pos);
		glDeleteBuffers(1, &Vbo[i].color);
	}
	Vbo.clear();
	Timecnt = 0;
	glm::vec3 MM = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 6; ++i) {
		Vbo.emplace_back();
	}
	if (Mod == 0) {
		std::cout << " 눌럿다 " << std::endl;
		GLfloat n = 0.2f;
		GLfloat pen = 0.25f;

		// 선 -> 삼각형
		{
			glm::vec3 m = { 0.0f, 0.0f, 0.0f }, g;
			g = m;
			m.x = MxMy[0].x + n;
			m.y = MxMy[0].y + n;
			g.x = MxMy[0].x;
			g.y = MxMy[0].y + n;
			LineToTri[0] = Points(m, g);

			m.x = MxMy[0].x - n;
			m.y = MxMy[0].y - n;
			g = m;
			LineToTri[1] = Points(m, g);

			m = LineToTri[0].move;
			g.x = MxMy[0].x + n;
			g.y = MxMy[0].y - n;
			LineToTri[2] = Points(m, g);

			glm::vec3 move[3], color[3];
			for (int i = 0; i < 3; ++i) {
				move[i] = LineToTri[i].move;
				color[i] = LineToTri[i].color;
			}

			glGenBuffers(1, &Vbo[0].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &Vbo[0].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
		}

		// 삼각형 -> 사각형
		{
			glm::vec3 m = { 0.0f, 0.0f, 0.0f }, g;
			g = m;

			m.x = MxMy[1].x;
			m.y = MxMy[1].y + n;
			g.x = MxMy[1].x - n;
			g.y = MxMy[1].y + n;
			TriToRect[0] = Points(m, g);

			m.x = MxMy[1].x - n;
			m.y = MxMy[1].y - n;
			g = m;
			TriToRect[1] = Points(m, g);

			m.x = MxMy[1].x + n;
			m.y = MxMy[1].y - n;
			g = m;
			TriToRect[2] = Points(m, g);

			m.x = MxMy[1].x;
			m.y = MxMy[1].y + n;
			g.x = MxMy[1].x - n;
			g.y = MxMy[1].y + n;
			TriToRect[3] = Points(m, g);

			m.x = MxMy[1].x + n;
			m.y = MxMy[1].y - n;
			g = m;
			TriToRect[4] = Points(m, g);

			m.x = MxMy[1].x;
			m.y = MxMy[1].y + n;
			g.x = MxMy[1].x + n;
			g.y = MxMy[1].y + n;
			TriToRect[5] = Points(m, g);

			glm::vec3 move[6], color[6];
			for (int i = 0; i < 6; ++i) {
				move[i] = TriToRect[i].move;
				color[i] = TriToRect[i].color;
			}

			glGenBuffers(1, &Vbo[1].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &Vbo[1].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
		}

		// 사각형 -> 오각형
		{
			glm::vec3 m = { 0.0f, 0.0f, 0.0f }, g;
			g = m;

			m.x = MxMy[2].x - n;
			m.y = MxMy[2].y + n;
			g.x = MxMy[2].x;
			g.y = MxMy[2].y + pen;
			RectToPen[0] = Points(m, g);
			RectToPen[3] = Points(m, g);
			RectToPen[6] = Points(m, g);

			m.x = MxMy[2].x - n;
			m.y = MxMy[2].y - n;
			g.x = MxMy[2].x - (pen / 5 * 3);
			g.y = MxMy[2].y - pen;
			RectToPen[1] = Points(m, g);
			RectToPen[8] = Points(m, g);

			m.x = MxMy[2].x + n;
			m.y = MxMy[2].y - n;
			g.x = MxMy[2].x + (pen / 5 * 3);
			g.y = MxMy[2].y - pen;
			RectToPen[2] = Points(m, g);
			RectToPen[4] = Points(m, g);

			m.x = MxMy[2].x + n;
			m.y = MxMy[2].y + n;
			g.x = MxMy[2].x + pen;
			g.y = MxMy[2].y + (pen / 5);
			RectToPen[5] = Points(m, g);

			m.x = MxMy[2].x - n;
			m.y = MxMy[2].y + n;
			g.x = MxMy[2].x - pen;
			g.y = MxMy[2].y + (pen / 5);
			RectToPen[7] = Points(m, g);

			glm::vec3 move[9], color[9];
			for (int i = 0; i < 9; ++i) {
				move[i] = RectToPen[i].move;
				color[i] = RectToPen[i].color;
			}

			glGenBuffers(1, &Vbo[2].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &Vbo[2].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
		}

		// 오각형 -> 점
		{
			glm::vec3 m = { 0.0f, 0.0f, 0.0f }, g;
			g = m;

			m.x = MxMy[3].x;
			m.y = MxMy[3].y + pen;
			g = MxMy[3];
			Pentagon[0] = Points(m, g);
			Pentagon[3] = Points(m, g);
			Pentagon[6] = Points(m, g);

			m.x = MxMy[3].x - (pen / 5 * 3);
			m.y = MxMy[3].y - pen;
			g = MxMy[3];
			Pentagon[1] = Points(m, g);
			Pentagon[8] = Points(m, g);

			m.x = MxMy[3].x + (pen / 5 * 3);
			m.y = MxMy[3].y - pen;
			g = MxMy[3];
			Pentagon[2] = Points(m, g);
			Pentagon[4] = Points(m, g);

			m.x = MxMy[3].x + pen;
			m.y = MxMy[3].y + (pen / 5);
			g = MxMy[3];
			Pentagon[5] = Points(m, g);

			m.x = MxMy[3].x - pen;
			m.y = MxMy[3].y + (pen / 5);
			g = MxMy[3];
			Pentagon[7] = Points(m, g);

			glm::vec3 move[9], color[9];
			for (int i = 0; i < 9; ++i) {
				move[i] = Pentagon[i].move;
				color[i] = Pentagon[i].color;
			}

			glGenBuffers(1, &Vbo[3].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[3].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &Vbo[3].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[3].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
		}

		// x축 y축 선긋기
		{
			GLfloat pos[2][3] = { {0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 1.0f} };
			GLfloat color[2][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

			glGenBuffers(1, &Vbo[4].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[4].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
			glGenBuffers(1, &Vbo[4].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[4].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

			GLfloat pos1[2][3] = { {-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f} };
			GLfloat color1[2][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

			glGenBuffers(1, &Vbo[5].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[5].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pos1), pos1, GL_STATIC_DRAW);
			glGenBuffers(1, &Vbo[5].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[5].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color1), color1, GL_STATIC_DRAW);
		}
	}
	else if (Mod == 1 /*선 -> 삼각형*/ ) {
		GLfloat n = 0.7f;
		// 선 -> 삼각형
		{
			glm::vec3 m = { 0.0f, 0.0f, 0.0f }, g;
			g = m;

			m.x = MM.x + n;
			m.y = MM.y + n;
			g.x = MM.x;
			g.y = MM.y + n;
			LineToTri[0] = Points(m, g);

			m.x = MM.x - n;
			m.y = MM.y - n;
			g = m;
			LineToTri[1] = Points(m, g);

			m = LineToTri[0].move;
			g.x = MM.x + n;
			g.y = MM.y - n;
			LineToTri[2] = Points(m, g);

			glm::vec3 move[3], color[3];
			for (int i = 0; i < 3; ++i) {
				move[i] = LineToTri[i].move;
				color[i] = LineToTri[i].color;
			}

			glGenBuffers(1, &Vbo[0].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &Vbo[0].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[0].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
		}
	}
	else if (Mod == 2 /*삼각형 -> 사각형*/ ) {
		GLfloat n = 0.7f;		
		glm::vec3 m = { 0.0f, 0.0f, 0.0f }, g;
		g = m;

		m.x = MM.x;
		m.y = MM.y + n;
		g.x = MM.x - n;
		g.y = MM.y + n;
		TriToRect[0] = Points(m, g);

		m.x = MM.x - n;
		m.y = MM.y - n;
		g = m;
		TriToRect[1] = Points(m, g);

		m.x = MM.x + n;
		m.y = MM.y - n;
		g = m;
		TriToRect[2] = Points(m, g);

		m.x = MM.x;
		m.y = MM.y + n;
		g.x = MM.x - n;
		g.y = MM.y + n;
		TriToRect[3] = Points(m, g);

		m.x = MM.x + n;
		m.y = MM.y - n;
		g = m;
		TriToRect[4] = Points(m, g);

		m.x = MM.x;
		m.y = MM.y + n;
		g.x = MM.x + n;
		g.y = MM.y + n;
		TriToRect[5] = Points(m, g);

		glm::vec3 move[6], color[6];
		for (int i = 0; i < 6; ++i) {
			move[i] = TriToRect[i].move;
			color[i] = TriToRect[i].color;
		}

		glGenBuffers(1, &Vbo[1].pos);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_DYNAMIC_DRAW);
		glGenBuffers(1, &Vbo[1].color);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo[1].color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
	}
	else if (Mod == 3 /*사각형 -> 오각형*/ ) {
		GLfloat n = 0.7f;
		GLfloat pen = 0.8f;
		// 사각형 -> 오각형
		{
			glm::vec3 m = { 0.0f, 0.0f, 0.0f }, g;
			g = m;

			m.x = MM.x - n;
			m.y = MM.y + n;
			g.x = MM.x;
			g.y = MM.y + pen;
			RectToPen[0] = Points(m, g);
			RectToPen[3] = Points(m, g);
			RectToPen[6] = Points(m, g);

			m.x = MM.x - n;
			m.y = MM.y - n;
			g.x = MM.x - (pen / 5 * 3);
			g.y = MM.y - pen;
			RectToPen[1] = Points(m, g);
			RectToPen[8] = Points(m, g);

			m.x = MM.x + n;
			m.y = MM.y - n;
			g.x = MM.x + (pen / 5 * 3);
			g.y = MM.y - pen;
			RectToPen[2] = Points(m, g);
			RectToPen[4] = Points(m, g);

			m.x = MM.x + n;
			m.y = MM.y + n;
			g.x = MM.x + pen;
			g.y = MM.y + (pen / 5);
			RectToPen[5] = Points(m, g);

			m.x = MM.x - n;
			m.y = MM.y + n;
			g.x = MM.x - pen;
			g.y = MM.y + (pen / 5);
			RectToPen[7] = Points(m, g);

			glm::vec3 move[9], color[9];
			for (int i = 0; i < 9; ++i) {
				move[i] = RectToPen[i].move;
				color[i] = RectToPen[i].color;
			}

			glGenBuffers(1, &Vbo[2].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &Vbo[2].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[2].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
		}
	}
	else if (Mod == 4 /*오각형 -> 점*/ ) {
		GLfloat n = 0.7f;
		GLfloat pen = 0.8f;
		// 오각형 -> 점
		{
			glm::vec3 m = { 0.0f, 0.0f, 0.0f }, g;
			g = m;

			m.x = MM.x;
			m.y = MM.y + pen;
			g = MM;
			Pentagon[0] = Points(m, g);
			Pentagon[3] = Points(m, g);
			Pentagon[6] = Points(m, g);

			m.x = MM.x - (pen / 5 * 3);
			m.y = MM.y - pen;
			g = MM;
			Pentagon[1] = Points(m, g);
			Pentagon[8] = Points(m, g);

			m.x = MM.x + (pen / 5 * 3);
			m.y = MM.y - pen;
			g = MM;
			Pentagon[2] = Points(m, g);
			Pentagon[4] = Points(m, g);

			m.x = MM.x + pen;
			m.y = MM.y + (pen / 5);
			g = MM;
			Pentagon[5] = Points(m, g);

			m.x = MM.x - pen;
			m.y = MM.y + (pen / 5);
			g = MM;
			Pentagon[7] = Points(m, g);

			glm::vec3 move[9], color[9];
			for (int i = 0; i < 9; ++i) {
				move[i] = Pentagon[i].move;
				color[i] = Pentagon[i].color;
			}

			glGenBuffers(1, &Vbo[3].pos);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[3].pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(move), move, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &Vbo[3].color);
			glBindBuffer(GL_ARRAY_BUFFER, Vbo[3].color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
		}
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