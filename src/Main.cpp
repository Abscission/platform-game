
#include <stdlib.h>

#include <vector>
#include <string>

#include <Windows.h>

#include "Types.h"
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

	for (int i = 0; i < 4; i++) level.Sprites[i] = new Sprite();

	level.Sprites[0] = 0;
	level.Sprites[1]->Load(Tiles, 0);
	level.Sprites[2]->Load(Tiles, 1);
	level.Sprites[3]->Load(Tiles, 3);

	Chunk TestChunk = {};
	TestChunk.X = 0;
	TestChunk.Y = 0;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (x == 0 || x == 7 || y == 0) {
				if (y == 5 || y == 6) {
					//doorway
					TestChunk.Grid[(2 + y) * 16 + x + 6] = { 3, false };
				}
				else {
					TestChunk.Grid[(2 + y) * 16 + x + 6] = { 1, true };
				}
			}
			else if (y == 7) {
				TestChunk.Grid[(2 + y) * 16 + x + 6] = { 2, true };
			}
			else {
				TestChunk.Grid[(2 + y) * 16 + x + 6] = { 3, false };
			}
		}
	}

	TestChunk.Grid[16 * 3 + 0] = { 1, true };
	TestChunk.Grid[16 * 3 + 1] = { 1, true };
	for (int i = 2; i < 12; i++) TestChunk.Grid[i] = { 2, true };

	TestChunk.Entities[0] = new GameObject();
	TestChunk.Entities[0]->LoadSprite(Mario, 0);

	level.SetChunk(0, 0, TestChunk);
	level.SetChunk(0, 1, TestChunk);
	level.SetChunk(1, 1, TestChunk);
	level.SetChunk(2, 1, TestChunk);
	level.SetChunk(3, 1, TestChunk);
	level.SetChunk(4, 2, TestChunk);

	std::vector <iRect> LevelGeometry = level.GenerateCollisionGeometryFromChunk(0, 0);
	std::vector <iRect> LevelGeometry2 = level.GenerateCollisionGeometryFromChunk(0, 1);

	LevelGeometry.insert(LevelGeometry.end(), LevelGeometry2.begin(), LevelGeometry2.end());


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
		DeltaTime = static_cast<double>(_DeltaTime.QuadPart);
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

		static IVec2 ChunksToDraw[6] = { {0, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 2} };

#pragma loop(hint_parallel(6))
		for (int i = 0; i < 6; i++) {
		//for (auto Chunk : ChunksToDraw) {
			level.DrawChunk(&Renderer, ChunksToDraw[i].X, ChunksToDraw[i].Y);
		}

		//TODO: See if iterator approach is fast enough
//#pragma loop(hint_parallel(8))
		for (int i = 0; i < GameObjects.size(); i++){
		//for (auto & Object : GameObjects){


			GameObject* Object = GameObjects[i];
			Object->Update(DeltaTime, LevelGeometry);
			Object->Draw(&Renderer);
		}

		if(!PlatformLayer.Update(DeltaTime)) GameRunning = false;
		Renderer.SetCameraPosition({ (int)Player.Position.X - 512, (int)Player.Position.Y - 300 });
	}

	return 0;
}