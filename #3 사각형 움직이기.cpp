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
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);
void Clear();void Mouse(int button, int state, int x, int y);void Motion(int x, int y);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<GLclampf> dis(0.0f, 1.0f);
std::uniform_real_distribution<GLfloat> make(-1.0f, 1.0f);

struct RGB
{
	GLclampf r = 1.0f, g = 1.0f, b = 1.0f;
};

struct Yrect
{
	GLfloat x = 0.0, y = 0.0, w = 0.0, h = 0.0;
	RGB c;
};

float winSizex = 800, winSizey = 600;
std::vector<Yrect> rt;
int choice = 0;
bool Lbt = false;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(winSizex, winSizey); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성 (윈도우 이름 )

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { // glew 초기화 
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	Clear();
	glutDisplayFunc(drawScene); // 출력 함수의 지정

	glutMouseFunc(Mouse); 	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutTimerFunc(10, TimerFunction, 1);
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	for (int i = 0; i < rt.size(); ++i) {
		if (i == choice) {
			glColor3f(0.0f, 0.0f, 0.0f);
			glRectf(rt[i].x - (rt[i].w + 0.01f), rt[i].y - (rt[i].h + 0.01f), rt[i].x + (rt[i].w + 0.01f), rt[i].y + (rt[i].h + 0.01f));
		}
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
	case 'a': {
		Yrect a;
		a.x = make(gen);
		a.y = make(gen);
		a.w = 0.15f;
		a.h = 0.15f;

		a.c.r = dis(gen);
		a.c.g = dis(gen);
		a.c.b = dis(gen);

		rt.push_back(a);
		if (rt.size() == 6) {
			rt.erase(rt.begin());
		}
		break;
	}
	}
	glutPostRedisplay(); // 화면 재 출력
}void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {

		glutTimerFunc(10, TimerFunction, 1);

		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void Clear()
{
	rt.emplace_back();
	rt[0].x = 0.0f;
	rt[0].y = 0.0f;
	rt[0].w = 0.15f;
	rt[0].h = 0.15f;

	rt[0].c.r = dis(gen);
	rt[0].c.g = dis(gen);
	rt[0].c.b = dis(gen);
}void Mouse(int button, int state, int x, int y)
{
	GLfloat mx = 0.0f, my = 0.0f;

	mx = (x - (winSizex / 2)) / (winSizex / 2);
	my = -(y - (winSizey / 2)) / (winSizey / 2);

	if (state == GLUT_DOWN) {
		// 좌클릭 했을 때
		if (button == GLUT_LEFT_BUTTON) {
			for (int i = 0; i < rt.size(); ++i) {
				if (rt[i].x - rt[i].w <= mx && rt[i].x + rt[i].w >= mx &&
					rt[i].y - rt[i].h <= my && rt[i].y + rt[i].h >= my) {
					choice = i;
				}
			}
			Lbt = true;
		}
	}
	else {
		Lbt = false;
	}
}void Motion(int x, int y)
{
	GLfloat mx = 0.0f, my = 0.0f;

	mx = (x - (winSizex / 2)) / (winSizex / 2);
	my = -(y - (winSizey / 2)) / (winSizey / 2);

	if (Lbt == true) {
		if (rt[choice].x - rt[choice].w <= mx && rt[choice].x + rt[choice].w >= mx &&
			rt[choice].y - rt[choice].h <= my && rt[choice].y + rt[choice].h >= my) {
			rt[choice].x = mx;
			rt[choice].y = my;
		}
	}
}
