#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;
bool face = true;
bool drive = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 1 }; //����� 0
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	if (key == 'C') {
		face = !face;
	}
	if (key == 'D') {
		drive = !drive;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void Quad(double A[], double B[], double C[], double D[]) {
	glBegin(GL_QUADS);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glEnd();

}

void DrawWings() {
	//����� �����
	double A[3] = {1, 1, 6.08};
	double B[3] = { 1, 2, 6.08 };
	double C[3] = { 2, 2, 6.08 };
	double D[3] = { 2, 4, 6.08 };
	double E[3] = { 1, 4, 6.08 };
	double F[3] = { 1, 5, 6.08 };
	double G[3] = { 0, 5, 6.08 };
	double H[3] = { 0, 6, 6.08 };
	double I[3] = { -2, 6, 6.08 };
	double J[3] = { -2, 5, 6.08 };
	double K[3] = { -3, 5, 6.08 };
	double L[3] = { -3, 3, 6.08 };
	double M[3] = { -2, 3, 6.08 };
	double N[3] = { -2, 2, 6.08 };
	double O[3] = { -1, 2, 6.08 };
	double P[3] = { -1, 1, 6.08 };
	double X[3] = { 0, 4, 6.08 };
	double Y[3] = { -1, 3, 6.08 };
	//������ �����
	double A_2[3] = {7, 1, 6.08 };
	double B_2[3] = { 7, 2, 6.08 };
	double C_2[3] = { 8, 2, 6.08 };
	double D_2[3] = { 8, 3, 6.08 };
	double E_2[3] = { 9, 3, 6.08 };
	double F_2[3] = { 9, 5, 6.08 };
	double G_2[3] = { 8, 5, 6.08 };
	double H_2[3] = { 8, 6, 6.08 };
	double I_2[3] = { 6, 6, 6.08 };
	double J_2[3] = { 6, 5, 6.08 };
	double K_2[3] = { 5, 5, 6.08 };
	double L_2[3] = { 5, 4, 6.08 };
	double M_2[3] = { 4, 4, 6.08 };
	double N_2[3] = { 4, 2, 6.08 };
	double O_2[3] = { 5, 2, 6.08 };
	double P_2[3] = { 5, 1, 6.08 };
	double X_2[3] = { 7, 3, 6.08 };
	double Y_2[3] = { 6, 4, 6.08 };
	//����� �����
	glColor3f(1.0f, 1.0f, 1.0f);
	Quad(A, B, O, P);
	Quad(M, Y, O, N);
	Quad(K, J, M, L);
	Quad(G, H, I, J);
	Quad(E, F, G, X);
	Quad(B, C, D, E);
	//������ �����
	glColor3f(1.0f, 1.0f, 1.0f);
	Quad(A_2, B_2, O_2, P_2);
	Quad(M_2, L_2, O_2, N_2);
	Quad(K_2, J_2, Y_2, L_2);
	Quad(G_2, H_2, I_2, J_2);
	Quad(D_2, E_2, F_2, G_2);
	Quad(B_2, C_2, D_2, X_2);
}

void DrawBee()
{
	//����:
	double a[3] = { 0,0,0 };
	double b[3] = { 0,0,6 };
	double c[3] = { 6,0,6 };
	double d[3] = { 6,0,0 };

	//����:
	double a_back[3] = { 0,7,0 };
	double b_back[3] = { 0,7,6 };
	double c_back[3] = { 6,7,6 };
	double d_back[3] = { 6,7,0 };


	//����� ����:
	double left_eye_1[3] = { 1,-0.08,3 };
	double left_eye_2[3] = { 2,-0.08,3 };
	double left_eye_3[3] = { 2,-0.08,4 };
	double left_eye_4[3] = { 1,-0.08,4 };

	//������ ����:
	double right_eye_1[3] = { 4,-0.08,3 };
	double right_eye_2[3] = { 5,-0.08,3 };
	double right_eye_3[3] = { 5,-0.08,4 };
	double right_eye_4[3] = { 4,-0.08,4 };

	//����� ����:
	double left_black_1[3] = { 0,-0.05,1 };
	double left_black_2[3] = { 2,-0.05,1 };
	double left_black_3[3] = { 2,-0.05,4 };
	double left_black_4[3] = { 0,-0.05,4 };

	//������ ����:
	double right_black_1[3] = { 4,-0.05,1 };
	double right_black_2[3] = { 6,-0.05,1 };
	double right_black_3[3] = { 6,-0.05,4 };
	double right_black_4[3] = { 4,-0.05,4 };

	//�����:
	double center_1[3] = { 3,-0.05,4.5 };
	double center_2[3] = { 3,-0.05,5.5 };
	double center_3[3] = { 3,-1.0,5.5 };
	double center_4[3] = { 3,-1.0,4.5 };

	double left_brow_1[3] = { 1,-0.05,4.5 };
	double left_brow_2[3] = { 2,-0.05,4.5 };
	double left_brow_3[3] = { 2,-0.05,5.5 };
	double left_brow_4[3] = { 1,-0.05,5.5 };

	double left_brow_forward_1[3] = { 1,-0.05,4.5 };
	double left_brow_forward_2[3] = { 1,-0.05,5.5 };
	double left_brow_forward_3[3] = { 1,-0.5,5.5 };
	double left_brow_forward_4[3] = { 1,-0.5,4.5 };

	double left_brow_forward_2_1[3] = { 2,-0.05,4.5 };
	double left_brow_forward_2_2[3] = { 2,-0.05,5.5 };
	double left_brow_forward_2_3[3] = { 2,-0.5,5.5 };
	double left_brow_forward_2_4[3] = { 2,-0.5,4.5 };

	double right_brow_forward_1[3] = { 4,-0.05,4.5 };
	double right_brow_forward_2[3] = { 4,-0.05,5.5 };
	double right_brow_forward_3[3] = { 4,-0.5,5.5 };
	double right_brow_forward_4[3] = { 4,-0.5,4.5 };

	double right_brow_forward_2_1[3] = { 5,-0.05,4.5 };
	double right_brow_forward_2_2[3] = { 5,-0.05,5.5 };
	double right_brow_forward_2_3[3] = { 5,-0.5,5.5 };
	double right_brow_forward_2_4[3] = { 5,-0.5,4.5 };


	double right_brow_1[3] = { 4,-0.05,4.5 };
	double right_brow_2[3] = { 5,-0.05,4.5 };
	double right_brow_3[3] = { 5,-0.05,5.5 };
	double right_brow_4[3] = { 4,-0.05,5.5 };

	//���������� ������ 1:
	double stripe_1[3] = { 6,2,6.05 };
	double stripe_2[3] = { 6,3,6.05 };
	double stripe_3[3] = { 0,3,6.05 };
	double stripe_4[3] = { 0,2,6.05 };

	double stripe_right_1[3] = { 6.05,2,6.05 };
	double stripe_right_2[3] = { 6.05,3,6.05 };
	double stripe_right_3[3] = { 6.05,3,-0.05 };
	double stripe_right_4[3] = { 6.05,2,-0.05 };

	double stripe_left_1[3] = { -0.05,2,6.05 };
	double stripe_left_2[3] = { -0.05,3,6.05 };
	double stripe_left_3[3] = { -0.05,3,-0.05 };
	double stripe_left_4[3] = { -0.05,2,-0.05 };

	double stripe_floor_1[3] = { 6,2,-0.05 };
	double stripe_floor_2[3] = { 6,3,-0.05 };
	double stripe_floor_3[3] = { 0,3,-0.05 };
	double stripe_floor_4[3] = { 0,2,-0.05 };

	//���������� ������ 2:

	double stripe_1_2[3] = { 6,4,6.05 };
	double stripe_2_2[3] = { 6,5,6.05 };
	double stripe_3_2[3] = { 0,5,6.05 };
	double stripe_4_2[3] = { 0,4,6.05 };

	double stripe_right_1_2[3] = { 6.05,4,6.05 };
	double stripe_right_2_2[3] = { 6.05,5,6.05 };
	double stripe_right_3_2[3] = { 6.05,5,-0.05 };
	double stripe_right_4_2[3] = { 6.05,4,-0.05 };

	double stripe_left_1_2[3] = { -0.05,4,6.05 };
	double stripe_left_2_2[3] = { -0.05,5,6.05 };
	double stripe_left_3_2[3] = { -0.05,5,-0.05 };
	double stripe_left_4_2[3] = { -0.05,4,-0.05 };

	double stripe_floor_1_2[3] = { 6,4,-0.05 };
	double stripe_floor_2_2[3] = { 6,5,-0.05 };
	double stripe_floor_3_2[3] = { 0,5,-0.05 };
	double stripe_floor_4_2[3] = { 0,4,-0.05 };

	//���������� ������ 3:

	double stripe_1_3[3] = { 6,6,6.05 };
	double stripe_2_3[3] = { 6,7,6.05 };
	double stripe_3_3[3] = { 0,7,6.05 };
	double stripe_4_3[3] = { 0,6,6.05 };

	double stripe_right_1_3[3] = { 6.05,6,6.05 };
	double stripe_right_2_3[3] = { 6.05,7,6.05 };
	double stripe_right_3_3[3] = { 6.05,7,-0.05 };
	double stripe_right_4_3[3] = { 6.05,6,-0.05 };

	double stripe_left_1_3[3] = { -0.05,6,6.05 };
	double stripe_left_2_3[3] = { -0.05,7,6.05 };
	double stripe_left_3_3[3] = { -0.05,7,-0.05 };
	double stripe_left_4_3[3] = { -0.05,6,-0.05 };

	double stripe_floor_1_3[3] = { 6,6,-0.05 };
	double stripe_floor_2_3[3] = { 6,7,-0.05 };
	double stripe_floor_3_3[3] = { 0,7,-0.05 };
	double stripe_floor_4_3[3] = { 0,6,-0.05 };

	//��������� �����
	double legs_1_right[3] = { 2, 2, -1.5 };
	double legs_2_right[3] = { 3, 2, -1.5 };
	double legs_3_right[3] = { 3, 2, 0 };
	double legs_4_right[3] = { 2, 2, 0 };

	double legs_1_left[3] = { 3, 2, -1.5 };
	double legs_2_left[3] = { 4, 2, -1.5 };
	double legs_3_left[3] = { 4, 2, 0 };
	double legs_4_left[3] = { 3, 2, 0 };

	double legs_1_right_2[3] = { 1, 3, -1.5 };
	double legs_2_right_2[3] = { 2, 3, -1.5 };
	double legs_3_right_2[3] = { 2, 3, 0 };
	double legs_4_right_2[3] = { 1, 3, 0 };

	double legs_1_left_2[3] = { 4, 3, -1.5 };
	double legs_2_left_2[3] = { 5, 3, -1.5 };
	double legs_3_left_2[3] = { 5, 3, 0 };
	double legs_4_left_2[3] = { 4, 3, 0 };

	double legs_1_right_3[3] = { 1, 5, -1.5 };
	double legs_2_right_3[3] = { 2, 5, -1.5 };
	double legs_3_right_3[3] = { 2, 5, 0 };
	double legs_4_right_3[3] = { 1, 5, 0 };

	double legs_1_left_3[3] = { 4, 5, -1.5 };
	double legs_2_left_3[3] = { 5, 5, -1.5 };
	double legs_3_left_3[3] = { 5, 5, 0 };
	double legs_4_left_3[3] = { 4, 5, 0 };

	//�������� �� ������
	double legs_1_right_painted[3] = { 2, 1.95, -0.7 };
	double legs_2_right_painted[3] = { 3, 1.95, -0.7 };
	double legs_3_right_painted[3] = { 3, 1.95, 0 };
	double legs_4_right_painted[3] = { 2, 1.95, 0 };

	double legs_1_left_painted[3] = { 3, 1.95, -0.7 };
	double legs_2_left_painted[3] = { 4, 1.95, -0.7 };
	double legs_3_left_painted[3] = { 4, 1.95, 0 };
	double legs_4_left_painted[3] = { 3, 1.95, 0 };

	double legs_1_right_2_painted[3] = { 1, 2.95, -0.7 };
	double legs_2_right_2_painted[3] = { 2, 2.95, -0.7 };
	double legs_3_right_2_painted[3] = { 2, 2.95, 0 };
	double legs_4_right_2_painted[3] = { 1, 2.95, 0 };

	double legs_1_left_2_painted[3] = { 4, 2.95, -0.7 };
	double legs_2_left_2_painted[3] = { 5, 2.95, -0.7 };
	double legs_3_left_2_painted[3] = { 5, 2.95, 0 };
	double legs_4_left_2_painted[3] = { 4, 2.95, 0 };

	double legs_1_right_3_painted[3] = { 1, 4.95, -0.7 };
	double legs_2_right_3_painted[3] = { 2, 4.95, -0.7 };
	double legs_3_right_3_painted[3] = { 2, 4.95, 0 };
	double legs_4_right_3_painted[3] = { 1, 4.95, 0 };

	double legs_1_left_3_painted[3] = { 4, 4.95, -0.7 };
	double legs_2_left_3_painted[3] = { 5, 4.95, -0.7 };
	double legs_3_left_3_painted[3] = { 5, 4.95, 0 };
	double legs_4_left_3_painted[3] = { 4, 4.95, 0 };

	//����
	glColor3f(1.0f, 0.9f, 0.0f);
	glNormal3d(0, 0, -6);
	Quad(a, b, c, d);
	//����
	glColor3f(0.36f, 0.25f, 0.20f);
	Quad(a_back, b_back, c_back, d_back);

	glColor3f(1.0f, 0.9f, 0.0f);
	//������ ���
	//glNormal3d(0, 0, 6);
	Quad(d, c, c_back, d_back);
	//����� ���
	//glNormal3d(-6, 0, 0);
	Quad(a, b, b_back, a_back);
	//�����
	Quad(b, c, c_back, b_back);
	//�����
	Quad(a, d, d_back, a_back);


	if (face) {
		glColor3f(0.0f, 0.0f, 0.7f);
		//����� ����:
		Quad(left_eye_1, left_eye_2, left_eye_3, left_eye_4);
		//������ ����:
		Quad(right_eye_1, right_eye_2, right_eye_3, right_eye_4);

		glColor3f(0.0f, 0.0f, 0.0f);
		//����� ����:
		Quad(left_black_1, left_black_2, left_black_3, left_black_4);
		//������� ����:
		Quad(right_black_1, right_black_2, right_black_3, right_black_4);
	}
	else {
		double glare_1[3] = { 1, -0.08, 2 };
		double glare_2[3] = { 2, -0.08, 2 };
		double glare_3[3] = { 2, -0.08, 3 };
		double glare_4[3] = { 1, -0.08, 3 };

		double glare_1_2[3] = { 4, -0.08, 2 };
		double glare_2_2[3] = { 5, -0.08, 2 };
		double glare_3_2[3] = { 5, -0.08, 3 };
		double glare_4_2[3] = { 4, -0.08, 3 };
		glColor3f(1.0f, 1.0f, 1.0f);
		//����� ����:
		Quad(glare_1, glare_2, glare_3, glare_4);
		//������ ����:
		Quad(glare_1_2, glare_2_2, glare_3_2, glare_4_2);

		double angry_1[3] = { 0, -0.05, 1 };
		double angry_2[3] = { 0, -0.05, 3 };
		double angry_3[3] = { 0, -0.05, 4 };
		double angry_4[3] = { 1, -0.05, 4 };
		double angry_5[3] = { 1, -0.05, 3 };
		double angry_6[3] = { 2, -0.05, 3 };
		double angry_7[3] = { 2, -0.05, 1 };

		double angry_1_2[3] = { 4, -0.05, 1 };
		double angry_2_2[3] = { 4, -0.05, 3 };
		double angry_3_2[3] = { 5, -0.05, 3 };
		double angry_4_2[3] = { 5, -0.05, 4 };
		double angry_5_2[3] = { 6, -0.05, 4 };
		double angry_6_2[3] = { 6, -0.05, 3 };
		double angry_7_2[3] = { 6, -0.05, 1 };

		glColor3f(1.0f, 0.0f, 0.0f);
		//����� ����:
		Quad(angry_2, angry_3, angry_4, angry_5);
		Quad(angry_1, angry_2, angry_6, angry_7);
		//������� ����:
		Quad(angry_6_2, angry_3_2, angry_4_2, angry_5_2);
		Quad(angry_1_2, angry_2_2, angry_6_2, angry_7_2);
	}


	//�����:
	glColor3f(0.0f, 0.0f, 0.0f);
	Quad(center_1, center_2, center_3, center_4);
	Quad(left_brow_1, left_brow_2, left_brow_3, left_brow_4);
	Quad(right_brow_1, right_brow_2, right_brow_3, right_brow_4);
	Quad(left_brow_forward_1, left_brow_forward_2, left_brow_forward_3, left_brow_forward_4);
	Quad(left_brow_forward_2_1, left_brow_forward_2_2, left_brow_forward_2_3, left_brow_forward_2_4);
	Quad(right_brow_forward_1, right_brow_forward_2, right_brow_forward_3, right_brow_forward_4);
	Quad(right_brow_forward_2_1, right_brow_forward_2_2, right_brow_forward_2_3, right_brow_forward_2_4);


	//���������� ������
	glColor3f(0.36f, 0.25f, 0.20f);
	Quad(stripe_1, stripe_2, stripe_3, stripe_4);
	Quad(stripe_floor_1, stripe_floor_2, stripe_floor_3, stripe_floor_4);
	Quad(stripe_right_1, stripe_right_2, stripe_right_3, stripe_right_4);
	Quad(stripe_left_1, stripe_left_2, stripe_left_3, stripe_left_4);

	Quad(stripe_1_2, stripe_2_2, stripe_3_2, stripe_4_2);
	Quad(stripe_floor_1_2, stripe_floor_2_2, stripe_floor_3_2, stripe_floor_4_2);
	Quad(stripe_right_1_2, stripe_right_2_2, stripe_right_3_2, stripe_right_4_2);
	Quad(stripe_left_1_2, stripe_left_2_2, stripe_left_3_2, stripe_left_4_2);

	Quad(stripe_1_3, stripe_2_3, stripe_3_3, stripe_4_3);
	Quad(stripe_floor_1_3, stripe_floor_2_3, stripe_floor_3_3, stripe_floor_4_3);
	Quad(stripe_right_1_3, stripe_right_2_3, stripe_right_3_3, stripe_right_4_3);
	Quad(stripe_left_1_3, stripe_left_2_3, stripe_left_3_3, stripe_left_4_3);

	//���������� �����
	glColor3f(0.36f, 0.25f, 0.20f);
	Quad(legs_1_right, legs_2_right, legs_3_right, legs_4_right);
	Quad(legs_1_left, legs_2_left, legs_3_left, legs_4_left);
	Quad(legs_1_right_2, legs_2_right_2, legs_3_right_2, legs_4_right_2);
	Quad(legs_1_left_2, legs_2_left_2, legs_3_left_2, legs_4_left_2);
	Quad(legs_1_right_3, legs_2_right_3, legs_3_right_3, legs_4_right_3);
	Quad(legs_1_left_3, legs_2_left_3, legs_3_left_3, legs_4_left_3);

	//�������� �� ������
	glColor3f(0.3f, 0.19f, 0.14f);
	Quad(legs_1_right_painted, legs_2_right_painted, legs_3_right_painted, legs_4_right_painted);
	Quad(legs_1_left_painted, legs_2_left_painted, legs_3_left_painted, legs_4_left_painted);
	Quad(legs_1_right_2_painted, legs_2_right_2_painted, legs_3_right_2_painted, legs_4_right_2_painted);
	Quad(legs_1_left_2_painted, legs_2_left_2_painted, legs_3_left_2_painted, legs_4_left_2_painted);
	Quad(legs_1_right_3_painted, legs_2_right_3_painted, legs_3_right_3_painted, legs_4_right_3_painted);
	Quad(legs_1_left_3_painted, legs_2_left_3_painted, legs_3_left_3_painted, legs_4_left_3_painted);

	DrawWings();
}

void DrawPlane() {
	double A[3] = { -50, -50, -10 };
	double B[3] = { -50, 60, -10 };
	double C[3] = { 60, 60, -10 };
	double D[3] = { 60, -50, -10 };
	glColor3f(0.35f, 0.5f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glEnd();

}

inline double fact(int n) {
	int r = 1;
	for (int i = 2; i <= n; ++i) {
		r *= i;
	}
	return r;
}
inline double Bern(int i, int n, double u) {
	return 1.0 * fact(n) * pow(u, i) * pow(1 - u, n - i) / fact(i) / fact(n - i);
}

inline Vector3 Bezier(Vector3* points, int n, int m, double u, double v) {
	Vector3 res(0, 0, 0);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			double Bi = Bern(i, n - 1, u);
			double Bj = Bern(j, m - 1, v);
			res = res + points[i * m + j] * Bi * Bj;
		}
	}
	return res;
}

