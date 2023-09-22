#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>#include <glm.hpp>
#include <ext.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <vector>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);void Mouse(int button, int state, int x, int y);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<GLclampf> dis(0.0f, 1.0f);
std::uniform_real_distribution<GLfloat> DxDy(-0.01f, 0.01f);

struct RGB
{
	GLclampf r = dis(gen), g = dis(gen), b = dis(gen);
};

struct Yrect
{
	GLfloat x = 0.0f, y = 0.0f, w = 0.08f, h = 0.08f;
	GLfloat dx = DxDy(gen), dy = DxDy(gen);
	RGB c;
};
struct xy
{
	GLfloat x = 0.0f, y = 0.0f;
};

float winSizex = 800, winSizey = 600;
std::vector<Yrect> rt;
std::vector<xy> savexy;
GLfloat z = 0.006f;
bool Lbt = false, aon = false, ion = false, con = false, oon = false, son = true;
int cnt = 0;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(winSizex, winSizey); // 윈도우의 크기 지정
	glutCreateWindow("YuBin"); // 윈도우 생성 (윈도우 이름 )

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { // glew 초기화 
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW 잘 깔렸다용\n";

	glutDisplayFunc(drawScene); // 출력 함수의 지정

	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutTimerFunc(10, TimerFunction, 1);
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // 바탕색
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	for (int i = 0; i < rt.size(); ++i) {
		glColor3f(rt[i].c.r, rt[i].c.g, rt[i].c.b);
		glRectf(rt[i].x - rt[i].w, rt[i].y - rt[i].h, rt[i].x + rt[i].w, rt[i].y + rt[i].h);
	}

	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	winSizex = w;
	winSizey = h;
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
	case 'A': {
		if (aon)
			aon = false;
		else
			aon = true;
		break;
	}
	case 'i':
	case 'I': {
		if (ion)
			ion = false;
		else
			ion = true;
		break;
	}
	case 'c':
	case 'C': {
		if (con)
			con = false;
		else
			con = true;
		break;
	}
	case 'O':
	case 'o': {
		if (oon)
			oon = false;
		else
			oon = true;
		break;
	}
	case 's':
	case 'S': {
		if (son)
			son = false;
		else {
			son = true;
			glutTimerFunc(10, TimerFunction, 1);
		}
		break;
	}
	case 'm':
	case 'M': {
		for (int i = 0; i < rt.size(); ++i) {
			rt[i].x = savexy[i].x;
			rt[i].y = savexy[i].y;
		}
		break;
	}
	case 'r':
	case 'R': {
		rt.clear();
		aon = ion = con = oon = false;
		son = true;
		break;
	}
	}
	glutPostRedisplay(); // 화면 재 출력
}void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		if (aon) {
			for (int i = 0; i < rt.size(); ++i) {
				rt[i].x += rt[i].dx;
				rt[i].y += rt[i].dy;

				if (rt[i].x - rt[i].w <= -1.0f || rt[i].x + rt[i].w >= 1.0f)
					rt[i].dx *= -1;
				else if (rt[i].y - rt[i].h <= -1.0f || rt[i].y + rt[i].h >= 1.0f)
					rt[i].dy *= -1;
			}
		}

		if (ion) {
			for (int i = 0; i < rt.size(); ++i) {
				rt[i].x += 0.006f;
				rt[i].y += z;

				if (rt[i].x >= 1.0f) {
					rt[i].x = -1.0f;
				}
			}
			cnt++;

			if (cnt >= 40) {
				z *= -1; 
				cnt = 0;
			}
		}

		if (con) {
			static GLfloat thin = 0.003f;
			for (int i = 0; i < rt.size(); ++i) {
				rt[i].w += thin;
				rt[i].h -= thin;
			}
			if (rt.size() >= 1 && (rt[0].w >= 0.16f || rt[0].w <= 0.0f))
				thin *= -1;
		}

		if (oon) {
			static int ocnt = 0;
			ocnt++;
			if (ocnt >= 40) {
				for (int i = 0; i < rt.size(); ++i) {
					rt[i].c.r = dis(gen);
					rt[i].c.g = dis(gen);
					rt[i].c.b = dis(gen);
				}
				ocnt = 0;
			}
		}

		if (son)
			glutTimerFunc(10, TimerFunction, 1);
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}
void Mouse(int button, int state, int x, int y)
{
	GLfloat mx = 0.0f, my = 0.0f;

	mx = (x - (winSizex / 2)) / (winSizex / 2);
	my = -(y - (winSizey / 2)) / (winSizey / 2);

	if (state == GLUT_DOWN) {
		// 좌클릭 했을 때
		if (button == GLUT_LEFT_BUTTON) {
			rt.emplace_back();
			rt.back().x = mx;
			rt.back().y = my;
			savexy.emplace_back();
			savexy.back().x = mx;
			savexy.back().y = my;

			if (rt.size() > 5) {
				rt.erase(rt.begin());
				savexy.erase(savexy.begin());
			}
		}
	}
	else {
		Lbt = false;
	}
}
