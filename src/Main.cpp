
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

	GameObject Player({ 20, 20 });
	Player.loadTexture("Assets/Mario.png");
	ResizeSprite((Win32Sprite*)Player.Sprite, 48);

	GameObjects.push_back(Player);

	GameObject AI({ 600 - 20, 20 }, { -10, 5 });
	AI.loadTexture("Assets/Mario.png");
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

	Win32Sprite Sprite;
	AssetManager::AssetFile BallAsset("Assets/ball.aaf");
	Sprite.Load(BallAsset, 0);

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

		if (InputMgr.GetKeyState('D')) 
			GameObjects[0].Velocity.X += (5000.f * DeltaTime);

		if (InputMgr.GetKeyState('A'))
			GameObjects[0].Velocity.X -= (5000.f * DeltaTime);

	
		if (InputMgr.GetKeyState(VK_SPACE)) {
			if (JumpTime == 0) {
				if (GameObjects[0].isGrounded) {
					GameObjects[0].isGrounded = false;

					JumpTime += DeltaTime;
					if (JumpTime < 0.5f){
						GameObjects[0].Velocity.Y -= 750 - JumpTime * DeltaTime;
					}
					else {

					}
				}
			}
		}
		else {
			JumpTime = 0;
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