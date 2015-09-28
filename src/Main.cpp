
//Copyright (c) 2015 Jason Light
//License: MIT

#include <stdlib.h>

#include <vector>
#include <string>
#include <fstream>

#include <Windows.h>

#include "Types.h"
#include "Utility.h"
#include "Maths.h"
#include "GameLayer.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Player.h"
#include "AssetManager.h"
#include "InputManager.h"
#include "Config.h"
#include "Level.h"
#include "LogManager.h"
#include "List.h"

#include "Test.h"

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR, int) {
	//Create a platform layer
	GameLayer PlatformLayer;
	PlatformLayer.Initialize();

	//Get a renderer object from the game layer 
	Renderer Renderer;
	Renderer = PlatformLayer.GetRenderer();

	//Set up the global log
	//GlobalLog = Log("platform-game-log.txt");

	//Temporary GameObject vector
	std::vector<GameObject*> GameObjects;
	AssetFile Mario("assets/assets.aaf");

	Player Player;
	Player.LoadSprite(Mario, 0);
	
	ResizeSprite(Player.Spr, 48);

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

	// TEMPORARY ANIMATION TEST CODE

	double AnimX = 64;
	double AnimY = 64;

	int AnimDirection = 0;

	Sprite AnimStill;
	AnimStill.Load("assets/animation.aaf", 0);
	ResizeSprite(&AnimStill, 48);

	Sprite AnimStillBack;
	AnimStillBack.Load("assets/animation.aaf", 8);
	ResizeSprite(&AnimStillBack, 48);

	Sprite TestAnimation;
	TestAnimation.Load("assets/animation.aaf", 0, 8);
	TestAnimation.Period = 800;

	Sprite TestAnimationBack;
	TestAnimationBack.Load("assets/animation.aaf", 8, 8);
	TestAnimationBack.Period = 800;

	// END TEMPORARY SECTION

	Level level = {};
	AssetFile LevelAsset("assets/testlevel00.aaf");
	level.LoadFromAsset(LevelAsset.GetAsset(0));

	static IVec2 ChunksToDraw[3] = { { 0, 0 }, {0, 1}, {1, 0} };

	level.SpawnEntity(&Player, 0, 0);

	std::vector <iRect> LevelGeometry = {};

	for (auto Chunk : ChunksToDraw) {

		std::vector <iRect> NewGeometry = level.GenerateCollisionGeometryFromChunk(Chunk.X, Chunk.Y);
		LevelGeometry.insert(LevelGeometry.end(), NewGeometry.begin(), NewGeometry.end());

	}

#ifdef _DEBUG
	Test();
#endif

	u16 SelectedSprite = 0;
	u8 Collision = 0;
	int NumSprites = level.Sprites.size();

	bool EditMode = false;

	bool EisDown;
	bool QisDown;
	bool SisDown;
	bool TabisDown;
	bool F2isDown;

	bool GameRunning = true;
	while (GameRunning) {

		MouseState MS = InputManager::Get().GetMouseState(Renderer.Window);

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


		for (int i = 0; i < 3; i++) {
			level.UpdateChunk(ChunksToDraw[i].X, ChunksToDraw[i].Y, DeltaTime, LevelGeometry);
		}

		for (int i = 0; i < 3; i++) {
			level.DrawChunk(&Renderer, ChunksToDraw[i].X, ChunksToDraw[i].Y);
		}

		for (int i = 0; i < 3; i++) {
			level.DrawChunkEntities(&Renderer, ChunksToDraw[i].X, ChunksToDraw[i].Y);
		}


		if (InputManager::Get().GetKeyState(VK_F2)) {
			if (!F2isDown) {
				F2isDown = true;

				EditMode = !EditMode;
			}
		}
		else {
			F2isDown = false;
		}


		if (EditMode) {

			if (MS.Btn1) {
				int X = MS.x / 32;
				int Y = MS.y / 32;

				int ChunkX = X / 16;
				int ChunkY = Y / 16;

				int LocalX = X % 16;
				int LocalY = Y % 16 + 1;

				Chunk* C = level.GetChunk(ChunkX, ChunkY);

				C->Grid[16 * LocalY + LocalX] = { SelectedSprite, Collision };

				LevelGeometry = {};

				for (auto Chunk : ChunksToDraw) {

					std::vector <iRect> NewGeometry = level.GenerateCollisionGeometryFromChunk(Chunk.X, Chunk.Y);
					LevelGeometry.insert(LevelGeometry.end(), NewGeometry.begin(), NewGeometry.end());

				}
			}

			if (MS.Btn2) {
				int X = MS.x / 32;
				int Y = MS.y / 32;

				int ChunkX = X / 16;
				int ChunkY = Y / 16;

				int LocalX = X % 16;
				int LocalY = Y % 16 + 1;

				Chunk* C = level.GetChunk(ChunkX, ChunkY);

				C->Grid[16 * LocalY + LocalX] = { 0, 0 };

				LevelGeometry = {};

				for (auto Chunk : ChunksToDraw) {

					std::vector <iRect> NewGeometry = level.GenerateCollisionGeometryFromChunk(Chunk.X, Chunk.Y);
					LevelGeometry.insert(LevelGeometry.end(), NewGeometry.begin(), NewGeometry.end());

				}
			}

			u32 BackgroundColor = Collision ? 0x00ff0000 : 0x0000ff00;

			if (InputManager::Get().GetKeyState('Q')) {
				if (!QisDown) {
					QisDown = true;
					if (SelectedSprite > 0) SelectedSprite--;
				}
			}
			else {
				QisDown = false;
			}

			if (InputManager::Get().GetKeyState('E')) {
				if (!EisDown) {
					EisDown = true;
					if (SelectedSprite < NumSprites - 1) SelectedSprite++;
				}
			}
			else {
				EisDown = false;
			}

			if (InputManager::Get().GetKeyState(VK_TAB)) {
				if (!TabisDown) {
					TabisDown = true;

					Collision = !Collision;
				}
			}
			else {
				TabisDown = false;
			}

			if (InputManager::Get().GetKeyState('S')) {
				if (!SisDown) {
					SisDown = true;

					if (InputManager::Get().GetKeyState(VK_CONTROL)) {
						OutputDebugStringA("Saving Level...");



						std::ofstream Output("level.bin", std::ofstream::binary);
						for (int i = 0; i < 256; i++)
							Output.write((char*)&level.GetChunk(0, 0)->Grid[i], 3);

					}
				}
			}
			else {
				SisDown = false;
			}

			Renderer.DrawRectangle(Renderer.Config.RenderResX - 66, Renderer.Config.RenderResY - 66, 36, 36, BackgroundColor);

			if (SelectedSprite != 0) {
				Renderer.DrawSprite(&level.Sprites[SelectedSprite], Renderer.Config.RenderResX - 64, Renderer.Config.RenderResY - 64);
			}
		}

		if(!PlatformLayer.Update(DeltaTime)) GameRunning = false;
	}

	return 0;
}