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
void Clear();void Mouse(int button, int state, int x, int y);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<GLclampf> dis(0.0f, 1.0f);
std::uniform_real_distribution<GLfloat> make(-0.8f, 0.8f);
std::uniform_real_distribution<GLfloat> wh(0.02f, 0.1f);
std::uniform_int_distribution<int> Mt(0, 2);

struct RGB
{
	GLclampf r = 1.0f, g = 1.0f, b = 1.0f;
};

struct Yrect
{
	GLfloat x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, mx = 0.0f, my = 0.0f;
	RGB c;
	int motion = Mt(gen);
};

float winSizex = 800, winSizey = 800;
std::vector<Yrect> rt;
std::vector<Yrect> movert;
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

	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutTimerFunc(10, TimerFunction, 1);
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	for (int i = rt.size() - 1; i >= 0; --i) {
		glColor3f(rt[i].c.r, rt[i].c.g, rt[i].c.b);
		glRectf(rt[i].x - rt[i].w, rt[i].y - rt[i].h, rt[i].x + rt[i].w, rt[i].y + rt[i].h);
	}
	for (int i = 0; i < movert.size(); ++i) {
		glColor3f(movert[i].c.r, movert[i].c.g, movert[i].c.b);
		glRectf(movert[i].x - movert[i].w, movert[i].y - movert[i].h, movert[i].x + movert[i].w, movert[i].y + movert[i].h);
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
	case 'r': {
		rt.clear();
		Clear();
		break;
	}
	}
	glutPostRedisplay(); // 화면 재 출력
}void TimerFunction(int value)
{
	switch (value)
	{
	case 1: {
		for (int i = 0; i < movert.size(); ++i) {
			movert[i].x += movert[i].mx;
			movert[i].y += movert[i].my;

			movert[i].w += -0.0008f;
			movert[i].h += -0.0008f;

			if (movert[i].w <= 0.0f) {
				movert.erase(movert.begin() + i);
				--i;
			}
		}

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
	for (int i = 0; i < 5; ++i) {
		rt.emplace_back();
		rt[i].x = make(gen);
		rt[i].y = make(gen);
		rt[i].w = rt[i].h = 0.2f;
		rt[i].c.r = dis(gen);
		rt[i].c.g = dis(gen);
		rt[i].c.b = dis(gen);
	}
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

					GLfloat s = (rt[i].w / 2);

					switch (rt[i].motion)
					{
						// 좌우상하 이동
					case 0: {
						// 상
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x - s;
						movert.back().y = rt[i].y + s;
						movert.back().h = movert.back().w = (rt[i].w / 2);
						movert.back().my = 0.003f;
						// 우
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x + s;
						movert.back().y = rt[i].y + s;
						movert.back().h = movert.back().w = (rt[i].w / 2);
						movert.back().mx = 0.003f;
						// 하
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x + s;
						movert.back().y = rt[i].y - s;
						movert.back().h = movert.back().w = (rt[i].w / 2);
						movert.back().my = -0.003f;
						// 좌
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x - s;
						movert.back().y = rt[i].y - s;
						movert.back().h = movert.back().w = (rt[i].w / 2);
						movert.back().mx = -0.003f;
						break;
					}
						// 대각선 이동
					case 1: {
						// 좌상
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x - s;
						movert.back().y = rt[i].y + s;
						movert.back().h = movert.back().w = (rt[i].w / 2);
						movert.back().my = 0.003f;
						movert.back().mx = -0.003f;
						// 우상
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x + s;
						movert.back().y = rt[i].y + s;
						movert.back().h = movert.back().w = (rt[i].w / 2);
						movert.back().mx = 0.003f;
						movert.back().my = 0.003f;
						// 우하
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x + s;
						movert.back().y = rt[i].y - s;
						movert.back().h = movert.back().w = (rt[i].w / 2);
						movert.back().mx = 0.003f;
						movert.back().my = -0.003f;
						// 좌하
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x - s;
						movert.back().y = rt[i].y - s;
						movert.back().h = movert.back().w = (rt[i].w / 2);
						movert.back().mx = -0.003f;
						movert.back().my = -0.003f;
						break;
					}
						// 8갈래로 이동
					case 2: {
						s = ((rt[i].w / 3) * 2);
						// 상
						movert.push_back(rt[i]);
						movert.back().y = rt[i].y + s;
						movert.back().h = movert.back().w = rt[i].w / 3;
						movert.back().my = 0.003f;
						// 우
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x + s;
						movert.back().h = movert.back().w = rt[i].w / 3;
						movert.back().mx = 0.003f;
						// 하
						movert.push_back(rt[i]);
						movert.back().y = rt[i].y - s;
						movert.back().h = movert.back().w = rt[i].w / 3;
						movert.back().my = -0.003f;
						// 좌
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x - s;
						movert.back().h = movert.back().w = rt[i].w / 3;
						movert.back().mx = -0.003f;
						// 좌상
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x - s;
						movert.back().y = rt[i].y + s;
						movert.back().h = movert.back().w = rt[i].w / 3;
						movert.back().my = 0.003f;
						movert.back().mx = -0.003f;
						// 우상
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x + s;
						movert.back().y = rt[i].y + s;
						movert.back().h = movert.back().w = rt[i].w / 3;
						movert.back().mx = 0.003f;
						movert.back().my = 0.003f;
						// 우하
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x + s;
						movert.back().y = rt[i].y - s;
						movert.back().h = movert.back().w = rt[i].w / 3;
						movert.back().mx = 0.003f;
						movert.back().my = -0.003f;
						// 좌하
						movert.push_back(rt[i]);
						movert.back().x = rt[i].x - s;
						movert.back().y = rt[i].y - s;
						movert.back().h = movert.back().w = rt[i].w / 3;
						movert.back().mx = -0.003f;
						movert.back().my = -0.003f;
						// 가운데
						movert.push_back(rt[i]);
						movert.back().h = movert.back().w = rt[i].w / 3;
						break;
					}
					default:
						break;
					}

					rt.erase(rt.begin() + i);
					return;
				}
			}
			for (int i = 0; i < movert.size(); ++i) {
				if (movert[i].x - movert[i].w <= mx && movert[i].x + movert[i].w >= mx &&
					movert[i].y - movert[i].h <= my && movert[i].y + movert[i].h >= my) {
					rt.push_back(movert[i]);
					movert.erase(movert.begin() + i);
					--i;

					GLfloat s = (rt.back().w / 2);

					switch (rt.back().motion)
					{
						// 좌우상하 이동
					case 0: {
						// 상
						movert.push_back(rt.back());
						movert.back().x = rt.back().x - s;
						movert.back().y = rt.back().y + s;
						movert.back().h = movert.back().w = s;
						movert.back().my = 0.003f;
						// 우
						movert.push_back(rt.back());
						movert.back().x = rt.back().x + s;
						movert.back().y = rt.back().y + s;
						movert.back().h = movert.back().w = s;
						movert.back().mx = 0.003f;
						// 하
						movert.push_back(rt.back());
						movert.back().x = rt.back().x + s;
						movert.back().y = rt.back().y - s;
						movert.back().h = movert.back().w = s;
						movert.back().my = -0.003f;
						// 좌
						movert.push_back(rt.back());
						movert.back().x = rt.back().x - s;
						movert.back().y = rt.back().y - s;
						movert.back().h = movert.back().w = s;
						movert.back().mx = -0.003f;
						break;
					}
						  // 대각선 이동
					case 1: {
						// 좌상
						movert.push_back(rt.back());
						movert.back().x = rt.back().x - s;
						movert.back().y = rt.back().y + s;
						movert.back().h = movert.back().w = s;
						movert.back().my = 0.003f;
						movert.back().mx = -0.003f;
						// 우상
						movert.push_back(rt.back());
						movert.back().x = rt.back().x + s;
						movert.back().y = rt.back().y + s;
						movert.back().h = movert.back().w = s;
						movert.back().mx = 0.003f;
						movert.back().my = 0.003f;
						// 우하
						movert.push_back(rt.back());
						movert.back().x = rt.back().x + s;
						movert.back().y = rt.back().y - s;
						movert.back().h = movert.back().w = s;
						movert.back().mx = 0.003f;
						movert.back().my = -0.003f;
						// 좌하
						movert.push_back(rt.back());
						movert.back().x = rt.back().x - s;
						movert.back().y = rt.back().y - s;
						movert.back().h = movert.back().w = s;
						movert.back().mx = -0.003f;
						movert.back().my = -0.003f;
						break;
					}
						  // 8갈래로 이동
					case 2: {
						s = ((rt.back().w / 3) * 2);
						// 상
						movert.push_back(rt.back());
						movert.back().y = rt.back().y + s;
						movert.back().h = movert.back().w = rt.back().w / 3;
						movert.back().my = 0.003f;
						// 우
						movert.push_back(rt.back());
						movert.back().x = rt.back().x + s;
						movert.back().h = movert.back().w = rt.back().w / 3;
						movert.back().mx = 0.003f;
						// 하
						movert.push_back(rt.back());
						movert.back().y = rt.back().y - s;
						movert.back().h = movert.back().w = rt.back().w / 3;
						movert.back().my = -0.003f;
						// 좌
						movert.push_back(rt.back());
						movert.back().x = rt.back().x - s;
						movert.back().h = movert.back().w = rt.back().w / 3;
						movert.back().mx = -0.003f;
						// 좌상
						movert.push_back(rt.back());
						movert.back().x = rt.back().x - s;
						movert.back().y = rt.back().y + s;
						movert.back().h = movert.back().w = rt.back().w / 3;
						movert.back().my = 0.003f;
						movert.back().mx = -0.003f;
						// 우상
						movert.push_back(rt.back());
						movert.back().x = rt.back().x + s;
						movert.back().y = rt.back().y + s;
						movert.back().h = movert.back().w = rt.back().w / 3;
						movert.back().mx = 0.003f;
						movert.back().my = 0.003f;
						// 우하
						movert.push_back(rt.back());
						movert.back().x = rt.back().x + s;
						movert.back().y = rt.back().y - s;
						movert.back().h = movert.back().w = rt.back().w / 3;
						movert.back().mx = 0.003f;
						movert.back().my = -0.003f;
						// 좌하
						movert.push_back(rt.back());
						movert.back().x = rt.back().x - s;
						movert.back().y = rt.back().y - s;
						movert.back().h = movert.back().w = rt.back().w / 3;
						movert.back().mx = -0.003f;
						movert.back().my = -0.003f;
						// 가운데
						movert.push_back(rt.back());
						movert.back().h = movert.back().w = rt.back().w / 3;
						break;
					}
					default:
						break;
					}

					rt.pop_back();
					return;
				}
			}
		}
	}
}
