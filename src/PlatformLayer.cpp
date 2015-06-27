#include "PlatformLayer.h"
#include "Win32_PlatformLayer.h"
#include "Renderer.h"
#include "RendererSoftware.h"
#include "Maths.h"

void Win32PlatformLayer::Initialize(){
	_Renderer = new RendererWin32Software;
	_Renderer->Initialize();
	this->Window = &((RendererWin32Software*)_Renderer)->Window;

}

bool Win32PlatformLayer::Update(float DeltaTime) {
	MSG Message;

	//Handle Win32 Messages first
	while (PeekMessage(&Message, *this->Window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	Rect Level[] = { { 0, 300, 220, 32 }, { 200, 500, 300, 10 }, { 490, 450, 250, 10 }, { 400, 0, 10, 500 }, {0, 600 - 25, 800, 16} };
	
	static Win32Sprite* Platform = new Win32Sprite();
	if (!Platform->Data) {
		Platform->Load("Assets/tile01.png");
		ResizeSprite(Platform, 16, 16);
	}

	for (auto Surface : Level) {
		_Renderer->DrawSpriteRectangle(Surface.X, Surface.Y, Surface.W, Surface.H, Platform);
	}

 	return _Renderer->Refresh();
}