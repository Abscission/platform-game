
#include <stdlib.h>

#include <vector>
#include <string>

#include <Windows.h>

#include "Utility.h"
#include "Maths.h"
#include "GameLayer.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Player.h"
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

	//Temporary GameObject vector
	std::vector<GameObject*> GameObjects;
	AssetFile Mario("assets/assets.aaf");

	Player Player;
	Player.LoadSprite(Mario, 0);
	
	ResizeSprite(Player._Sprite, 48);

	GameObjects.push_back(&Player);

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

	if (InputManager::Get().IsControllerConnected()) {
		OutputDebugString("Controller Connected\n");
	}
	else {
		OutputDebugString("Controller Not Connected\n");
	}

	Level level;

	AssetFile Tiles("assets/tiles.aaf");
	level.Sprites[1] = new Sprite();
	level.Sprites[1]->Load(Tiles, 1);

	Chunk TestChunk = {};
	TestChunk.Grid[16 * 3 + 0] = { 1 };
	TestChunk.Grid[16 * 3 + 1] = { 1 };
	TestChunk.Grid[2] = { 1 };
	TestChunk.Grid[3] = { 1 };
	TestChunk.Grid[4] = { 1 };
	TestChunk.Grid[5] = { 1 };

	level.SetChunk(0, 0, TestChunk);

	std::vector <iRect> LevelGeometry = level.GenerateCollisionGeometryFromChunk(0, 0);

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
			sprintf_s(frametime, "%f fps\n", 1.f / DeltaTime);
			OutputDebugStringA(frametime);

			elapsedTime = 0;
			Frames = 0;
		}

		InputManager::Get().Update();
		ControllerState Controller = InputManager::Get().GetControllerState();

		if (Controller.Buttons & 0x10) {
			GameRunning = false;
		}

		//Update Level HERE
		level.DrawChunk(&Renderer, 0, 0);


		//TODO: See if iterator approach is fast enough
//#pragma loop(hint_parallel(8))
		for (int i = 0; i < GameObjects.size(); i++){
		//for (auto & Object : GameObjects){


			GameObject* Object = GameObjects[i];
			Object->Update(DeltaTime, ;
			Object->Draw(&Renderer);
		}

		if(!PlatformLayer.Update(DeltaTime)) GameRunning = false;
		Renderer.SetCameraPosition({ Player.Position.X - 512, Player.Position.Y - 300 });

	}

	return 0;
}