#include "Win32_RendererDirect2D.h"

#include <gl\GL.h>

#include <Windows.h>
#include <vector>

#include "PicoPNG.h"

#include "Win32_Utility.h"

bool ShouldClose = false;

//The window callback, this is what processes messages from our windows
LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_CLOSE:
		DestroyWindow(Window);
		ShouldClose = true;
		return 0;
	default:
		return DefWindowProc(Window, Message, wParam, lParam);
	}
}

bool RendererWin32Direct2D::OpenWindow(int Width, int Height, char* Title){
	WNDCLASSEX WindClass = {}; //Create a Window Class structure

	//Check if the class has been registered aleady	
	if (!GetClassInfoEx(Instance, L"JasonWindowClassName", &WindClass)) {
		//Create and fill a Window Class structure
		WindClass.hInstance = Instance;
		WindClass.lpfnWndProc = WindowProc;
		WindClass.cbSize = sizeof(WindClass);
		WindClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //Redraw the Window if it is moved or resized horizontally or vertically
		WindClass.lpszClassName = L"JasonWindowClassName"; //Unique identifying class name

		if (!RegisterClassEx(&WindClass)) {
			DWORD error = GetLastError();
			DisplayMessage(error);
			return false;
		}
	}

	this->Window = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, "JasonWindowClassName", Title, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, 0, 0, Instance, 0);

	return true;
}

bool RendererWin32Direct2D::Initialize() {
	Config.ResX = 800;
	Config.ResY = 600;
	Config.BPP = 4;

	Instance = GetModuleHandle(NULL);
	this->OpenWindow(Config.ResX, Config.ResY, "Title");

	this->DeviceContext = GetDC(this->Window);
	wglCreateContext(this->DeviceContext);

	return 0;
}

bool RendererWin32Direct2D::Refresh() {
	MSG Message;

	//Handle Win32 Messages first
	while (PeekMessage(&Message, this->Window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}


	return !ShouldClose;
}

void RendererWin32Direct2D::DrawRectangle(int X, int Y, int Width, int Height, char R, char G, char B) {
	//TODO: Implement
}

void RendererWin32Direct2D::DrawSprite(Sprite* Spr, int X, int Y) {
	//TODO: Implement
	
}