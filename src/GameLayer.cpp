
//Copyright (c) 2015 Jason Light
//License: MIT

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
	_Sprite& Spr;
};

bool GameLayer::Update(double DeltaTime) {
	MSG Message;

	//Handle Win32 Messages first
	while (PeekMessage(&Message, this->Window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}


 	return _Renderer.Refresh();
}

GameGlobals G;