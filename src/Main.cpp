
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
#include "Enemy.h"
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

	//Temporary GameObject vector
	AssetFile Mario("assets/assets.aaf");

	Player Player;
	Player.LoadSprite(Mario, 0);
	
	ResizeSprite(Player.Spr, 48);

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

	Level level = {};
	AssetFile LevelAsset("assets/popcorn.aaf");
	level.LoadFromAsset(LevelAsset.GetAsset(0));

	std::vector<IVec2> ChunksToDraw;

	level.SpawnEntity(&Player, 512 * 32, 512);

#ifdef _DEBUG
	Test();
#endif

	float FontTimer = 0;

	u16 SelectedSprite = 0;
	u8 Collision = 0;
	int NumSprites = level.Sprites.size();

	bool EditMode = false;
	bool Paused = false;
	bool CameraPan = false;
	bool DrawGeometry = false;

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

	Form TextureBrowser(-410, 200, 400, 600, &Renderer);
	TextureBrowser.Color = rgba(0, 0, 0, 225);
	TextureBrowser.Enabled = false;

	G.level = &level;
	G.player = &Player;
	G.renderer = &Renderer;

	Enemy E;
	E.State = AI_ALERT;
	E.LoadSprite("assets/assets.aaf", 0);
	ResizeSprite(E.Spr, 24);
	level.SpawnEntity(&E, 500 * 32, 512);

	Pickup* P[10];
	for (int i = 0; i < 10; i++) {
		P[i] = new Pickup();
		level.SpawnEntity(P[i], 512 * 32 + 64 * i, 600);
	}

	float Timer = 0;

	bool GameRunning = true;
	while (GameRunning) {
		bool Crosshair = false;

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

		//Things that use DeltaTime, but shouldn't pause go here

		if (FontTimer > 0) {
			FontTimer -= DeltaTime;
		}

		//Zero out the deltatime for everything else
		if (Paused || Player.Dead) DeltaTime = 0;

		Timer += DeltaTime;

		//Update the Input Manager
		InputManager::Get().Update();
		ControllerState Controller = InputManager::Get().GetControllerState();

		//Kill the game if start is pressed
		if (Controller.Buttons & 0x10) {
			GameRunning = false;
		}

		//Get the camera position for calculations
		IVec2 CameraPos = Renderer.GetCameraPosition();

		//If we are in edit mode
		if (G.editing) {
			//Draw the grid
			for (int i = 0; i < Renderer.Config.RenderResX / 32 + 2; i++) {
				Renderer.DrawRectangle((i * 32) - Mod(CameraPos.X, 32) - 1, 0, (CameraPos.X + 32 * i - Mod(CameraPos.X, 32)) % 512 == 0 ? 3 : 1, Renderer.Config.RenderResY, 0xaaaaff);
			}

			for (int i = 0; i < Renderer.Config.RenderResY / 32 + 2; i++) {
				Renderer.DrawRectangle(0, (i * 32) - Mod(CameraPos.Y, 32) - 1, Renderer.Config.RenderResX, (CameraPos.Y + 32 * i - Mod(CameraPos.Y, 32)) % 512 == 0 ? 3 : 1, 0xaaaaff);
			}
		}

		//Calculate which chunks we need to draw
		ChunksToDraw.clear();

		int camerafirstchunkx = CameraPos.X / 512;
		int camerafirstchunky = CameraPos.Y / 512;
	
		for (int i = 0; i < Renderer.Config.RenderResX / 512 + 2; i++) {
			for (int j = 0; j < Renderer.Config.RenderResY / 512 + 2; j++) {
				ChunksToDraw.push_back({ MAX(0, camerafirstchunkx + i),  MAX(0, camerafirstchunky + j) });
			}
		}

		//Update the level (including all entitites)
		level.Update(DeltaTime);

		//And draw all the chunks
		for (auto C : ChunksToDraw) {
			level.DrawChunk(&Renderer, C.X, C.Y);
		}

		//Draw the entities too
		for (auto E : level.Entities) {
			E->Draw(&Renderer);
		}

		//Some toggles
		if (InputManager::Get().GetKeyDown('P'))Paused = !Paused;
		if (InputManager::Get().GetKeyDown(VK_F2)) G.editing = !G.editing;

		if (FontTimer > 0) {
			Arial.RenderString(&Renderer, 10, 10, "Saving Level...");
		}

		//If in edit mode
		if (G.editing) {
			//Allow the camera to be panned
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


			//Check the mouse button
			if (MS.Btn1) {
				//Find the grid coordinates of the mouse
				int X = (MS.x + CameraPos.X) / 32;
				int Y = (MS.y + CameraPos.Y) / 32;

				//If they are in bounds
				if (!(X < 0 || Y < 0 || X > UINT16_MAX || Y > UINT16_MAX)) {

					//Get the coordinate of the chunk
					int ChunkX = X / 16;
					int ChunkY = Y / 16;

					//And the coordinate within the chunk
					int LocalX = X % 16;
					int LocalY = Y % 16;

					//Get a pointer to the chunk from the level
					Chunk* C = level.GetChunk(ChunkX, ChunkY);

					//If it succeeded
					if (C != nullptr) {
						//Set the block at the grid location to the currently selected sprite and collision values
						C->Grid[16 * LocalY + LocalX] = { SelectedSprite, Collision };
						
						//If the chunk isn't listed as existing, list it now. This is for level saving purposes.
						if (!C->inIndex) {
							C->inIndex = true;
							level.ExistingChunks.push_back({ C->X, C->Y });
						}

						//Update the geometry
						level.SetChunkGeometry(C);
					}
				}
			}

			//Same as mouse1, but delete the block
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
			
			//More toggles
			if (InputManager::Get().GetKeyDown('Q')) if (SelectedSprite > 0) SelectedSprite--;
			if (InputManager::Get().GetKeyDown('E')) if (SelectedSprite < NumSprites - 1) SelectedSprite++;
			if (InputManager::Get().GetKeyDown('Z')) G.debuglos = !G.debuglos;
			if (InputManager::Get().GetKeyDown(VK_TAB)) Collision = Collision == 0 ? COLLIDE_ALL : 0;
			if (InputManager::Get().GetKeyDown('C')) DrawGeometry = !DrawGeometry;
			if (InputManager::Get().GetKeyDown('S')) {
				if (InputManager::Get().GetKeyState(VK_CONTROL)) {
					TestForm.Enabled = true;
				}
			}
			

			//Draw the chunk geometry if requestsed, by looping over all the chunks and calling their DrawChunkCollisionGeometry method
			if (DrawGeometry) {
				for (auto C : ChunksToDraw) {
					level.DrawChunkCollisionGeometry(&Renderer, C.X, C.Y);
				}
			}


			//Set the background color to red if collisions are enabled, or green otherwise, and then draw a sqare of that color in the bottom right
			u32 BackgroundColor = Collision ? 0x00ff0000 : 0x0000ff00;
			Renderer.DrawRectangle(Renderer.Config.RenderResX - 66, Renderer.Config.RenderResY - 66, 36, 36, BackgroundColor);

			//If a sprite is selected, draw it over the square (it is smaller so we get a border
			if (SelectedSprite != 0) {
				Renderer.DrawSpriteSS(&level.Sprites[SelectedSprite], Renderer.Config.RenderResX - 64, Renderer.Config.RenderResY - 64);
			}

			//Render our test form
			TestForm.Render(&Renderer);

			Crosshair = true;
		}

		//If the player is being moved, cancel the camera pan

		if (InputManager::Get().GetKeyState('W') || InputManager::Get().GetKeyState('A') || InputManager::Get().GetKeyState('S') || InputManager::Get().GetKeyState('D') || InputManager::Get().GetKeyState(VK_SPACE)) CameraPan = false;

		//Update and render the texture browser
		TextureBrowser.Update(Renderer.Window);
		TextureBrowser.Render(&Renderer);

		//If the camera isn't panning, make the player "push" the screen along
		if (!CameraPan) {

			int ResX = Renderer.Config.RenderResX;
			int ResY = Renderer.Config.RenderResY;


			if (Player.Position.X < Renderer.GetCameraPosition().X + ResX / 4) {
				Renderer.SetCameraPosition(Player.Position.X - ResX / 4, Renderer.GetCameraPosition().Y);
			}

			if (Player.Position.X > (Renderer.GetCameraPosition().X + Renderer.Config.RenderResX) - ResX / 4) {
				Renderer.SetCameraPosition(Player.Position.X + ResX / 4 - Renderer.Config.RenderResX, Renderer.GetCameraPosition().Y);
			}

			if (Player.Position.Y < Renderer.GetCameraPosition().Y + ResY / 4) {
				Renderer.SetCameraPosition(Renderer.GetCameraPosition().X, Player.Position.Y - ResY / 4);
			}

			if (Player.Position.Y > (Renderer.GetCameraPosition().Y + Renderer.Config.RenderResY) - ResY / 4) {
				Renderer.SetCameraPosition(Renderer.GetCameraPosition().X, Player.Position.Y + ResY / 4 - Renderer.Config.RenderResY);
			}
		}

		if (Player.Dead) {
			Crosshair = true;

			const char * Message = "You Died";
			iRect Position = Arial.GetStringRect(0, 0, Message);
			Position.H = 32;
			Position.X = Renderer.Config.RenderResX / 2 - Position.W / 2;
			Position.Y = Renderer.Config.RenderResY / 2 - Position.H / 2;
			Arial.RenderString(&Renderer, Position.X, Position.Y, Message);

			const char * RespawnMsg = "Respawn";
			int W = ArialBlack.GetStringRect(0, 0, RespawnMsg).W;

			Renderer.DrawRectangle(Renderer.Config.RenderResX / 2 - 250 / 2, Renderer.Config.RenderResY / 2 + 32, 250, 34, 0xffffff);
			ArialBlack.RenderString(&Renderer, Renderer.Config.RenderResX / 2 - W / 2, Renderer.Config.RenderResY / 2 + 32, "Respawn");

			if (MS.Btn1) {
				if (MS.x > Renderer.Config.RenderResX / 2 - 250 / 2 && MS.x < Renderer.Config.RenderResX / 2 - 250 / 2 + 250) {
					if (MS.y > Renderer.Config.RenderResY / 2 + 32 && MS.y < Renderer.Config.RenderResY / 2 + 32 + 34) {
						Player.Dead = false;
					}
				}
			}
		}

		if (Crosshair) {
			//Draw a crosshair
			Renderer.DrawRectangle(MS.x - 17, MS.y - 2, 34, 4, 0x000000);
			Renderer.DrawRectangle(MS.x - 2, MS.y - 17, 4, 34, 0x000000);

			Renderer.DrawRectangle(MS.x - 16, MS.y - 1, 32, 2, 0xffffff);
			Renderer.DrawRectangle(MS.x - 1, MS.y - 16, 2, 32, 0xffffff);
		}

		char score[256];
		sprintf_s(score, "Score: %I64d", G.player->Score);
		Arial.RenderString(&Renderer, Renderer.Config.RenderResX - 250, 10, score);

		char time[256];
		sprintf_s(time, "Time: %.2f", Timer);
		Arial.RenderString(&Renderer, Renderer.Config.RenderResX - 250, 52, time);

		if(!PlatformLayer.Update(DeltaTime)) GameRunning = false;
	}

	return 0;
}