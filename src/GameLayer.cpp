
#include <Windows.h>

#include "GameLayer.h"
#include "Renderer.h"
#include "Maths.h"

void GameLayer::Initialize(){
	_Renderer.Initialize();
	this->Window = _Renderer.Window;
}

struct Surface {
	Rect Position;
	Sprite& Spr;
};

bool GameLayer::Update(float DeltaTime) {
	MSG Message;

	//Handle Win32 Messages first
	while (PeekMessage(&Message, this->Window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	static Sprite Platform01;
	if (!Platform01.Data) {
		AssetFile Tiles("assets/tiles.aaf");
		Platform01.Load(Tiles, 0);
		
		ResizeSprite(&Platform01, 32, 32);
	}

	static Sprite Platform02;
	if (!Platform02.Data) {
		AssetFile Tiles("assets/tiles.aaf");
		Platform02.Load(Tiles, 2);

		ResizeSprite(&Platform02, 32, 32);
	}

	static Sprite Platform03;
	if (!Platform03.Data) {
		AssetFile Tiles("assets/tiles.aaf");
		Platform03.Load(Tiles, 1);

		ResizeSprite(&Platform03, 32, 32);
	}


	static Sprite Platform04;
	if (!Platform04.Data) {
		AssetFile Tiles("assets/tiles.aaf");
		Platform04.Load(Tiles, 3);

		ResizeSprite(&Platform04, 32, 32);
	}


	Surface Level[] = { { { 0, 300, 224, 64 }, Platform01 }, { { 320, 512, 320, 64 }, Platform01 }, { { 512, 0, 64, 512 }, Platform01 }, { { 0, 768 - 32, 1024, 32 }, Platform02 }, { { 0, 768, 1024, 32 }, Platform03 }
};

	for (auto Surface : Level) {
		//_Renderer.DrawSpriteRectangle(Surface.Position.X, Surface.Position.Y, Surface.Position.W, Surface.Position.H, &Surface.Spr);
	}

 	return _Renderer.Refresh();
}