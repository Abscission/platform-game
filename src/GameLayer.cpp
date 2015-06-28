#include "GameLayer.h"
#include "RendererSoftware.h"
#include "Maths.h"

void GameLayer::Initialize(){
	_Renderer.Initialize();
	this->Window = _Renderer.Window;
}

bool GameLayer::Update(float DeltaTime) {
	MSG Message;

	//Handle Win32 Messages first
	while (PeekMessage(&Message, this->Window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	Rect Level[] = { { 0, 300, 224, 64 }, { 320, 512, 320, 64 }, { 512, 0, 64, 512 }, { 0, 768 - 32, 1024, 64 } };
	static float CameraX = 0;
	//CameraX += 25 * DeltaTime;

	static Sprite Platform;
	if (!Platform.Data) {
		AssetManager::AssetFile Assets("assets/assets.aaf");
		Platform.Load(Assets, 1);
		
		ResizeSprite(&Platform, 16, 16);
	}

	for (auto Surface : Level) {
		_Renderer.DrawSpriteRectangle(Surface.X - CameraX, Surface.Y, Surface.W, Surface.H, &Platform);
	}

 	return _Renderer.Refresh();
}