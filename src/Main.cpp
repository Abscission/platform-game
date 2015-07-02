
#include <stdlib.h>

#include <vector>
#include <string>

#include <Windows.h>

#include "Utility.h"
#include "Maths.h"
#include "GameLayer.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Entity.h"
#include "AssetManager.h"
#include "InputManager.h"
#include "Config.h"
#include "Level.h"

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

	AssetFile Mario("assets/assets.aaf");

	GameObject Player({40, 40 });
	Player.LoadSprite(Mario, 0);
	
	ResizeSprite(Player._Sprite, 48);

	GameObjects.push_back(Player);

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

	Level level;

	Chunk C = level.GetChunk(2, 1);
	C = level.GetChunk(1, 1);

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

		Rect MoveRect = { 10, 10, 1000, 1000 };

		GameObject& Player = GameObjects[0];
		
		IVec2 PlayerScreenPos = Renderer.TransformPosition({ Player.Position.X, Player.Position.Y });

		if (PlayerScreenPos.X < MoveRect.X){
			IVec2 CameraPos = Renderer.GetCameraPosition();
			//Renderer.SetCameraPosition(CameraPos.X + PlayerScreenPos.X, CameraPos.Y);
		}

		if (Controller.Buttons & 0x10) {
			GameRunning = false;
		}

		if (GameObjects[0].Position.Y > 2000) {
			GameObjects[0].Position = { 50, -100 };
			GameObjects[0].Velocity = { 0, 0 };

		}

		//Update Level HERE


		//TODO: See if iterator approach is fast enough
		for (auto & Object : GameObjects){
			Object.Update(DeltaTime);
			Object.Draw(&Renderer);
		}

		
		if(!PlatformLayer.Update(DeltaTime)) GameRunning = false;

	}

	return 0;
}