Vector3 Bcurve(Vector3* points, int n, double t) {
	Vector3 res(0, 0, 0);
	for (int i = 0; i < n; i++)
		res = res + points[i] * Bern(i, n - 1, t);
	return res;
}

float anim_h = 0.01;
double anim_t = anim_h;

void Render(OpenGL* ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//DrawBee();

	
	if (drive) {
		Vector3 p[4];
		p[0] = Vector3(0, 0, 0);
		p[1] = Vector3(-15, 20, 40);
		p[2] = Vector3(25, 50, -10);
		p[3] = Vector3(-25, -6, 10);
		glBegin(GL_LINE_STRIP);          // ��� ������ ������, � �������� ������� ��������� ������
		glVertex3dv(p[0].toArray());
		glVertex3dv(p[1].toArray());
		glVertex3dv(p[2].toArray());
		glVertex3dv(p[3].toArray());
		glEnd();

		glPushMatrix();
		Vector3 pos = Bcurve(p, 4, anim_t);
		Vector3 pre_pos = Bcurve(p, 4, anim_t - anim_h);
		Vector3 dir = (pos - pre_pos).normolize();

		//dir = Vector3(1, 0.1, 0.1);

		Vector3 orig(1, 0, 0);
		Vector3 rotX(dir.X(), dir.Y(), 0);
		rotX = rotX.normolize();
		double cosU = (orig.X() * rotX.X()) + (orig.Y() * rotX.Y()) + (orig.Z() * rotX.Z());
		Vector3 vecpr = orig.vectProisvedenie(rotX);

		double sinSign = vecpr.Z() / abs(vecpr.Z());
		double U = acos(cosU) * 180 / 3.141592 * sinSign;

		double cosZU = (0 * dir.X()) + (0 * dir.Y()) + (1 * dir.Z());
		double ZU = acos(dir.Z()) * 180.0 / M_PI - 90;

		glTranslated(pos.X(), pos.Y(), pos.Z());
		glRotated(U, 0, 0, 1);
		glRotated(ZU, 0, 1, 0);

		DrawBee();
		glPopMatrix();

		glDisable(GL_LIGHTING);
		glColor3d(1, 0.3, 0);

		glBegin(GL_LINE);
		glVertex3dv(pos.toArray());
		glVertex3dv((pos + dir.normolize() * 3).toArray());


		glEnd();

		anim_t += anim_h;
		if (anim_t > 1) anim_h = -anim_h;
		if (anim_t < 0) anim_h = -anim_h;

		glBegin(GL_LINE_STRIP);
		glDisable(GL_LIGHTING);
		glColor3d(1, 0, 1);
		for (double t = 0; t <= 1; t += 0.01) {
			glVertex3dv(Bcurve(p, 4, t).toArray());
		}
		glEnd();
	}
	else DrawBee();

	DrawPlane();

	//===================================
   //��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 200);
	rec.setPosition(10, ogl->getHeight() - 200 - 10);


	std::stringstream ss;
	ss << "� - ��������� ������ �����" << std::endl;
	ss << "D - ���/���� �������� �� ������" << std::endl;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}