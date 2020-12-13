#include "Render.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"
#include "MyOGL.h"
#include <math.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include "CVector.h"
#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include "MyShaders.h"
#include "ObjLoader.h"
#include "GUItextRectangle.h"
#include "Texture.h"
#include "MyVector3d.h"


#define TOP_RIGHT 1.0f,1.0f
#define TOP_LEFT 0.0f,1.0f
#define BOTTOM_RIGHT 1.0f,0.0f
#define BOTTOM_LEFT 0.0f,0.0f

double delta_time=0.01;
GLuint texId[3];
GuiTextRectangle rec;
bool textureMode = true;
bool lightMode = true;
bool shadow = false;
//��������� ������ ��� ��������� ����
#define POP glPopMatrix()
#define PUSH glPushMatrix()
ObjFile *model;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

Shader s[10];  //��������� ��� ������ ��������
Shader frac;
Shader cassini;

double angle2 = 10;
bool DeerFlag = true;
bool StarFlag = false;
bool FlyDeerFlag = false;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	Vector3 cameraFront;

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
		lookPoint.setCoords(-11, 4.47, 1.92);

		pos.setCoords(-15.7, 9.15, 3.7);

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

	void SetLookPoint(double x, double y, double z)
	{
		lookPoint.setCoords(x, y, z);
	}

	void SetLookPoint(Vector3 vec)
	{
		lookPoint = vec;
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
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
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
			c.scale = c.scale*1.5;
			c.Show();
		}
		/*
		if (f1)
			glEnable(GL_LIGHTING);
		if (f2)
			glEnable(GL_TEXTURE_2D);
		if (f3)
			glEnable(GL_DEPTH_TEST);
			*/
	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
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
}
light;  //������� �������� �����
//������ ���������� ����
int mouseX = 0, mouseY = 0;
float offsetX = 0, offsetY = 0;
float zoom=1;
float Time = 0;
int tick_o = 0;
int tick_n = 0;

void NormalizeVector(double* vec)
{
	double modVector = -sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2)); // ����� �������

	for (int i = 0; i < 3; ++i)
	{
		vec[i] /= modVector;
	}
}
void FindNormal(double* a, double* b, double* c, int FlagSwap = 0) // �� ����� � ���� ��� �������
{
	double vectorA[3], vectorB[3];

	for (int i = 0; i < 3; ++i) // �������� ������ A � B
	{
		vectorA[i] = a[i] - c[i];
		vectorB[i] = b[i] - c[i];
	}

	double VectorNormal[3];

	VectorNormal[0] = vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2]; //���������� ������� �� ������
	VectorNormal[1] = -vectorA[0] * vectorB[2] + vectorB[0] * vectorA[2];
	VectorNormal[2] = vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1];

	NormalizeVector(VectorNormal);

	if (FlagSwap != 0)
	{
		for (int i = 0; i < 3; ++i) // �������� ������ A � B
		{
			VectorNormal[i] *= -1;
		}
	}

	glNormal3dv(VectorNormal);
}

//���������� �������� ����
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.005 * dx;
		camera.fi2 += -0.005 * dy;
		camera.pos.setCoords(camera.camDist * cos(camera.fi2) * cos(camera.fi1),
			camera.camDist * cos(camera.fi2) * sin(camera.fi1),
			camera.camDist * sin(camera.fi2));
		//camera.camDist = 10;
		

	}


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}
	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y,60,ogl->aspect);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

//���������� �������� ������  ����
void mouseWheelEvent(OpenGL *ogl, int delta)
{
	//��� ������ ������ ��� �������� �������
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.camDist += 0.005 * delta;
		camera.pos.setCoords(camera.camDist * cos(camera.fi2) * cos(camera.fi1),
			camera.camDist * cos(camera.fi2) * sin(camera.fi1),
			camera.camDist * sin(camera.fi2));
	
	}

}

