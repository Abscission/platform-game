#pragma once

#include "PlatformLayer.h"
#include "Renderer.h"
#include "RendererSoftware.h"

typedef HWND Window;

class Win32PlatformLayer : public PlatformLayer {
private:
	Window* Window;
	Renderer* _Renderer;
	RAWINPUTDEVICE Rid;

public:
	void Initialize();
	bool Update(float DeltaTime);

	Renderer* GetRenderer() {
		return _Renderer;
	}
};
