#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>#include <glm.hpp>
#include <ext.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <random>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);void TimerFunction(int value);
void Clear();void Mouse(int button, int state, int x, int y);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<GLclampf> dis(0.0f, 1.0f);

struct RGB
{
	GLclampf r = 1.0f, g = 1.0f, b = 1.0f;
};

struct Yrect
{
	GLfloat x = 0.0, y = 0.0, w = 0.0, h = 0.0;
	RGB c;
	bool small = false;
};

float winSizex = 800, winSizey = 600;
Yrect backrect[4]{}, smallrect[4]{};
int Sizesmallrt = 4; // 4는 기본 상태, 0 1 2 3 을 배열에 넣어 사이즈를 바꾸고 다시 4로 만들기

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
	
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutTimerFunc(10, TimerFunction, 1);
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 ‘blue’ 로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	
	for (int i = 0; i < 4; ++i) {
		glColor3f(backrect[i].c.r, backrect[i].c.g, backrect[i].c.b);
		glRectf(backrect[i].x - backrect[i].w, backrect[i].y - backrect[i].h, backrect[i].x + backrect[i].w, backrect[i].y + backrect[i].h);
		glColor3f(smallrect[i].c.r, smallrect[i].c.g, smallrect[i].c.b);
		glRectf(smallrect[i].x - smallrect[i].w, smallrect[i].y - smallrect[i].h, smallrect[i].x + smallrect[i].w, smallrect[i].y + smallrect[i].h);
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

	}
	glutPostRedisplay(); // 화면 재 출력
}void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		if (Sizesmallrt != 4) {
			if (smallrect[Sizesmallrt].small == false) {
				smallrect[Sizesmallrt].w -= 0.05f;
				smallrect[Sizesmallrt].h -= 0.05f;

				if (smallrect[Sizesmallrt].w <= 0.3f) {
					smallrect[Sizesmallrt].small = true;
					Sizesmallrt = 4;
				}
			}
			else if (smallrect[Sizesmallrt].small == true) {
				smallrect[Sizesmallrt].w += 0.05f;
				smallrect[Sizesmallrt].h += 0.05f;

				if (smallrect[Sizesmallrt].w >= 0.5f) {
					smallrect[Sizesmallrt].small = false;
					Sizesmallrt = 4;
				}
			}

			glutTimerFunc(10, TimerFunction, 1);
		}

		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void Clear()
{
	backrect[0].x  = smallrect[0].x = -0.5f;
	backrect[0].y = smallrect[0].y = 0.5f;
	backrect[1].x = smallrect[1].x = 0.5f;
	backrect[1].y = smallrect[1].y = 0.5f;
	backrect[2].x = smallrect[2].x = -0.5f;
	backrect[2].y = smallrect[2].y = -0.5f;
	backrect[3].x = smallrect[3].x = 0.5f;
	backrect[3].y = smallrect[3].y = -0.5f;

	for (int i = 0; i < 4; ++i) {
		backrect[i].w = smallrect[i].w = 0.5f;
		backrect[i].h = smallrect[i].h = 0.5f;

		backrect[i].c.r =  dis(gen);
		backrect[i].c.g = dis(gen);
		backrect[i].c.b = dis(gen);
		smallrect[i].c.r =  dis(gen);
		smallrect[i].c.g = dis(gen);
		smallrect[i].c.b = dis(gen);
	}
}void Mouse(int button, int state, int x, int y)
{
	GLfloat mx = 0.0f, my = 0.0f;

	mx = (x - (winSizex / 2)) / (winSizex / 2);
	my = -(y - (winSizey / 2)) / (winSizey / 2); 
		
	if (state == GLUT_DOWN) {
		// 좌클릭 했을 때
		if (button == GLUT_LEFT_BUTTON) {
			for (int i = 0; i < 4; ++i) {
				// smallrect 안에 좌클릭 했을 때
				if (smallrect[i].x - smallrect[i].w <= mx && smallrect[i].x + smallrect[i].w >= mx &&
					smallrect[i].y - smallrect[i].h <= my && smallrect[i].y + smallrect[i].h >= my) {
					smallrect[i].c.r = dis(gen);
					smallrect[i].c.g = dis(gen);
					smallrect[i].c.b = dis(gen);
				}
				// backrect 만 좌클릭
				else if (backrect[i].x - backrect[i].w <= mx && backrect[i].x + backrect[i].w >= mx &&
					backrect[i].y - backrect[i].h <= my && backrect[i].y + backrect[i].h >= my) {
					backrect[i].c.r = dis(gen);
					backrect[i].c.g = dis(gen);
					backrect[i].c.b = dis(gen);
				}
			}
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			for (int i = 0; i < 4; ++i) {
				if (smallrect[i].x - smallrect[i].w <= mx && smallrect[i].x + smallrect[i].w >= mx &&
					smallrect[i].y - smallrect[i].h <= my && smallrect[i].y + smallrect[i].h >= my) {
					Sizesmallrt = i;
					glutTimerFunc(10, TimerFunction, 1);
				}
			}
		}
	}
}