//���������� ������� ������ ����������
void keyDownEvent(OpenGL *ogl, int key)
{
	if (OpenGL::isKeyPressed('L'))
	{
		lightMode = !lightMode;
	}

	if (OpenGL::isKeyPressed('T'))
	{
		textureMode = !textureMode;
	}	   

	if (OpenGL::isKeyPressed('F'))
	{
		light.pos = camera.pos;
	}

	if (OpenGL::isKeyPressed('W'))
	{
		if (DeerFlag)
		{
			if (angle2 >= 40)
			{
				angle2 = 40;
				DeerFlag = false;
			}
			else
			{
				angle2 += 1;
			}
		}

		else 
		{
			if (angle2 <= 10)
			{
				angle2 = 10;
				DeerFlag = true;
			}
			else
			{
				angle2 -= 1;
			}
		}
	}

	if (OpenGL::isKeyPressed('S'))
	{
		StarFlag = !StarFlag;
	}

	if (OpenGL::isKeyPressed('E'))
	{
		FlyDeerFlag = !FlyDeerFlag;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}

ObjFile objModel, Body, Head, Star, NewYear, FlyDeer;
Texture DeerTex, FonTex;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
#pragma region MyRegion



	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);

	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 
	
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	/*
	//texture1.loadTextureFromFile("textures\\texture.bmp");   �������� �������� �� �����
	*/


	frac.VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	frac.FshaderFileName = "shaders\\frac.frag"; //��� ����� ������������ �������
	frac.LoadShaderFromFile(); //��������� ������� �� �����
	frac.Compile(); //�����������

	cassini.VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	cassini.FshaderFileName = "shaders\\cassini.frag"; //��� ����� ������������ �������
	cassini.LoadShaderFromFile(); //��������� ������� �� �����
	cassini.Compile(); //�����������
	

	s[0].VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	s[0].FshaderFileName = "shaders\\light.frag"; //��� ����� ������������ �������
	s[0].LoadShaderFromFile(); //��������� ������� �� �����
	s[0].Compile(); //�����������

	s[1].VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	s[1].FshaderFileName = "shaders\\textureShader.frag"; //��� ����� ������������ �������
	s[1].LoadShaderFromFile(); //��������� ������� �� �����
	s[1].Compile(); //�����������

	

	 //��� ��� ��� ������� ������ *.obj �����, ��� ��� ��� ��������� �� ���������� � ���������� �������, 
	 // ������������ �� ����� ����������, � ������������ ������ � *.obj_m
#pragma endregion

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Salju.obj_m", &objModel);
	FonTex.loadTextureFromFile("textures//FonTex.bmp");
	FonTex.bindTexture();

	loadModel("models\\Body.obj_m", &Body);
	DeerTex.loadTextureFromFile("textures//DeerTex.bmp");
	DeerTex.bindTexture();

	loadModel("models\\Head.obj_m", &Head);

	loadModel("models\\Star.obj_m", &Star);

	loadModel("models\\NewYear.obj_m", &NewYear);

	loadModel("models\\FlyDeer.obj_m", &FlyDeer);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("textures//Sky.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//���������� �� ��� ��������
	glGenTextures(1, &texId[0]);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId[0]);

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
}

double f(double p0, double p1, double p2, double p3, double t)// ������� ������ �����
{
	return (p0 * (1 - t) * (1 - t) * (1 - t)) + (3 * t * (1 - t) * (1 - t) * p1) + (3 * t * t * (1 - t) * p2) + t * t * t * p3; //����������� �������
}

Vector3 Bezye11(double* P1, double* P2, double P3[3], double P4[3], double t)
{

	double P[4];

	P[0] = f(P1[0], P2[0], P3[0], P4[0], t);
	P[1] = f(P1[1], P2[1], P3[1], P4[1], t);
	P[2] = f(P1[2], P2[2], P3[2], P4[2], t);
	P[3] = f(P1[3], P2[3], P3[3], P4[3], t);

	Vector3 D;
	D.setCoords(P[0], P[1], P[2]);
	return D;
}

