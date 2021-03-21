#include <GL/glew.h>
#include <stdio.h>
#include <windows.h>
#include "DelaunayTriangulation.h"

#define ID_ENABLE_CIRCLES 100
#define ID_CLEAR 101

//Global Variables
DelaunayTriangulation* delaunayTrianguationObj;
HDC g_HDC;
glm::vec3 mouseClickPos;
int windowWidth, windowHeight;
std::vector<glm::vec3> points;
bool done = false;
bool isDisplayCircles = true;

void Initialize();
void Resize(int width, int height);
void Render();
void Display();
void DrawCircle(float, float, float, int);
void Grid();
void CursorMove(POINT p);
void SetupPixelFormat(HDC hDC);

/*
	CALLBACK: calling convention
*/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdlineArgs, int nCmdshow)
{
	WNDCLASS wndclass;
	HWND hwnd;
	MSG msg;
	DWORD   dwStyle;
	RECT    windowRect;

	/*
		WndClass is a structure used to set the properties of the window to be created.
	*/

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = "Delaunay Triangulation";
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = (WNDPROC)WndProc;
	wndclass.hInstance = GetModuleHandle(NULL);
	wndclass.hbrBackground = NULL;
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_HAND);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("TEXT INSIDE MESSAGE BOX"), TEXT("TITLE FOR THE MESSAGE BOX"), MB_OK);
		return -1;
	}

	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

	int width = 1000, height = 1000;

	hwnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		"Delaunay Triangulation",
		TEXT("Delaunay Triangulation"),
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, SW_SHOW);

	delaunayTrianguationObj = new DelaunayTriangulation();

	while (!done)
	{
		PeekMessage(&msg, hwnd, NULL, NULL, PM_REMOVE);

		if (msg.message == WM_QUIT)     //did we receive a quit message?
		{
			done = true;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			Render();
		}
	}
	return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HGLRC hRC;
	static HDC hDC;
	int width, height;

	switch (msg)
	{
	case WM_CREATE:
	{
		hDC = GetDC(hwnd);  //get current windows device context
		g_HDC = hDC;
		SetupPixelFormat(hDC); //call our pixel format setup function
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);

		HMENU hMenu = CreateMenu();
		AppendMenu(hMenu, MF_STRING, ID_ENABLE_CIRCLES, "Display Circumcircles");
		AppendMenu(hMenu, MF_STRING, ID_CLEAR, "Clear");
		SetMenu(hwnd, hMenu);

		Initialize();
		break;
	}
	case WM_SIZE:
	{
		height = HIWORD(lparam);
		width = LOWORD(lparam);
		windowHeight = height;
		windowWidth = width;
		Resize(width, height);
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
			case ID_ENABLE_CIRCLES:
			{
				isDisplayCircles = !isDisplayCircles;
				break;
			}
			case ID_CLEAR:
			{
				points.clear();
				delaunayTrianguationObj->triangles.clear();
				break;
			}

		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		POINT p;
		GetCursorPos(&p);
		 
		mouseClickPos.x = p.x; // Set the x-coord variable
		mouseClickPos.y = p.y; // Set the y-coord variable

		mouseClickPos.x = mouseClickPos.x - windowWidth/ 2;
		mouseClickPos.y = windowHeight / 2 - mouseClickPos.y;

		if (points.size() > 0 )
		{
			if (mouseClickPos != points.back())
			{
				points.push_back(mouseClickPos);
			}
		} 
		else
		{
			points.push_back(mouseClickPos);
		}

		if (points.size() > 2)
		{
			if (delaunayTrianguationObj != NULL)
			{
				delete(delaunayTrianguationObj);
				delaunayTrianguationObj = NULL;
			}

			delaunayTrianguationObj = new DelaunayTriangulation();

			delaunayTrianguationObj->boundingBox[0].coordinates.x = 99999;
			delaunayTrianguationObj->boundingBox[0].coordinates.y = 99999;

			delaunayTrianguationObj->boundingBox[1].coordinates.x = -99999;
			delaunayTrianguationObj->boundingBox[1].coordinates.y = -99999;

			for (int i = 0; i < points.size(); i++) {

				Point* p = (Point*)malloc(sizeof(Point));
				p->coordinates = points[i];
				p->coordinates.z = 0;

				if (points[i].x < delaunayTrianguationObj->boundingBox[0].coordinates.x)
				{
					delaunayTrianguationObj->boundingBox[0].coordinates.x = points[i].x;
				}
				if (points[i].y < delaunayTrianguationObj->boundingBox[0].coordinates.y)
				{
					delaunayTrianguationObj->boundingBox[0].coordinates.y = points[i].y;
				}

				if (points[i].x > delaunayTrianguationObj->boundingBox[1].coordinates.x)
				{
					delaunayTrianguationObj->boundingBox[1].coordinates.x = points[i].x;
				}
				if (points[i].y > delaunayTrianguationObj->boundingBox[1].coordinates.y)
				{
					delaunayTrianguationObj->boundingBox[1].coordinates.y = points[i].y;
				}

				delaunayTrianguationObj->vertices.push_back(p);
			}

			delaunayTrianguationObj->setSuperTriangle();
			delaunayTrianguationObj->triangulate();
		}

		Render();
	
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wparam)
		{
			case VK_ESCAPE:
			{
				PostQuitMessage(0);
				break;
			}
		}
		break;
	}
	case WM_CLOSE:
	{
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
		done = true;
		
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		done = true;
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

void Initialize()
{
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}

void Resize(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-width / 2, width / 2, -height / 2, height / 2, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	POINT curp;
	GetCursorPos(&curp);

	glLoadIdentity();

	glPushMatrix();
	Grid();
	CursorMove(curp);
	Display();
	glPopMatrix();
	glFlush();
}

void Display(void)
{

	for (int i = 0; i < points.size(); i++)
	{
		glColor3f(1.0, 1.0, 0);
		glPointSize(10.0);
		glBegin(GL_POINTS);
		glVertex2f(points[i].x, points[i].y);
		glEnd();

	}

	if (isDisplayCircles) {
		for (int i = 0; i < delaunayTrianguationObj->triangles.size(); i++)
		{
			DrawCircle(delaunayTrianguationObj->triangles[i].center.coordinates.x, delaunayTrianguationObj->triangles[i].center.coordinates.y, delaunayTrianguationObj->triangles[i].radius, 100);
		}
	}

	for (int i = 0; i < delaunayTrianguationObj->triangles.size(); i++)
	{
		glColor4f(1.0, 0, 0, 0.2);

		glBegin(GL_TRIANGLES);

		glLineWidth(3.0);

		glVertex2f(delaunayTrianguationObj->triangles[i].p1->coordinates.x, delaunayTrianguationObj->triangles[i].p1->coordinates.y);
		glVertex2f(delaunayTrianguationObj->triangles[i].p2->coordinates.x, delaunayTrianguationObj->triangles[i].p2->coordinates.y);
		glVertex2f(delaunayTrianguationObj->triangles[i].p3->coordinates.x, delaunayTrianguationObj->triangles[i].p3->coordinates.y);

		glEnd();
	}

	for (int i = 0; i < delaunayTrianguationObj->triangles.size(); i++)
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glBegin(GL_LINE_LOOP);

		glLineWidth(5.0);

		glVertex2f(delaunayTrianguationObj->triangles[i].p1->coordinates.x, delaunayTrianguationObj->triangles[i].p1->coordinates.y);
		glVertex2f(delaunayTrianguationObj->triangles[i].p2->coordinates.x, delaunayTrianguationObj->triangles[i].p2->coordinates.y);
		glVertex2f(delaunayTrianguationObj->triangles[i].p3->coordinates.x, delaunayTrianguationObj->triangles[i].p3->coordinates.y);

		glEnd();
	}

	SwapBuffers(g_HDC);
}

