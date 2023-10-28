#include "stdafx.h"
#include "GLShapes.h"
#include "GLObj.h"
#include "GLLIne.h"

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
void Special_Keyboard(int key, int x, int y);

float winSizex = 800, winSizey = 800;
GLuint vao;
GLfloat Trun = 0.f;

class GLEarth:
	public GLObj {
public:
	GLObj* sun;
	glm::vec3 pyr{ 0.f, 0.f, 0.f };

	GLEarth() {};
	GLEarth(GLObj* a) : sun(a) { }

	void Update(int i) {
		if (i == 0) {
			World_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			World_mat = glm::translate(World_mat, sun->pos + pos);
		}
		else if (i == 1) {
			World_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(-1.0f, 1.0f, 0.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			World_mat = glm::translate(World_mat, sun->pos + pos);
		}
		else if (i == 2) {
			World_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 1.0f, 0.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			World_mat = glm::translate(World_mat, sun->pos + pos);
		}

		World_mat = glm::scale(World_mat, scale);

		World_mat = glm::translate(World_mat, -midpos);
	}
};
class GLMoon:
	public GLEarth {
public:
	GLEarth* earth;

	GLMoon() {};
	GLMoon(GLEarth* a) : earth(a) {}

	void Update(int i) {
		if (i == 0) {
			World_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, earth->sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(earth->sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(earth->sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -earth->sun->pos);

			glm::mat4 Earth_mat = glm::mat4(1.0);
			Earth_mat = glm::translate(Earth_mat, earth->sun->pos);
			Earth_mat = glm::rotate(Earth_mat, glm::radians(earth->revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			Earth_mat = glm::rotate(Earth_mat, glm::radians(earth->revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			Earth_mat = glm::rotate(Earth_mat, glm::radians(earth->revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
			Earth_mat = glm::translate(Earth_mat, -earth->sun->pos);

			Earth_mat = glm::translate(Earth_mat, earth->sun->pos + earth->pos);

			World_mat *= Earth_mat;

			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, pos);
			World_mat = glm::scale(World_mat, scale);
			World_mat = glm::translate(World_mat, -midpos);
		}
		else if (i == 1) {
			World_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, earth->sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(earth->sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(earth->sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -earth->sun->pos);

			glm::mat4 Earth_mat = glm::mat4(1.0);
			Earth_mat = glm::translate(Earth_mat, earth->sun->pos);
			Earth_mat = glm::rotate(Earth_mat, glm::radians(earth->revolve_theta.x), glm::vec3(-1.0f, 1.0f, 0.0f));
			Earth_mat = glm::translate(Earth_mat, -earth->sun->pos);

			Earth_mat = glm::translate(Earth_mat, earth->sun->pos + earth->pos);

			World_mat *= Earth_mat;

			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(-1.0f, 1.0f, 0.0f));
			World_mat = glm::translate(World_mat, pos);
			World_mat = glm::scale(World_mat, scale);
			World_mat = glm::translate(World_mat, -midpos);
		}
		else if (i == 2) {
			World_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, earth->sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(earth->sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(earth->sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -earth->sun->pos);

			glm::mat4 Earth_mat = glm::mat4(1.0);
			Earth_mat = glm::translate(Earth_mat, earth->sun->pos);
			Earth_mat = glm::rotate(Earth_mat, glm::radians(earth->revolve_theta.x), glm::vec3(1.0f, 1.0f, 0.0f));
			Earth_mat = glm::translate(Earth_mat, -earth->sun->pos);

			Earth_mat = glm::translate(Earth_mat, earth->sun->pos + earth->pos);

			World_mat *= Earth_mat;

			World_mat = glm::rotate(World_mat, glm::radians(revolve_theta.y), glm::vec3(1.0f, 1.0f, 0.0f));
			World_mat = glm::translate(World_mat, pos);
			World_mat = glm::scale(World_mat, scale);
			World_mat = glm::translate(World_mat, -midpos);
		}
	}
};
class one: 
	public GLShapes
{
public:
	GLuint v_pos[3];
	GLObj* sun;
	GLMoon* moon;

	one() {};
	one(GLObj* s) : sun(s) {
		glm::vec3 color{ 0.f, 0.f, 0.f };
		glGenBuffers(1, &v_color);
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), &color, GL_STATIC_DRAW);

		glGenBuffers(3, v_pos);

		glm::vec3 rob[360]{};
		GLfloat r = 0.25f;
		for (int i = 0; i < 360; ++i) {
			// x z 평면 r = 0.25f
			rob[i].x = r * glm::cos(glm::radians((float)i));
			rob[i].y = 0.f;
			rob[i].z = r * glm::sin(glm::radians((float)i));
		}
		glBindBuffer(GL_ARRAY_BUFFER, v_pos[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rob), rob, GL_STATIC_DRAW);

		for (int i = 0; i < 360; ++i) {
			// x z 평면 r = 0.25f
			rob[i].x = r * glm::cos(glm::radians((float)i));
			rob[i].y = 0.f;
			rob[i].z = r * glm::sin(glm::radians((float)i));
		}
		glBindBuffer(GL_ARRAY_BUFFER, v_pos[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rob), rob, GL_STATIC_DRAW);

		for (int i = 0; i < 360; ++i) {
			// x z 평면 r = 0.25f
			rob[i].x = r * glm::cos(glm::radians((float)i));
			rob[i].y = 0.f;
			rob[i].z = r * glm::sin(glm::radians((float)i));
		}
		glBindBuffer(GL_ARRAY_BUFFER, v_pos[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rob), rob, GL_STATIC_DRAW);
	}
	one(GLMoon* m) : sun(m->earth->sun), moon(m) {
		glm::vec3 color{ 0.f, 0.f, 0.f };
		glGenBuffers(1, &v_color);
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), &color, GL_STATIC_DRAW);

		glGenBuffers(3, v_pos);

		glm::vec3 rob[360]{};
		GLfloat r = 0.07f;
		for (int i = 0; i < 360; ++i) {
			rob[i].x = r * glm::cos(glm::radians((float)i));
			rob[i].y = 0.f;
			rob[i].z = r * glm::sin(glm::radians((float)i));
		}
		glBindBuffer(GL_ARRAY_BUFFER, v_pos[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rob), rob, GL_STATIC_DRAW);
	}
	virtual ~one() {};

	void draw(int i) {
		GLfloat a = 45.f;
		if (i == 0) {
			World_mat = glm::mat4(1.0);

			glm::mat4 Earth_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			Earth_mat = glm::translate(Earth_mat, moon->earth->sun->pos);
			Earth_mat = glm::rotate(Earth_mat, glm::radians(moon->earth->revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			Earth_mat = glm::rotate(Earth_mat, glm::radians(moon->earth->revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			Earth_mat = glm::rotate(Earth_mat, glm::radians(moon->earth->revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
			Earth_mat = glm::translate(Earth_mat, -moon->earth->sun->pos);

			Earth_mat = glm::translate(Earth_mat, moon->earth->sun->pos + moon->earth->pos);

			World_mat *= Earth_mat;

			World_mat = glm::translate(World_mat, pos);
		}
		else if (i == 1) {
			World_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			glm::mat4 Earth_mat = glm::mat4(1.0);
			Earth_mat = glm::translate(Earth_mat, moon->earth->sun->pos);
			Earth_mat = glm::rotate(Earth_mat, glm::radians(moon->earth->revolve_theta.x), glm::vec3(-1.0f, 1.0f, 0.0f));
			Earth_mat = glm::translate(Earth_mat, -moon->earth->sun->pos);

			Earth_mat = glm::translate(Earth_mat, moon->earth->sun->pos + moon->earth->pos);

			Earth_mat = glm::rotate(Earth_mat, glm::radians(a), glm::vec3(0.0f, 0.0f, 1.0f));

			World_mat *= Earth_mat;

			World_mat = glm::translate(World_mat, pos);
		}
		else {
			World_mat = glm::mat4(1.0);

			World_mat = glm::translate(World_mat, sun->pos);
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
			World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat = glm::translate(World_mat, -sun->pos);

			glm::mat4 Earth_mat = glm::mat4(1.0);
			Earth_mat = glm::translate(Earth_mat, moon->earth->sun->pos);
			Earth_mat = glm::rotate(Earth_mat, glm::radians(moon->earth->revolve_theta.x), glm::vec3(1.0f, 1.0f, 0.0f));
			Earth_mat = glm::translate(Earth_mat, -moon->earth->sun->pos);

			Earth_mat = glm::translate(Earth_mat, moon->earth->sun->pos + moon->earth->pos);

			Earth_mat = glm::rotate(Earth_mat, glm::radians(-a), glm::vec3(0.0f, 0.0f, 1.0f));
			World_mat *= Earth_mat;

			World_mat = glm::translate(World_mat, pos);
		}

		glBindBuffer(GL_ARRAY_BUFFER, v_pos[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, v_color);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(World_mat));
		glDrawArrays(GL_LINE_LOOP, 0, 360);
	}
	void draw() {
		for (int i = 0; i < 3; ++i) {
			if (i == 0) {
				World_mat = glm::mat4(1.0);

				World_mat = glm::translate(World_mat, sun->pos);
				World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
				World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
				World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
				World_mat = glm::translate(World_mat, -sun->pos);

				World_mat = glm::translate(World_mat, sun->pos);
			}
			else if (i == 1) {
				World_mat = glm::mat4(1.0);
				GLfloat a = 45.f;

				World_mat = glm::translate(World_mat, sun->pos);
				World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
				World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
				World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
				World_mat = glm::translate(World_mat, -sun->pos);

				World_mat = glm::translate(World_mat, sun->pos);
				World_mat = glm::rotate(World_mat, glm::radians(a), glm::vec3(0.0f, 0.0f, 1.0f));
				World_mat = glm::translate(World_mat, -sun->pos);

				World_mat = glm::translate(World_mat, sun->pos);
			}
			else {
				World_mat = glm::mat4(1.0);
				GLfloat a = 45.f;

				World_mat = glm::translate(World_mat, sun->pos);
				World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
				World_mat = glm::rotate(World_mat, glm::radians(sun->rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
				World_mat = glm::rotate(World_mat, glm::radians(Trun), glm::vec3(0.0f, 0.0f, 1.0f));
				World_mat = glm::translate(World_mat, -sun->pos);

				World_mat = glm::translate(World_mat, sun->pos);
				World_mat = glm::rotate(World_mat, glm::radians(-a), glm::vec3(0.0f, 0.0f, 1.0f));
				World_mat = glm::translate(World_mat, -sun->pos);

				World_mat = glm::translate(World_mat, sun->pos);
			}

			glBindBuffer(GL_ARRAY_BUFFER, v_pos[i]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, v_color);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(World_mat));
			glDrawArrays(GL_LINE_LOOP, 0, 360);
		}
	}
};

GLObj Sun;
std::vector <GLEarth> Earth;
std::vector <GLMoon> Moon;
GLLine lineObj;
one robbi, yubin[3];
glm::vec3 ViewZoom{ 0.f, 0.f, 0.f };

bool Back_Cull = true, Draw_Mod = true;
int Mod = 1, Projection_Mod = 1;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#19 하트 태양계");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Init();
	glutKeyboardFunc(Keyboard); 
	glutSpecialFunc(Special_Keyboard); 
	glutTimerFunc(10, TimerFunction, 1);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene()
{
	glUseProgram(shaderProgramID);

	// 카메라 변환
	glm::mat4 Camera_Mat = glm::lookAt(glm::vec3{ 0.f, 0.2f, 1.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f });

	// 투영 변환
	glm::mat4 Projection_Mat = glm::mat4(1.0f);
	{
		if (Projection_Mod == 0) {
			Projection_Mat = glm::ortho( -2.f, 2.f, -2.f, 2.f, 0.1f, 30.f);
		}
		else if (Projection_Mod == 1) {
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

	std::string a;
	if (Draw_Mod) a = "solid";
	else a = "line";
	//// X축 Y축 그리기
	//lineObj.Update();
	//lineObj.draw_prepare(PosLocation, "Pos");
	//lineObj.draw_prepare(ColorLocation, "Color");
	//lineObj.draw_prepare(WorldTransLocation, "World");
	//lineObj.draw();

	Sun.Update();
	Sun.draw_prepare(PosLocation, "Pos");
	Sun.draw_prepare(ColorLocation, "Color");
	Sun.draw_prepare(WorldTransLocation, "World");
	Sun.draw(a);
	
	for (int i = 0; i < Earth.size(); ++i) {
		Earth[i].Update(i);
		Earth[i].draw_prepare(PosLocation, "Pos");
		Earth[i].draw_prepare(ColorLocation, "Color");
		Earth[i].draw_prepare(WorldTransLocation, "World");
		Earth[i].draw(a);
	}
	
	for (int i = 0; i < Moon.size(); ++i) {
		Moon[i].Update(i);
		Moon[i].draw_prepare(PosLocation, "Pos");
		Moon[i].draw_prepare(ColorLocation, "Color");
		Moon[i].draw_prepare(WorldTransLocation, "World");
		Moon[i].draw(a);
	}

	robbi.draw();
	for (int i = 0; i < 3; ++i)
		yubin[i].draw(i);

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
		for (int i = 0; i < Earth.size(); ++i) {
			Earth[i].revolve_theta += Earth[i].pyr;
		}
		for (int i = 0; i < Moon.size(); ++i) {
			Moon[i].revolve_theta += Moon[i].pyr;
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
	case 'm':
	case 'M': {
		if (!Draw_Mod) Draw_Mod = true;
		else Draw_Mod = false;
		break;
	}
	case 'p':
	case 'P': {
		if (Projection_Mod == 0) Projection_Mod = 1;
		else Projection_Mod = 0;
		break;
	}
	case '-':
	case '_': {
		Sun.pos += glm::vec3{ 0.f, 0.f, -0.02f };
		break;
	}
	case '=':
	case '+': {
		Sun.pos += glm::vec3{ 0.f, 0.f, 0.02f };
		break;
	}
	case 'a':
	case 'A': {
		Sun.rotate_theta += glm::vec3{ 0.f, -2.f, 0.f };
		break;
	}
	case 'd':
	case 'D': {
		Sun.rotate_theta += glm::vec3{ 0.f, 2.f, 0.f };
		break;
	}
	case 'w':
	case 'W': {
		Trun += 2.f;
		break;
	}
	case 's':
	case 'S': {
		Trun += -2.f;
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
void Special_Keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: {
		Sun.pos += glm::vec3{ -0.02f, 0.f, 0.f };
		break;
	}
	case GLUT_KEY_RIGHT: {
		Sun.pos += glm::vec3{ 0.02f, 0.f, 0.f };
		break;
	}
	case GLUT_KEY_UP: {
		Sun.pos += glm::vec3{ 0.f, 0.02f, 0.f };
		break;
	}
	case GLUT_KEY_DOWN: {
		Sun.pos += glm::vec3{ 0.f, -0.02f, 0.f };
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
	{
		// 태양 초기 설정
		{
			std::ifstream inputFile("./OBJ/heart.obj");

			if (inputFile.is_open())
				Sun.objLoad(inputFile);
			else
				std::cerr << "Failed to heart obj file" << std::endl;

			Sun.pos = glm::vec3{ 0.f, 0.0f, 0.0f };
			Sun.scale = glm::vec3{ 0.02f, 0.02f, 0.02f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Sun.face_cnt * 3; ++i) {
				glm::vec3 a{ 1.f, colorRd(gen), colorRd(gen) };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Sun.v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Sun.v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 가운데 지구 초기 설정
		{
			std::ifstream inputFile("./OBJ/heart.obj");
			Earth.emplace_back(GLEarth(&Sun));

			if (inputFile.is_open())
				Earth.back().objLoad(inputFile);
			else
				std::cerr << "Failed to heart obj file" << std::endl;

			Earth.back().pos = glm::vec3{ 0.f, 0.0f, 0.25f };
			Earth.back().scale = glm::vec3{ 0.005f, 0.005f, 0.005f };
			Earth.back().pyr = glm::vec3{ 0.f, 0.8f, 0.f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Sun.face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Earth.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Earth.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 가운데 지구의 달 초기 설정
		{
			std::ifstream inputFile("./OBJ/heart.obj");
			Moon.emplace_back(GLMoon(&Earth.back()));

			if (inputFile.is_open())
				Moon.back().objLoad(inputFile);
			else
				std::cerr << "Failed to heart obj file" << std::endl;

			Moon.back().pos = glm::vec3{ 0.f, 0.0f, 0.07f };
			Moon.back().scale = glm::vec3{ 0.003f, 0.003f, 0.003f };
			Moon.back().pyr = glm::vec3{ 0.f, 1.6f, 0.f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Sun.face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen) + 0.5f, colorRd(gen) + 0.5f, colorRd(gen) + 0.5f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Moon.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Moon.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 오른쪽으로 기운 지구 초기 설정
		{
			std::ifstream inputFile("./OBJ/heart.obj");
			Earth.emplace_back(GLEarth(&Sun));

			if (inputFile.is_open())
				Earth.back().objLoad(inputFile);
			else
				std::cerr << "Failed to heart obj file" << std::endl;

			Earth.back().pos = glm::vec3{ 0.f, 0.0f, 0.25f };
			Earth.back().scale = glm::vec3{ 0.005f, 0.005f, 0.005f };
			Earth.back().pyr = glm::vec3{ -1.2f, 0.f, 0.f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Sun.face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Earth.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Earth.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 가운데 지구의 달 초기 설정
		{
			std::ifstream inputFile("./OBJ/heart.obj");
			Moon.emplace_back(GLMoon(&Earth.back()));

			if (inputFile.is_open())
				Moon.back().objLoad(inputFile);
			else
				std::cerr << "Failed to heart obj file" << std::endl;

			Moon.back().pos = glm::vec3{ 0.f, 0.0f, 0.07f };
			Moon.back().scale = glm::vec3{ 0.003f, 0.003f, 0.003f };
			Moon.back().pyr = glm::vec3{ 0.f, 2.f, 0.f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Sun.face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen) + 0.5f, colorRd(gen) + 0.5f, colorRd(gen) + 0.5f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Moon.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Moon.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 왼쪽으로 기운 지구 초기 설정
		{
			std::ifstream inputFile("./OBJ/heart.obj");
			Earth.emplace_back(GLEarth(&Sun));

			if (inputFile.is_open())
				Earth.back().objLoad(inputFile);
			else
				std::cerr << "Failed to heart obj file" << std::endl;

			Earth.back().pos = glm::vec3{ 0.f, 0.0f, 0.25f };
			Earth.back().scale = glm::vec3{ 0.005f, 0.005f, 0.005f };
			Earth.back().pyr = glm::vec3{ 2.1f, 0.f, 0.f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Sun.face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen), colorRd(gen), 1.f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Earth.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Earth.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}
		// 가운데 지구의 달 초기 설정
		{
			std::ifstream inputFile("./OBJ/heart.obj");
			Moon.emplace_back(GLMoon(&Earth.back()));

			if (inputFile.is_open())
				Moon.back().objLoad(inputFile);
			else
				std::cerr << "Failed to heart obj file" << std::endl;

			Moon.back().pos = glm::vec3{ 0.f, 0.0f, 0.07f };
			Moon.back().scale = glm::vec3{ 0.003f, 0.003f, 0.003f };
			Moon.back().pyr = glm::vec3{ 0.f, 1.6f, 0.f };

			std::vector<glm::vec3> color;
			for (int i = 0; i < Sun.face_cnt * 3; ++i) {
				glm::vec3 a{ colorRd(gen) + 0.5f, colorRd(gen) + 0.5f, colorRd(gen) + 0.5f };
				color.emplace_back(a);
			}

			glGenBuffers(1, &Moon.back().v_color);
			glBindBuffer(GL_ARRAY_BUFFER, Moon.back().v_color);
			glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		}

		robbi = one(&Sun);
	}

	for (int i = 0; i < Earth.size(); ++i) {
		Earth[i].sun = &Sun;
	}
	for (int i = 0; i < Moon.size(); ++i) {
		Moon[i].earth = &Earth[i];
		yubin[i] = one(&Moon[i]);
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