void Bese2(double delta_time)
{
	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max ���������� = 1 �� 5 ������
		if (t_max > 1)
		{
			t_max = 1; //����� ����������
			flagReverse = !flagReverse;
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max ���������� = 1 �� 5 ������
		if (t_max < 0)
		{
			t_max = 0; //����� ����������
			flagReverse = !flagReverse;
		}
	}
	double P1[] = { 60,10,13 }; //���� �����
	double P2[] = { 35,5,9 };
	double P3[] = { 10,5,4 };
	double P4[] = { -20,5,4 };


	Vector3 P_old = Bezye11(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = Bezye11(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	double A[] = { -0.5,-0.5,-0.5 };
	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(90, 1, 0, 0);	
	glRotated(25, 1, 0, 0);
	//DeerTex.bindTexture();
	FlyDeer.DrawObj();
	glPopMatrix();

}
void Render(OpenGL *ogl)
{   
#pragma region MyRegion



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);



	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	

	//��������� ���������
	GLfloat amb[] = { 0.7, 0.7, 0.7, 1. };
	GLfloat dif[] = { 0.9, 0.9, 0.9, 1. };
	GLfloat spec[] = { 0.9, 0.9, 0.9, 1. };
	GLfloat sh = 0.1f * 256;

	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//������� ���  




	s[0].UseShader();

	//�������� ���������� � ������.  ��� ���� - ���� ����� uniform ���������� �� �� �����. 
	int location = glGetUniformLocationARB(s[0].program, "light_pos");
	//��� 2 - �������� �� ��������
	glUniform3fARB(location, light.pos.X(), light.pos.Y(),light.pos.Z());

	location = glGetUniformLocationARB(s[0].program, "Ia");
	glUniform3fARB(location, 0.2, 0.2, 0.2);

	location = glGetUniformLocationARB(s[0].program, "Id");
	glUniform3fARB(location, 1.0, 1.0, 1.0);

	location = glGetUniformLocationARB(s[0].program, "Is");
	glUniform3fARB(location, .7, .7, .7);


	location = glGetUniformLocationARB(s[0].program, "ma");
	glUniform3fARB(location, 0.2, 0.2, 0.1);

	location = glGetUniformLocationARB(s[0].program, "md");
	glUniform3fARB(location, 0.4, 0.65, 0.5);

	location = glGetUniformLocationARB(s[0].program, "ms");
	glUniform4fARB(location, 0.9, 0.8, 0.3, 25.6);

	location = glGetUniformLocationARB(s[0].program, "camera");
	glUniform3fARB(location, camera.pos.X(), camera.pos.Y(), camera.pos.Z());
#pragma endregion


	Shader::DontUseShaders();


	glPushMatrix();
	glTranslated(-11, 4.47, 1.92);
	glScaled(0.5, 0.5, 0.5);
	glRotated(180, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glRotated(angle2, 1, 0, 0);
	DeerTex.bindTexture();
	Head.DrawObj();
	glPopMatrix();


	glPushMatrix();
	glTranslated(-11, 4, 0.5);
	glScaled(0.5, 0.5, 0.5);
	glRotated(180, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	Body.DrawObj();
	glPopMatrix();

	if (FlyDeerFlag == true)
	{
		glPushMatrix();
		Bese2(delta_time);
		glPopMatrix();
	}
double B[] = { 30, -5, 18 };
double C[] = { 30, -5, -7 };
double D[] = { -13, -5,  -7 };
double E[] = { -13, -5, 18 };

glPushMatrix();
glScaled(2, 2, 1);
glRotated(25, 0, 0, 1);
FindNormal(C, B, E);
glBindTexture(GL_TEXTURE_2D, texId[0]);
glBegin(GL_QUADS);
glTexCoord2d(TOP_LEFT);
glVertex3dv(B);
glTexCoord2d(BOTTOM_LEFT);
glVertex3dv(C);
glTexCoord2d(BOTTOM_RIGHT);
glVertex3dv(D);
glTexCoord2d(TOP_RIGHT);
glVertex3dv(E);
glEnd();
glPopMatrix();


glPushMatrix();
glRotated(180, 0, 0, 1);
glRotated(90, 1, 0, 0);
FonTex.bindTexture();
objModel.DrawObj();
glPopMatrix();


if (StarFlag == true)
{
	glPushMatrix();

	glRotated(180, 0, 0, 1);
	glRotated(90, 1, 0, 0);

	NewYear.DrawObj();
	glPopMatrix();

	glPushMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glTranslated(-13, 6.2, 2.2);
	glScaled(0.3, 0.3, 0.3);
	glRotated(180, 0, 0, 1);
	glRotated(90, 1, 0, 0);

	Star.DrawObj();
	glPopMatrix();

}

}   //����� ���� �������

bool gui_init = false;

//������ ���������, ��������� ����� �������� �������
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

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
	rec.setSize(300, 250);
	rec.setPosition(10, ogl->getHeight() - 250 - 10);


	std::stringstream ss;

	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	//ss << "G - ������� ���� �� �����������" << std::endl;
	//ss << "G+��� ������� ���� �� ���������" << std::endl;
	//ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;


	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
		
	Shader::DontUseShaders(); 
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

