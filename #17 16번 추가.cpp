#include "stdafx.h"
#include "GLShapes.h"
#include "GLCube.h"
#include "GLObj.h"
// #include "GLTetrahedron.h"
// #include "GLPyramid.h"
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
void Special_Keyboard(int key, int x, int y);//void Mouse(int button, int state, int x, int y);
void MouseWheel(int wheel, int dir, int x, int y);

float winSizex = 800, winSizey = 800;
GLuint vao, sp_v_pos;

GLObj heartObj;
GLObj ChickenDrumstickObj;
GLLine lineObj;
GLfloat heartTheta = 180.0f, stickTheta = 0.0f, Half = 0.5f;
glm::vec3 heartplusHalf{ 0.002f, 0.0f, 0.0f }, stickplusHalf{ 0.002f, 0.0f, 0.0f }, plusHalf{ 0.002f, 0.0f, 0.0f }, stickStart{ 0.0f, 0.0f, 0.0f }, heartStart{ 0.0f, 0.0f, 0.0f }, Obj2MidPos;

// 기본 육면체 출력, 1은 사면체 출력 / 0은 닭다리 1은 하트 / 0은 자신에 대해 신축 1은 원점에 대해 신축 / Mod = 5에서 쓰이는 방향 구별 변수
int Mod = 0, Select = 0, ScaleMod = 0, Xmod = 0, cnt = 0;
int Rotation_Mod = 0; // 1 왼쪽으로 2 오른쪽으로 3 위로 4 아래로
glm::vec3 Theta_plus{}, Revolve_Theta_plus{};

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(winSizex, winSizey);
	glutCreateWindow("#17");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	Init();
	//Clear();
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutSpecialFunc(Special_Keyboard); // 키보드 입력 콜백함수
	glutTimerFunc(10, TimerFunction, 1);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene()
{
	glUseProgram(shaderProgramID);

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

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// X축 Y축 그리기
	lineObj.Update();
	lineObj.draw_prepare(PosLocation, "Pos");
	lineObj.draw_prepare(ColorLocation, "Color");
	lineObj.draw_prepare(WorldTransLocation, "World");
	glDrawArrays(GL_LINES, 0, 6);

	// std::cout << (int)glIsEnabled(GL_CULL_FACE) << std::endl;

	if (Mod != 4) {
		ChickenDrumstickObj.Update();
	}
	else {
		ChickenDrumstickObj.World_mat = glm::mat4(1.0);

		ChickenDrumstickObj.World_mat = glm::scale(ChickenDrumstickObj.World_mat, ChickenDrumstickObj.Oscale);

		ChickenDrumstickObj.World_mat = glm::translate(ChickenDrumstickObj.World_mat, Obj2MidPos);
		ChickenDrumstickObj.World_mat = glm::rotate(ChickenDrumstickObj.World_mat, glm::radians(ChickenDrumstickObj.revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ChickenDrumstickObj.World_mat = glm::rotate(ChickenDrumstickObj.World_mat, glm::radians(ChickenDrumstickObj.revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ChickenDrumstickObj.World_mat = glm::rotate(ChickenDrumstickObj.World_mat, glm::radians(ChickenDrumstickObj.revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ChickenDrumstickObj.World_mat = glm::translate(ChickenDrumstickObj.World_mat, -Obj2MidPos);

		ChickenDrumstickObj.World_mat = glm::translate(ChickenDrumstickObj.World_mat, ChickenDrumstickObj.pos);

		ChickenDrumstickObj.World_mat = glm::rotate(ChickenDrumstickObj.World_mat, glm::radians(ChickenDrumstickObj.rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ChickenDrumstickObj.World_mat = glm::rotate(ChickenDrumstickObj.World_mat, glm::radians(ChickenDrumstickObj.rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ChickenDrumstickObj.World_mat = glm::rotate(ChickenDrumstickObj.World_mat, glm::radians(ChickenDrumstickObj.rotate_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

		ChickenDrumstickObj.World_mat = glm::scale(ChickenDrumstickObj.World_mat, ChickenDrumstickObj.scale);
		ChickenDrumstickObj.World_mat = glm::translate(ChickenDrumstickObj.World_mat, ChickenDrumstickObj.midpos);
	}
	ChickenDrumstickObj.draw_prepare(PosLocation, "Pos");
	ChickenDrumstickObj.draw_prepare(ColorLocation, "Color");
	ChickenDrumstickObj.draw_prepare(WorldTransLocation, "World");
	ChickenDrumstickObj.draw("line");

	if (Mod != 4) {
		heartObj.Update();
	}
	else {
		heartObj.World_mat = glm::mat4(1.0);

		heartObj.World_mat = glm::scale(heartObj.World_mat, heartObj.Oscale);

		heartObj.World_mat = glm::translate(heartObj.World_mat, Obj2MidPos);
		heartObj.World_mat = glm::rotate(heartObj.World_mat, glm::radians(heartObj.revolve_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
		heartObj.World_mat = glm::rotate(heartObj.World_mat, glm::radians(heartObj.revolve_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
		heartObj.World_mat = glm::rotate(heartObj.World_mat, glm::radians(heartObj.revolve_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));
		heartObj.World_mat = glm::translate(heartObj.World_mat, -Obj2MidPos);

		heartObj.World_mat = glm::translate(heartObj.World_mat, heartObj.pos);

		heartObj.World_mat = glm::rotate(heartObj.World_mat, glm::radians(heartObj.rotate_theta.x), glm::vec3(1.0f, 0.0f, 0.0f));
		heartObj.World_mat = glm::rotate(heartObj.World_mat, glm::radians(heartObj.rotate_theta.y), glm::vec3(0.0f, 1.0f, 0.0f));
		heartObj.World_mat = glm::rotate(heartObj.World_mat, glm::radians(heartObj.rotate_theta.z), glm::vec3(0.0f, 0.0f, 1.0f));

		heartObj.World_mat = glm::scale(heartObj.World_mat, heartObj.scale);
		heartObj.World_mat = glm::translate(heartObj.World_mat, heartObj.midpos);
	}
	heartObj.draw_prepare(PosLocation, "Pos");
	heartObj.draw_prepare(ColorLocation, "Color");
	heartObj.draw_prepare(WorldTransLocation, "World");
	heartObj.draw("solid");

	if (Mod == 1) {
		glm::mat4 mat { 1.f };
		glBindBuffer(GL_ARRAY_BUFFER, sp_v_pos);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
		mat = glm::rotate(mat, glm::radians(30.f), glm::vec3(1.0f, 0.0f, 0.0f));
		mat = glm::rotate(mat, glm::radians(-30.f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(WorldTransLocation, 1, GL_FALSE, glm::value_ptr(mat));

		glPointSize(2.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2002);
	}

	//glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

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
		if (Mod == 1) { // 스파이럴
			float stickradian = stickTheta * 0.0174533;
			float heartradian = heartTheta * 0.0174533;
			glm::vec3 middlePoint = { 0.0f, 0.0f, 0.0f };

			ChickenDrumstickObj.pos = { middlePoint.x + Half * cos(stickradian),
														ChickenDrumstickObj.pos.y, 
														middlePoint.z + Half * sin(stickradian) };
			heartObj.pos = { middlePoint.x + Half * cos(heartradian),
														heartObj.pos.y,
														middlePoint.z + Half * sin(heartradian) };

			stickTheta += 1.0f;
			heartTheta += 1.0f;
			Half -= 0.0005;
			
			//if (stickTheta >= 360) {
			//	heartTheta = 0;
			//	stickTheta = 0;
			//}

			if (Half <= 0.0f) {
				Half = 0.5f;
				stickTheta = 0;
				heartTheta = 180.f;
			}
		}
		else if (Mod == 2) {
			ChickenDrumstickObj.pos -= stickplusHalf;
			heartObj.pos -= heartplusHalf;
			// std::cout << heartObj.pos.x << std::endl;

			if (((stickStart.x < 0 && (ChickenDrumstickObj.pos.x <= stickStart.x || ChickenDrumstickObj.pos.x >= 0.0f)) || 
				(stickStart.x > 0 && (ChickenDrumstickObj.pos.x >= stickStart.x || ChickenDrumstickObj.pos.x <= 0.0f))) ||
				((heartStart.x < 0 && (heartObj.pos.x <= heartStart.x || heartObj.pos.x >= 0.0f)) || 
					(heartStart.x > 0 && (heartObj.pos.x >= heartStart.x || heartObj.pos.x <= 0.0f)))) {
				stickplusHalf *= -1;
				heartplusHalf *= -1;
			}
		}
		else if (Mod == 3) {
			ChickenDrumstickObj.pos += plusHalf;
			heartObj.pos -= plusHalf;
			cnt++;

			if (cnt == 100) {
				plusHalf *= -1;
				cnt = 0;
			}
		}
		else if (Mod == 4) {
			cnt++;
			if (cnt == 90) {
				cnt = 0;
				Mod = 0;
				Revolve_Theta_plus = glm::vec3{ 0.0f, 0.0f, 0.0f };
			}
			// std::cout << heartObj.pos.x << " " << heartObj.pos.y << " " << heartObj.pos.z << std::endl;
		}
		else if (Mod == 5) {
			if (Xmod == 0) {
				ChickenDrumstickObj.pos -= plusHalf;
				heartObj.pos += plusHalf;
				ChickenDrumstickObj.pos.x -= 0.002f;
				heartObj.pos.x += 0.002f;
			}
			else if (Xmod == 1) {
				ChickenDrumstickObj.pos -= plusHalf;
				heartObj.pos += plusHalf;
				ChickenDrumstickObj.pos.x += 0.002f;
				heartObj.pos.x -= 0.002f;
			}

			// std::cout << ChickenDrumstickObj.pos.y << std::endl;
			if (ChickenDrumstickObj.pos.y <= -0.5f || heartObj.pos.y >= 0.5f || ChickenDrumstickObj.pos.y >= 0.5f || heartObj.pos.y <= -0.5f) {
				plusHalf *= -1;
			}
			else if (ChickenDrumstickObj.pos.x <= -0.5f || heartObj.pos.x >= 0.5f || ChickenDrumstickObj.pos.x >= 0.5f || heartObj.pos.x <= -0.5f) {
				if (Xmod == 0) Xmod = 1;
				else Xmod = 0;
			}
		}

		heartObj.rotate_theta += Theta_plus;
		ChickenDrumstickObj.rotate_theta += Theta_plus;
		heartObj.revolve_theta += Revolve_Theta_plus;
		ChickenDrumstickObj.revolve_theta += Revolve_Theta_plus;
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
	case 'j':
	case 'J': {
		Theta_plus = glm::vec3{ 0.0f, -2.0f, 0.0f };
		if (Rotation_Mod == 1) {
			Rotation_Mod = 0;
			Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else Rotation_Mod = 1;
		break;
	}
	case 'l':
	case 'L': {
		Theta_plus = glm::vec3{ 0.0f, 2.0f, 0.0f };
		if (Rotation_Mod == 2) {
			Rotation_Mod = 0;
			Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else  Rotation_Mod = 2;
		break;
	}
	case 'i':
	case 'I':  {
		Theta_plus = glm::vec3{ 2.0f, 0.0f, 0.0f };
		if (Rotation_Mod == 3) {
			Rotation_Mod = 0;
			Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else Rotation_Mod = 3;
		break;
	}
	case 'k':
	case 'K': {
		Theta_plus = glm::vec3{ -2.0f, 0.0f, 0.0f };
		if (Rotation_Mod == 4) {
			Rotation_Mod = 0;
			Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else Rotation_Mod = 4;
		break;
	}
	case 'a':
	case 'A': {
		if (Revolve_Theta_plus.y == 2.0f) {
			Revolve_Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else
			Revolve_Theta_plus = glm::vec3{ 0.0f, 2.0f, 0.0f };
		break;
	}
	case 'd':
	case 'D': {
		if (Revolve_Theta_plus.y == -2.0f) {
			Revolve_Theta_plus = { 0.0f, 0.0f, 0.0f };
		}
		else
			Revolve_Theta_plus = glm::vec3{ 0.0f, -2.0f, 0.0f };
		break;
	}
	case 'r':
	case 'R': {
		if (Mod != 1) Mod = 1;
		else Mod = 0;
		break;
	}
	case 't':
	case 'T': {
		for (int i = 0; i < 3; ++i) {
			heartplusHalf[i] = heartObj.pos[i] / 100;
			stickplusHalf[i] = ChickenDrumstickObj.pos[i] / 100;
			stickStart[i] = ChickenDrumstickObj.pos[i];
			heartStart[i] = heartObj.pos[i];
		}
		if (Mod != 2) Mod = 2;
		else Mod = 0;
		break;
	}
	case '1': {
		for (int i = 0; i < 3; ++i) {
			plusHalf[i] = (heartObj.pos[i] - ChickenDrumstickObj.pos[i]) / 100;
		}
		if (Mod != 3) Mod = 3;
		else Mod = 0;
		cnt = 0;
		break;
	}
	case '2': {
		if (Mod != 4)  {
			ChickenDrumstickObj.revolve_theta.x = 30.0f;
			ChickenDrumstickObj.revolve_theta.y = -30.0f;
			ChickenDrumstickObj.pos = glm::vec3{ 0.5f, 0.0f, 0.0f };
			ChickenDrumstickObj.scale = glm::vec3{ 0.005f, 0.005f, 0.005f };
			ChickenDrumstickObj.Oscale = glm::vec3{ 1.0f, 1.0f, 1.0f };

			heartObj.revolve_theta.x = 30.0f;
			heartObj.revolve_theta.y = -30.0f;
			heartObj.pos = glm::vec3{ -0.5f, 0.0f, 0.0f };
			heartObj.scale = glm::vec3{ 0.03f, 0.03f, 0.03f };
			heartObj.Oscale = glm::vec3{ 1.0f, 1.0f, 1.0f };

			Revolve_Theta_plus = glm::vec3{ 0.0f, 0.0f, 0.0f };
			Theta_plus = glm::vec3{ 0.0f, 0.0f, 0.0f };

			Mod = 4;
			Revolve_Theta_plus = glm::vec3{ 0.0f, 2.0f, 0.0f };
		}
		else  {
			Mod = 0;
			Revolve_Theta_plus = glm::vec3{ 0.0f, 0.0f, 0.0f };
		}
		cnt = 0;
		break;
	}
	case '3': {
		plusHalf = glm::vec3{ 0.0f, 0.002f, 0.0f };
		if (Mod != 5) Mod = 5;
		else Mod = 0;
		Xmod = 0;
		break;
	}
	case 's':
	case 'S': {
		ChickenDrumstickObj.revolve_theta.x = 30.0f;
		ChickenDrumstickObj.revolve_theta.y = -30.0f;
		ChickenDrumstickObj.pos = glm::vec3{ 0.5f, 0.0f, 0.0f };
		ChickenDrumstickObj.scale = glm::vec3{ 0.005f, 0.005f, 0.005f };
		ChickenDrumstickObj.Oscale = glm::vec3{ 1.0f, 1.0f, 1.0f };

		heartObj.revolve_theta.x = 30.0f;
		heartObj.revolve_theta.y = -30.0f;
		heartObj.pos = glm::vec3{ -0.5f, 0.0f, 0.0f };
		heartObj.scale = glm::vec3{ 0.03f, 0.03f, 0.03f };
		heartObj.Oscale = glm::vec3{ 1.0f, 1.0f, 1.0f };

		Revolve_Theta_plus = glm::vec3{ 0.0f, 0.0f, 0.0f };
		Theta_plus = glm::vec3{ 0.0f, 0.0f, 0.0f };
		break;
	}
	case '_': 
	case '-': {
		if (Select == 0)
			ChickenDrumstickObj.pos += glm::vec3{ 0.0f, 0.0f, -0.005f };
		else if (Select == 1)
			heartObj.pos += glm::vec3{ 0.0f, 0.0f, -0.005f };
		else {
			ChickenDrumstickObj.pos += glm::vec3{ 0.0f, 0.0f, -0.005f };
			heartObj.pos += glm::vec3{ 0.0f, 0.0f, -0.005f };
		}
		break;
	}
	case '+':
	case '=': {
		if (Select == 0)
			ChickenDrumstickObj.pos += glm::vec3{ 0.0f, 0.0f, 0.005f };
		else if (Select == 1)
			heartObj.pos += glm::vec3{ 0.0f, 0.0f, 0.005f };
		else {
			ChickenDrumstickObj.pos += glm::vec3{ 0.0f, 0.0f, 0.005f };
			heartObj.pos += glm::vec3{ 0.0f, 0.0f, 0.005f };
		}
		break;
	}
	default:
		break;
	}

	Half = 0.5f;
	glutPostRedisplay(); // 화면 재 출력
}

void Special_Keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1: {
		Select = 0;
		break;
	}
	case GLUT_KEY_F2: {
		Select = 1;
		break;
	}
	case GLUT_KEY_F3: {
		Select = 2;
		break;
	}
	case GLUT_KEY_END: {
		if (ScaleMod == 0)
			ScaleMod = 1;
		else ScaleMod = 0;
		std::cout << ScaleMod << std::endl;
		break;
	}
	case GLUT_KEY_LEFT: {
		if (Select == 0)
			ChickenDrumstickObj.pos += glm::vec3{ -0.005f, 0.0f, 0.0f };
		else if (Select == 1)
			heartObj.pos += glm::vec3{ -0.005f, 0.0f, 0.0f };
		else {
			heartObj.pos += glm::vec3{ -0.005f, 0.0f, 0.0f };
			ChickenDrumstickObj.pos += glm::vec3{ -0.005f, 0.0f, 0.0f };
		}

		break;
	}
	case GLUT_KEY_RIGHT: {
		if (Select == 0)
			ChickenDrumstickObj.pos += glm::vec3{ 0.005f, 0.0f, 0.0f };
		else if (Select == 1)
			heartObj.pos += glm::vec3{ 0.005f, 0.0f, 0.0f };
		else {
			ChickenDrumstickObj.pos += glm::vec3{ 0.005f, 0.0f, 0.0f };
			heartObj.pos += glm::vec3{ 0.005f, 0.0f, 0.0f };
		}
		break;
	}
	case GLUT_KEY_UP: {
		if (Select == 0)
			ChickenDrumstickObj.pos += glm::vec3{ 0.0f, 0.005f, 0.0f };
		else if (Select == 1)
			heartObj.pos += glm::vec3{ 0.0f, 0.005f, 0.0f };
		else {
			ChickenDrumstickObj.pos += glm::vec3{ 0.0f, 0.005f, 0.0f };
			heartObj.pos += glm::vec3{ 0.0f, 0.005f, 0.0f };
		}
		break;
	}
	case GLUT_KEY_DOWN: {
		if (Select == 0)
			ChickenDrumstickObj.pos += glm::vec3{ 0.0f, -0.005f, 0.0f };
		else if (Select == 1)
			heartObj.pos += glm::vec3{ 0.0f, -0.005f, 0.0f };
		else {
			ChickenDrumstickObj.pos += glm::vec3{ 0.0f, -0.005f, 0.0f };
			heartObj.pos += glm::vec3{ 0.0f, -0.005f, 0.0f };
		}
		break;
	}
	default:
		break;
	}

	glutPostRedisplay(); // 화면 재 출력
}

void MouseWheel(int wheel, int dir, int x, int y)
{
	if (dir > 0) {
		// up
		if (Select == 0) {
			if (ScaleMod == 0) {
				ChickenDrumstickObj.scale += glm::vec3{ 0.002f, 0.002f, 0.002f };
				if (ChickenDrumstickObj.scale.x >= 0.02f)
					ChickenDrumstickObj.scale -= glm::vec3{ 0.002f, 0.002f, 0.002f };
			}
			else {
				ChickenDrumstickObj.Oscale += glm::vec3{ 0.02f, 0.02f, 0.02f };
				if (ChickenDrumstickObj.Oscale.x >= 1.5f)
					ChickenDrumstickObj.Oscale -= glm::vec3{ 0.02f, 0.02f, 0.02f };
			}
		}
		else if (Select == 1) {
			if (ScaleMod == 0) {
				heartObj.scale += glm::vec3{ 0.002f, 0.002f, 0.002f };
				if (heartObj.scale.x >= 0.1f)
					heartObj.scale -= glm::vec3{ 0.002f, 0.002f, 0.002f };
			}
			else {
				heartObj.Oscale += glm::vec3{ 0.02f, 0.02f, 0.02f };
				if (heartObj.Oscale.x >= 1.5f)
					heartObj.Oscale -= glm::vec3{ 0.02f, 0.02f, 0.02f };
			}
		}
	}
	else if (dir < 0) {
		// down
		if (Select == 0) {
			if (ScaleMod == 0) {
				ChickenDrumstickObj.scale -= glm::vec3{ 0.002f, 0.002f, 0.002f };
				if (ChickenDrumstickObj.scale.x <= 0.0005f)
					ChickenDrumstickObj.scale += glm::vec3{ 0.002f, 0.002f, 0.002f };
			}
			else {
				ChickenDrumstickObj.Oscale -= glm::vec3{ 0.02f, 0.02f, 0.02f };
				if (ChickenDrumstickObj.Oscale.x <= 0.0f)
					ChickenDrumstickObj.Oscale += glm::vec3{ 0.02f, 0.02f, 0.02f };
			}
		}
		else if (Select == 1) {
			if (ScaleMod == 0) {
				heartObj.scale -= glm::vec3{ 0.002f, 0.002f, 0.002f };
				if (heartObj.scale.x <= 0.003f)
					heartObj.scale += glm::vec3{ 0.002f, 0.002f, 0.002f };
			}
			else {
				heartObj.Oscale -= glm::vec3{ 0.02f, 0.02f, 0.02f };
				if (heartObj.Oscale.x <= 0.0f)
					heartObj.Oscale += glm::vec3{ 0.02f, 0.02f, 0.02f };
			}
		}
	}
}

void Init()
{
	glBindVertexArray(vao);

	// 면 초기 설정
	{
		std::ifstream inputFile("./OBJ/heart.obj");

		if (inputFile.is_open())
			heartObj.objLoad(inputFile);
		else
			std::cerr << "Failed to heart obj file" << std::endl;
		
		heartObj.revolve_theta.x = 30.0f;
		heartObj.revolve_theta.y = -30.0f;
		heartObj.pos = glm::vec3{ -0.5f, 0.0f, 0.0f };
		heartObj.scale = glm::vec3{ 0.03f, 0.03f, 0.03f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < heartObj.face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), colorRd(gen) };
			color.emplace_back(a);
		}

		glGenBuffers(1, &heartObj.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, heartObj.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
	}
	// 면 초기 설정
	{
		std::ifstream inputFile("./OBJ/ChickenDrumstick.obj");

		if (inputFile.is_open())
			ChickenDrumstickObj.objLoad(inputFile);
		else
			std::cerr << "Failed to heart obj file" << std::endl;
		
		ChickenDrumstickObj.revolve_theta.x = 30.0f;
		ChickenDrumstickObj.revolve_theta.y = -30.0f;
		ChickenDrumstickObj.pos = glm::vec3{ 0.5f, 0.0f, 0.0f };
		ChickenDrumstickObj.scale = glm::vec3{ 0.003f, 0.003f, 0.003f };

		std::vector<glm::vec3> color;
		for (int i = 0; i < ChickenDrumstickObj.face_cnt * 3; ++i) {
			glm::vec3 a{ colorRd(gen), colorRd(gen), colorRd(gen) };
			color.emplace_back(a);
		}

		glGenBuffers(1, &ChickenDrumstickObj.v_color);
		glBindBuffer(GL_ARRAY_BUFFER, ChickenDrumstickObj.v_color);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
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

		lineObj.rotate_theta.x = 30.0f;
		lineObj.rotate_theta.y = -30.0f;
	}
	
	{
		std::vector <glm::vec3> sp;
		GLfloat theta = 0.f;
		for (GLfloat half = 0.5f; half >= 0; half -= 0.0005) {
			sp.emplace_back(glm::vec3{ half * cos(theta * 0.0174533),
				0.f,
				half * sin(theta * 0.0174533) });
			theta += 1.f;
		}
		std::cout << theta;
		theta = 180.f;
		for (GLfloat half = 0.5f; half >= 0; half -= 0.0005) {
			sp.emplace_back(glm::vec3{ half * cos(theta * 0.0174533),
													0.f,
													half * sin(theta * 0.0174533) });
			theta += 1.f;
		}

		glGenBuffers(1, &sp_v_pos);
		glBindBuffer(GL_ARRAY_BUFFER, sp_v_pos);
		glBufferData(GL_ARRAY_BUFFER, sp.size() * sizeof(glm::vec3), sp.data(), GL_STATIC_DRAW);
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