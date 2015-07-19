#pragma once

#include "Renderer.h"

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