void DrawCircle(float cx, float cy, float r, int num_segments)
{
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++) {
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 
		float x = r * cosf(theta);//calculate the x component 
		float y = r * sinf(theta);//calculate the y component 
		glVertex2f(x + cx, y + cy);//output vertex 
	}
	glEnd();
}

void Grid()
{
	for (int i = -windowWidth / 2; i < windowWidth / 2; i += 10)
	{
		glColor3f(0.2, 0.2, 0.2);

		glBegin(GL_LINES);

		glVertex2f(i, -windowHeight / 2);
		glVertex2f(i, windowHeight / 2);

		glEnd();
	}

	for (int i = -windowHeight / 2; i < windowHeight / 2; i += 10)
	{
		glColor3f(0.2, 0.2, 0.2);
		glBegin(GL_LINES);

		glVertex2f(-windowWidth / 2, i);
		glVertex2f(windowWidth / 2, i);

		glEnd();
	}
}


void CursorMove(POINT p)
{
	p.x = p.x -windowWidth/ 2.0;
	p.y = windowHeight / 2.0 - p.y;

	glColor3f(0.5, 0., 0);
	glPointSize(10.0);
	glBegin(GL_POINTS);
	glVertex2f(p.x, p.y);
	glEnd();
}

void SetupPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			16,
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0 };

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}
