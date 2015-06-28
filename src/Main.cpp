
#include <stdlib.h> 
#include <vector>
#include <Windows.h>

#include "Utility.h"

#include "Maths.h"

#include "GameLayer.h"
#include "RendererSoftware.h"
#include "GameObject.h"
#include "Entity.h"
#include "AssetManager.h"
#include "InputManager.h"

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR, int) {
	//Create a platform layer
	GameLayer PlatformLayer;
	PlatformLayer.Initialize();

	//Get a renderer object from the game layer 
	Renderer Renderer;
	Renderer = PlatformLayer.GetRenderer();

	//Get an Input Manager
	InputManager InputMgr;

	//Temporary GameObject vector
	std::vector<GameObject> GameObjects;

	GameObject Player({40, 40 });
	Player.loadTexture("assets/Mario.png");
	ResizeSprite(Player._Sprite, 48);

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


		int MaxSpeed = (InputMgr.GetKeyState(VK_SHIFT) || Controller.Buttons & 0x2000) ? 400 : 600;

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

		if (JumpTime == 0) {
			if (InputMgr.GetKeyState(VK_SPACE) || Controller.Buttons & 0x1000) {
				if (GameObjects[0].canJump) {
					if (!GameObjects[0].isGrounded) {
						//walljump
						GameObjects[0].Velocity.X = 600 * GameObjects[0].wallJumpDirection;
					}

					GameObjects[0].Velocity.Y = -600;
					JumpTime += DeltaTime;
				}
			}
		}

		if (JumpTime > 0){
			if (InputMgr.GetKeyState(VK_SPACE) || Controller.Buttons & 0x1000) {
				JumpTime += DeltaTime;
				if (JumpTime < 1.0f) {
					GameObjects[0].Velocity.Y -= 500 * DeltaTime;
				}
			}
			else {
				JumpTime = 0;
			}
		} 

		//GameObjects[0].Velocity.Y += JumpTime;

		if (Controller.Buttons & 0x10) {
			GameRunning = false;
		}

		if (GameObjects[0].Position.Y > 2000) {
			GameObjects[0].Position = { 50, -100 };
			GameObjects[0].Velocity = { 0, 0 };

		}

		//TODO: See if iterator approach is fast enough
		for (auto & Object : GameObjects){
			Object.Update(DeltaTime);
			Object.Draw(&Renderer);
		}

		
		if(!PlatformLayer.Update(DeltaTime)) GameRunning = false;
	
		
	}

	return 0;
}