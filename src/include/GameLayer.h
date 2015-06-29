#pragma once

#include "RendererSoftware.h"

class GameLayer  {
private:
	HWND Window;
	Renderer _Renderer;
public:
	void Initialize();
	bool Update(float DeltaTime);

	HWND GetWindow() {
		return Window;
	}

	Renderer GetRenderer() {
		return _Renderer;
	}
};