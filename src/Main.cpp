
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

#include "Font.h"

#include "Forms.h"

#include "Test.h"

struct SaveArgs {
	Level* Level;
	Form* Form;
	float* TextTime;
};

void SaveLevel(void* A) {
	SaveArgs* Args = (SaveArgs*)A;

	Args->Level->Save();
	Args->Form->Enabled = false;
	*Args->TextTime = 200;

}

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR, int) {
	
	Font Arial;
	Arial.Load("assets/Arial.aaf", 0, 1);

	Font ArialBlack;
	ArialBlack.Load("assets/Arial.aaf", 2, 3);

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
	AssetFile LevelAsset("assets/popcorn.aaf");
	level.LoadFromAsset(LevelAsset.GetAsset(0));

	std::vector<IVec2> ChunksToDraw = { { 0, 0 }, {0, 1}, {1, 0}, {1, 1} };

	level.SpawnEntity(&Player, 512 * 32, 512);

#ifdef _DEBUG
	Test();
#endif

	std::string LevelName;
	std::string LevelAuthor;

	float FontTimer = 0;

	u16 SelectedSprite = 0;
	u8 Collision = 0;
	int NumSprites = level.Sprites.size();

	bool EditMode = true;
	bool Paused = true;
	bool CameraPan = false;
	bool DrawGeometry = false;
	bool LevelMetaBox = true;

	bool MetaBoxLevelNameSelected = true;
	bool MetaBoxLevelAuthorSelected = false;


	bool EisDown;
	bool QisDown;
	bool SisDown;
	bool TabisDown;
	bool F2isDown;
	bool PisDown;
	bool CisDown;

	Form TestForm(800, 400, &Renderer);

	Label FormTitle(10, 10, "Save Level");
	FormTitle.SetFont(&Arial);
	TestForm.Controls.push_back(&FormTitle);

	Label NameLabel(10, 50, "Level Name:");
	NameLabel.SetFont(&Arial);
	TestForm.Controls.push_back(&NameLabel);

	Label AuthorLabel(10, 50 + 32 + 10, "Level Author:");
	AuthorLabel.SetFont(&Arial);
	TestForm.Controls.push_back(&AuthorLabel);
	
	Label FilenameLabel(10, 50 + 64 + 20, "Filename:");
	FilenameLabel.SetFont(&Arial);
	TestForm.Controls.push_back(&FilenameLabel);

	TextBox NameTextBox(20 + NameLabel.Pos.W, 50, 200, 36, &level.Name);
	NameTextBox.SetFont(&ArialBlack);
	TestForm.Controls.push_back(&NameTextBox);

	TextBox TestTextBox(20 + AuthorLabel.Pos.W, 50 + 32 + 10, 200, 36, &level.Author);
	TestTextBox.SetFont(&ArialBlack);
	TestForm.Controls.push_back(&TestTextBox);

	TextBox FilenameBox(20 + FilenameLabel.Pos.W, FilenameLabel.Pos.Y, 200, 36, &level.Filename);
	FilenameBox.SetFont(&ArialBlack);
	TestForm.Controls.push_back(&FilenameBox);

	std::string SaveText = "Save";
	Button Save(300, 300, &ArialBlack, &SaveText, &SaveLevel);
	SaveArgs A = { &level, &TestForm, &FontTimer };
	Save.Arg = (void*)&A;
	TestForm.Controls.push_back(&Save);

	bool GameRunning = true;
	while (GameRunning) {

		TestForm.Update(Renderer.Window);

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

		if (FontTimer > 0) {
			FontTimer -= DeltaTime;
		}

		if (Paused) DeltaTime = 0;

		InputManager::Get().Update();
		ControllerState Controller = InputManager::Get().GetControllerState();

		if (Controller.Buttons & 0x10) {
			GameRunning = false;
		}

		IVec2 CameraPos = Renderer.GetCameraPosition();

		ChunksToDraw.clear();

		int camerafirstchunkx = CameraPos.X / 512;
		int camerafirstchunky = CameraPos.Y / 512;
	
		for (int i = 0; i < Renderer.Config.RenderResX / 512 + 2; i++) {
			for (int j = 0; j < Renderer.Config.RenderResY / 512 + 2; j++) {
				ChunksToDraw.push_back({ MAX(0, camerafirstchunkx + i),  MAX(0, camerafirstchunky + j) });
			}
		}

		level.Update(DeltaTime, ChunksToDraw);

		for (auto C : ChunksToDraw) {
			level.DrawChunk(&Renderer, C.X, C.Y);
		}

		auto E = level.Entities.First;
		while (E != nullptr) {
			E->Item->Draw(&Renderer);
			E = E->Next;
		}


		if (InputManager::Get().GetKeyState('P')) {
			if (!PisDown) {
				PisDown = true;
				Paused = !Paused;
			}
		}
		else {
			PisDown = false;
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


		if (FontTimer > 0) {
			Arial.RenderString(&Renderer, 10, 10, "Saving Level...");
		}

		if (EditMode) {

			if (InputManager::Get().GetKeyState(VK_LEFT)) {
				CameraPan = true;
				Renderer.SetCameraPosition(CameraPos + IVec2{ -10, 0 });
			}

			if (InputManager::Get().GetKeyState(VK_RIGHT)) {
				CameraPan = true;
				Renderer.SetCameraPosition(CameraPos + IVec2{ 10, 0 });
			}

			if (InputManager::Get().GetKeyState(VK_UP)) {
				CameraPan = true;
				Renderer.SetCameraPosition(CameraPos + IVec2{ 0, -10 });
			}

			if (InputManager::Get().GetKeyState(VK_DOWN)) {
				CameraPan = true;
				Renderer.SetCameraPosition(CameraPos + IVec2{ 0, 10 });
			}




			if (MS.Btn1) {
				int X = (MS.x + CameraPos.X) / 32;
				int Y = (MS.y + CameraPos.Y) / 32;

				if (!(X < 0 || Y < 0)) {

					int ChunkX = X / 16;
					int ChunkY = Y / 16;

					int LocalX = X % 16;
					int LocalY = Y % 16;

					Chunk* C = level.GetChunk(ChunkX, ChunkY);

					if (C != nullptr) {
						C->Grid[16 * LocalY + LocalX] = { SelectedSprite, Collision };
						
						if (!C->inIndex) {
							C->inIndex = true;
							level.ExistingChunks.push_back({ C->X, C->Y });
						}

						level.SetChunkGeometry(C);
					}
				}
			}

			if (MS.Btn2) {
				int X = (MS.x + CameraPos.X) / 32;
				int Y = (MS.y + CameraPos.Y) / 32;

				if (!(X < 0 || Y < 0)) {

					int ChunkX = X / 16;
					int ChunkY = Y / 16;

					int LocalX = X % 16;
					int LocalY = Y % 16;

					Chunk* C = level.GetChunk(ChunkX, ChunkY);

					if (C != nullptr) {
						C->Grid[16 * LocalY + LocalX] = { 0, 0 };
						level.SetChunkGeometry(C);
					}
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

			if (InputManager::Get().GetKeyState('C')) {
				if (!CisDown) {
					CisDown = true;

					DrawGeometry = !DrawGeometry;
				}
			}
			else {
				CisDown = false;
			}

			if (InputManager::Get().GetKeyState('S')) {
				if (!SisDown) {
					SisDown = true;


					if (InputManager::Get().GetKeyState(VK_CONTROL)) {
						TestForm.Enabled = true;
					}
				}
			}
			else {
				SisDown = false;
			}

			if (DrawGeometry) {
				for (auto C : ChunksToDraw) {
					level.DrawChunkCollisionGeometry(&Renderer, C.X, C.Y);
				}
			}

			Renderer.DrawRectangle(Renderer.Config.RenderResX - 66, Renderer.Config.RenderResY - 66, 36, 36, BackgroundColor);

			for (int i = 0; i < Renderer.Config.RenderResX / 32; i++) {
				Renderer.DrawRectangleBlend((i * 32) - Mod(CameraPos.X, 32) - 1, 0, (CameraPos.X + 32 * i - Mod(CameraPos.X, 32)) % 512 == 0 ? 3 : 1, Renderer.Config.RenderResY, 0x33333333);
				//Renderer.DrawRectangle((i * 32) - Mod(CameraPos.X, 32) - 1, 0, (CameraPos.X + 32 * i - Mod(CameraPos.X, 32)) % 512 == 0 ? 3 : 1, Renderer.Config.RenderResY, 0x8780D5);
			}

			for (int i = 0; i < Renderer.Config.RenderResY / 32; i++) {
				Renderer.DrawRectangleBlend(0, (i * 32) - Mod(CameraPos.Y, 32) - 1, Renderer.Config.RenderResX, (CameraPos.Y + 32 * i - Mod(CameraPos.Y, 32)) % 512 == 0 ? 3 : 1, 0x33333333);
				//Renderer.DrawRectangle(0, (i * 32) - Mod(CameraPos.Y, 32) - 1, Renderer.Config.RenderResX, (CameraPos.Y + 32 * i - Mod(CameraPos.Y, 32)) % 512 == 0 ? 3 : 1, 0x8780D5);

			}

			if (SelectedSprite != 0) {
				Renderer.DrawSpriteSS(&level.Sprites[SelectedSprite], Renderer.Config.RenderResX - 64, Renderer.Config.RenderResY - 64);
			}

			TestForm.Render(&Renderer);

			Renderer.DrawRectangle(MS.x - 17, MS.y - 2, 34, 4, 0x000000);
			Renderer.DrawRectangle(MS.x - 2, MS.y - 17, 4, 34, 0x000000);

			Renderer.DrawRectangle(MS.x - 16, MS.y - 1, 32, 2, 0xffffff);
			Renderer.DrawRectangle(MS.x - 1, MS.y - 16, 2, 32, 0xffffff);
		}

		if (!CameraPan) {
			if (Player.Position.X < Renderer.GetCameraPosition().X + 400) {
				Renderer.SetCameraPosition(Player.Position.X - 400, Renderer.GetCameraPosition().Y);
			}

			if (Player.Position.X > (Renderer.GetCameraPosition().X + Renderer.Config.RenderResX) - 400) {
				Renderer.SetCameraPosition(Player.Position.X + 400 - Renderer.Config.RenderResX, Renderer.GetCameraPosition().Y);
			}

			if (Player.Position.Y < Renderer.GetCameraPosition().Y + 300) {
				Renderer.SetCameraPosition(Renderer.GetCameraPosition().X, Player.Position.Y - 300);
			}

			if (Player.Position.Y > (Renderer.GetCameraPosition().Y + Renderer.Config.RenderResY) - 300) {
				Renderer.SetCameraPosition(Renderer.GetCameraPosition().X, Player.Position.Y + 300 - Renderer.Config.RenderResY);
			}
		}



		if(!PlatformLayer.Update(DeltaTime)) GameRunning = false;
	}

	return 0;
}