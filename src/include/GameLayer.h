
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include "Renderer.h"
#include "Player.h"
#include "Level.h"

struct GameGlobals {
	Player* player;
	Renderer* renderer;
	Level* level;
	bool debuglos;
	bool editing;
};

extern GameGlobals G;

class GameLayer  {
private:
	HWND Window;
	Renderer _Renderer;
public:
	void Initialize();
	bool Update(double DeltaTime);

	HWND GetWindow() {
		return Window;
	}

	Renderer GetRenderer() {
		return _Renderer;
	}
};
