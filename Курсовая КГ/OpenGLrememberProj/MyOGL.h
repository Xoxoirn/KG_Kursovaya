#ifndef MYOGL_H
#define MYOGL_H

#pragma warning( disable : 4305 )
#pragma warning( disable : 4244 )

#include <vector>
#include "MyVector3d.h"

#include <windows.h>
#include <gl/GL.h>
#include "gl/GLU.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

class OpenGL;


//тип для делегата void f (void)
typedef void(*action)(OpenGL *) ;
typedef void(*MouseEventDelegate)(OpenGL *,int,int);
typedef void(*WheelEventDelegate)(OpenGL *, int);
typedef void(*KeyEventDelegate)(OpenGL*, int);
typedef void(*ResizeEventDelegate)(OpenGL *, int, int);

class Camera;
class Light;

class OpenGL
{
	HWND g_hWnd;
	HDC g_hDC;
	HGLRC g_hRC;
	int width, height;
	

	HBITMAP bitmap;
	HDC bitmap_hdc;
	HDC dc;

	void createBitmap();
	

public:

	

	//функции рендера
	std::vector < action > renderFunc;

	//render GUI

	action renderGuiFunc;
	ResizeEventDelegate resizeFunc;

	//функции инициализации
	std::vector < action > initFunc;
	
	std::vector <MouseEventDelegate>  mouseFunc;
	std::vector <WheelEventDelegate>  wheelFunc;
	std::vector <KeyEventDelegate>  keyDownFunc;
	std::vector <KeyEventDelegate>  keyUpFunc;

	Camera *mainCamera;
	Light *mainLight;

	int getWidth()
	{
		return width;
	}

	int getHeight()
	{
		return height;
	}
	HWND getHwnd()
	{
		return g_hWnd;
	}

	OpenGL();
	~OpenGL();

	void setHWND(HWND window);
	

	int OldMouseX, OldMouseY;
	void mouseMovie(int mX, int mY);
	void wheelEvent(float delta);
	void keyDownEvent(int key);
	void keyUpEvent(int key);

	void DrawAxes();

	void render();
	void resize(int w, int h);
	void init(void);

	double aspect;

	std::string message;

	static bool isKeyPressed(int key)
	{
		
		//стандартная функция WinApi для чтения нажатой клавиши
		//см https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms646293(v=vs.85).aspx
		
		short state = GetAsyncKeyState(key);


		//  0x8000 припереводе из 16ричной  в двоичную давет 1000 0000 0000 0000 .
		//  операция & совершает побитовое И над операндами.    Таким образом после нее остется первый бит (0 или 1),
		//  в нем и содержится информация о там нажата ли клавиша
		return static_cast<bool>(state & 0x8000);
	}

	//загрузка BMP из файла,
	
	static int LoadBMP(__in LPCSTR  filename, __out int* Wigth, __out int* Height, __out RGBTRIPLE** arr)
	{
		DWORD nBytesRead = 0;
		int read_size = 0;
		int i = 0;
		int width, height, size;
		BITMAPINFOHEADER infoh;
		BITMAPFILEHEADER fileh;

		HANDLE file = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);


		//Считываем заголовки BMP файла
		ReadFile((HANDLE)file, &fileh, sizeof(BITMAPFILEHEADER), &nBytesRead, 0);
		ReadFile((HANDLE)file, &infoh, sizeof(BITMAPINFOHEADER), &nBytesRead, 0);

		/*
		Подробнее о заголовках https://msdn.microsoft.com/ru-ru/library/windows/desktop/dd183374(v=vs.85).aspx
							   https://msdn.microsoft.com/ru-ru/library/windows/desktop/dd183376(v=vs.85).aspx

		*/

		width = infoh.biWidth;
		height = infoh.biHeight;
		*Wigth = width;
		*Height = height;


		//
		size = width * 3 + width % 4;
		size = size * height;
		nBytesRead = fileh.bfOffBits;
		*arr = (RGBTRIPLE*)malloc(size);

		while (read_size < size)
		{
			ReadFile(file, *arr + i, sizeof(RGBTRIPLE), &nBytesRead, 0);
			read_size += nBytesRead;
			i++;
		}
		CloseHandle(file);
		return 1;

	}


	//Переводим BMP в массив чароов.  Один пиксаль кодируется последовательно 4мя байтами (R G B A)
	//Так же тут картинка переворачивается, в BMP она хранится перевернутой.
	static int RGBtoChar(__in RGBTRIPLE* arr, __in int width, __in int height, __out char** out)
	{
		int size = height * width * 4;
		char* mas;
		if (width <= 0 || height <= 0)
		{
			return 0;
		}

		mas = (char*)malloc(size * sizeof(char));
		for (int i = height - 1; i >= 0; i--)
			for (int j = 0; j < width; j++)
			{
				*(mas + i * width * 4 + j * 4 + 0) = arr[(i)*width + j].rgbtRed;
				*(mas + i * width * 4 + j * 4 + 1) = arr[(i)*width + j].rgbtGreen;
				*(mas + i * width * 4 + j * 4 + 2) = arr[(i)*width + j].rgbtBlue;
				*(mas + i * width * 4 + j * 4 + 3) = 0;
			}
		*out = mas;
		return 1;

	}
	static void drawSphere()
	{

	}

};







#endif