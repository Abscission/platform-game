
#include <stdlib.h> 
#include <vector>
#include <Windows.h>

#include "Utility.h"

#include "Maths.h"

#include "PlatformLayer.h"
#include "Win32_PlatformLayer.h"
#include "RendererSoftware.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Entity.h"
#include "AssetManager.h"
#include "InputManager.h"

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR, int) {
	PlatformLayer* PlatformLayer;
	PlatformLayer = new Win32PlatformLayer();
	PlatformLayer->Initialize();

	//Get a renderer object from the platform layer 
	Renderer* renderer;
	renderer = PlatformLayer->GetRenderer();

	InputManager InputMgr;

	std::vector<GameObject> GameObjects;

	GameObject Player({40, 40 });
	Player.loadTexture("assets/Mario.png");
	ResizeSprite((Win32Sprite*)Player.Sprite, 48);

	GameObjects.push_back(Player);

	GameObject AI({ 600 - 20, 20 }, { -10, 5 });
	AI.loadTexture("assets/Mario.png");
	GameObjects.push_back(AI);

	float counter = 1.f;
	LARGE_INTEGER Time;
	LARGE_INTEGER LastTime;
	LARGE_INTEGER _DeltaTime;
	LARGE_INTEGER PerfFreq;

	QueryPerformanceFrequency(&PerfFreq);
	QueryPerformanceCounter(&Time);

	double elapsedTime = 0;
	double DeltaTime = 0;

	int Frames = 0;
	double JumpTime = 0;

	if (InputMgr.IsControllerConnected()) {
		OutputDebugString("Controller Connected\n");
	}
	else {
		OutputDebugString("Controller Not Connected\n");
	}

	bool GameRunning = true;
	while (GameRunning) {
		//High res timer stuff, will be abstracted later.
		//Game should have access to DeltaTime only.
		LastTime = Time;
		QueryPerformanceCounter(&Time);
		_DeltaTime.QuadPart = Time.QuadPart - LastTime.QuadPart;

		_DeltaTime.QuadPart *= 1000000;
		_DeltaTime.QuadPart /= PerfFreq.QuadPart;

		//DeltaTime is the time since the last frame in seconds
		DeltaTime = _DeltaTime.QuadPart;
		DeltaTime *= 1.0e-6;

		Frames++;
		if ((elapsedTime += DeltaTime) > 1) {
			//Ghetto Framerate Counter
			char frametime[30];
			sprintf_s(frametime, "%f ms/f \n", DeltaTime * 1000);
			OutputDebugStringA(frametime);

			elapsedTime = 0;
			Frames = 0;
		}

		InputMgr.Update();

		ControllerState Controller = InputMgr.GetControllerState();


		int MaxSpeed = 500;

		if (InputMgr.GetKeyState('D') || Controller.Buttons & 0x8) {
			GameObjects[0].TargetVelocity.X = MaxSpeed;
		}
		else if (InputMgr.GetKeyState('A') || Controller.Buttons & 0x4) {
			GameObjects[0].TargetVelocity.X = -MaxSpeed;
		}
		else {
			GameObjects[0].TargetVelocity.X = (MaxSpeed / 32767.f) * Controller.LeftStick.X;

//			GameObjects[0].TargetVelocity.X = 0;
		}

		if (InputMgr.GetKeyState(VK_SPACE) || Controller.Buttons & 0x1000) {
			if (GameObjects[0].isGrounded) {
				GameObjects[0].Velocity.Y = -750;
			}
		}

		//GameObjects[0].Velocity.Y += JumpTime;

		if (Controller.Buttons & 0x10) {
			GameRunning = false;
		}



		//TODO: See if iterator approach is fast enough
		for (auto & Object : GameObjects){
			Object.Update(DeltaTime);
			Object.Draw(renderer);
		}

		
		if(!PlatformLayer->Update(DeltaTime)) GameRunning = false;
	
		
	}

	return 0;
}