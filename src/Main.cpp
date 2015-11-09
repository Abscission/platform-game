
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

enum EntityType {
	ENTITY_PLAYER = 0,
	ENTITY_PICKUP,
	ENTITY_ENEMY,
	ENTITY_FLAG,
	NUMBER_OF_ENTITY_TYPES
};

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR, int) {
	Font Arial;
	Arial.Load("assets/arial.aaf", 0);
	G.font = &Arial;

	Font Ariali;
	Ariali.Load("assets/arial.aaf", 1);
	G.font_italic = &Ariali;

	char test [] = { 'A', 'S', 'd', '0', '=', 0 };

	toLower(test);

	GlobalLog.WriteF("%s", test);


	//Create a platform layer
	GameLayer PlatformLayer;
	PlatformLayer.Initialize();

	//Get a renderer object from the game layer 
	Renderer Renderer;
	Renderer = PlatformLayer.GetRenderer();

	G.renderer = &Renderer;

	AssetFile Mario("assets/assets.aaf");

	G.player = new Player();
	G.player->LoadSprite(Mario, 0);
	G.player->Color = rgba(255,0,0,255);
	ResizeSprite(G.player->Spr, 48);

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

	Sprite PickupSprite;
	PickupSprite.Load("assets/assets.aaf", 1);

	Level* level = new Level;
	AssetFile LevelAsset("assets/level.aaf");
	level->LoadFromAsset(LevelAsset.GetAsset(0));

	std::vector<IVec2> ChunksToDraw;

	level->SpawnEntity(G.player, 512 * 32, 512);

#ifdef _DEBUG
	Test();
#endif

	float FontTimer = 0;

	u16 SelectedSprite = 0;
	u8 Collision = 0;
	size_t NumSprites = level->Sprites.size();

	bool Paused = false;
	bool CameraPan = false;
	bool DrawGeometry = false;
	bool PlacingPickups = false;
	bool PauseMenu = false;

	Form TestForm(600, 400);
	TestForm.Color = 0xffffffff;

	Label FormTitle(10, 10, "Save Level", 36);
	FormTitle.Color = 0xff6394FF;
	FormTitle.SetFont(&Arial);
	TestForm.Controls.push_back(&FormTitle);

	Label NameLabel(10, 50, "Level Name:", 32);
	NameLabel.Color = 0xff6394FF;
	NameLabel.SetFont(&Arial);
	TestForm.Controls.push_back(&NameLabel);

	Label AuthorLabel(10, 50 + 32 + 10, "Level Author:", 32);
	AuthorLabel.Color = 0xff6394FF;
	AuthorLabel.SetFont(&Arial);
	TestForm.Controls.push_back(&AuthorLabel);
	
	Label FilenameLabel(10, 50 + 64 + 20, "Filename:", 32);
	FilenameLabel.Color = 0xff6394FF;
	FilenameLabel.SetFont(&Arial);
	TestForm.Controls.push_back(&FilenameLabel);

	TextBox NameTextBox(20 + NameLabel.Pos.W, 50, 200, 36, &level->Name);
	NameTextBox.Background = 0xff6394FF;
	NameTextBox.Color = 0xffffff;
	NameTextBox.SetFont(&Arial);
	TestForm.Controls.push_back(&NameTextBox);

	TextBox TestTextBox(20 + AuthorLabel.Pos.W, 50 + 32 + 10, 200, 36, &level->Author);
	TestTextBox.SetFont(&Arial);
	TestForm.Controls.push_back(&TestTextBox);

	TextBox FilenameBox(20 + FilenameLabel.Pos.W, FilenameLabel.Pos.Y, 200, 36, &level->Filename);
	FilenameBox.SetFont(&Arial);
	TestForm.Controls.push_back(&FilenameBox);

	std::string SaveText = "Save";
	Button Save(300, 300, &Arial, &SaveText, &SaveLevel);
	SaveArgs A = { level, &TestForm, &FontTimer };
	Save.Arg = (void*)&A;
	TestForm.Controls.push_back(&Save);

	Form TextureBrowser(-410, 200, 400, 600);
	TextureBrowser.Color = rgba(0, 0, 0, 225);
	TextureBrowser.Enabled = false;

	G.level = level;
	G.player = G.player;

	G.GUIOpen = false;

	EntityType SelectedEntity;

	float Timer = 0;

	MouseState MS = InputManager::Get().GetMouseState(Renderer.Window);
	MouseState MSPrevious = InputManager::Get().GetMouseState(Renderer.Window);

	HelpSign S("Welcome to Jason Light's Platform Game!\nTest a new line!");
	level->SpawnEntity(&S, 512 * 32, 0);
	
	AssetFile level_asset("assets/Levels.aaf");
	Level* Levels = new Level[level_asset.NumberOfAssets];
	u64 NumLevels = level_asset.NumberOfAssets;

	for (int i = 0; i < level_asset.NumberOfAssets; i++) {
		Levels[i].LoadInfoFromAsset(level_asset.GetAsset(i));
	}

	bool GameRunning = true;
	while (GameRunning) {

		bool Crosshair = false;

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

		//Update the Input Manager
		InputManager::Get().Update();
		ControllerState Controller = InputManager::Get().GetControllerState();
		MSPrevious = MS;
		MS = InputManager::Get().GetMouseState(Renderer.Window);
		iRect MouseRect{ MS.x, MS.y, 1, 1 };

		iRect Screen = { 0, 0, Renderer.Config.RenderResX, Renderer.Config.RenderResY };

		switch (G.Screen) {

		case MAIN_MENU: {
			Crosshair = true;
			Arial.RenderString(40, 40, "Platform Game", 72, 0xffffff);
			iRect PlayHitbox = Arial.RenderString(40, Screen.H - 128, "Play", 32, 0xffffff);
			iRect OptionsHitbox = Arial.RenderString(40, Screen.H - 92, "Options", 32, 0xffffff);
			iRect ExitHitbox = Arial.RenderString(40, Screen.H - 56, "Exit", 32, 0xffffff);

			if (MS.Btn1) {
				if (CheckCollisionAABB(PlayHitbox, { (float)MS.x, (float)MS.y, 1.f, 1.f })) {
					G.Screen = LEVEL_SELECT;
					G.LastScreen = MAIN_MENU;
				}
				else if (CheckCollisionAABB(OptionsHitbox, { (float)MS.x, (float)MS.y, 1.f, 1.f })) {
					G.Screen = OPTIONS_MENU;
					G.LastScreen = MAIN_MENU;
				}
				else if (CheckCollisionAABB(ExitHitbox, { (float)MS.x, (float)MS.y, 1.f, 1.f })) GameRunning = false;
			}
		} break;

		case OPTIONS_MENU: {
			Crosshair = true;

			Arial.RenderString(40, 40, "Options", 72, 0xffffff);

			Arial.RenderString(Screen.W / 4, 120, "Resolution", 36, 0xffffff);

			Renderer.DrawRectangle(Screen.W / 4 + 300, 117, 400, 36, 0xffffff);
			iRect ResolutionRect = Arial.GetStringRect(Screen.W / 4 + 320, 122, "1680 x 1050", 32);
			Renderer.DrawRectangleBlend(Screen.W / 4 + 300 + 200 - ResolutionRect.W / 2, ResolutionRect.Y, ResolutionRect.W, ResolutionRect.H, 0x22000000);
			Arial.RenderString(Screen.W / 4 + 300 + 200 - ResolutionRect.W / 2, 122, "1680 x 1050", 32);

			if (InputManager::Get().GetKeyDown(VK_ESCAPE)) G.Screen = G.LastScreen;

		} break;

		case LEVEL_SELECT: {
			Crosshair = true;
			Arial.RenderString(40, 40, "Level Select", 72, 0xffffff);

			iRect StringRect = Arial.RenderString(200, 200, "Choose player color (A and D to select): ", 32, 0xffffffff);
			Renderer.DrawRectangleBlend(220 + StringRect.W, 200, 32, 32, G.player->Color);
			if (InputManager::Get().GetKeyState('D')) {
				hsv_color C = RGBtoHSV(G.player->Color);
				C.h += (float)(DeltaTime * 100);
				if (C.h > 360) 
					C.h = 0;
				G.player->Color = HSVtoRGB(C).color;
			}

			if (InputManager::Get().GetKeyState('A')) {
				hsv_color C = RGBtoHSV(G.player->Color);
				C.h -= (float)(DeltaTime * 100);
				if (C.h < 0)
					C.h = 360;
				G.player->Color = HSVtoRGB(C).color;
			}
			
			if (InputManager::Get().GetKeyDown(VK_RETURN)) {
				G.player->UpdateColor();
				G.Screen = IN_GAME;
			}

			for (int i = 0; i < NumLevels; i++) {
				int LevelX = 20 + 420 * i;
				int LevelY = 400;

				Renderer.DrawRectangle(LevelX, LevelY, 400, 300, 0xffffff);
				G.font->RenderString(LevelX + 10, LevelY + 10, Levels[i].Name.c_str(), 36, rgba_color( 99, 148, 255, 255 ).color);
				G.font_italic->RenderString(LevelX + 10, LevelY + 46, ("by " + Levels[i].Author).c_str(), 28, rgba_color(99, 148, 255, 255).color);

				if (MS.Btn1) {
					if (CheckCollisionAABB(MouseRect, { (float)LevelX, (float)LevelY, 400, 300 })) {
						Levels[i].LoadFromAsset(LevelAsset.GetAsset(i));
						level = &Levels[i];
						G.level = &Levels[i];
						G.Screen = IN_GAME;
					}
				}
			}
			if (InputManager::Get().GetKeyDown(VK_ESCAPE)) G.Screen = G.LastScreen;


		} break;

		case IN_GAME: {
				TestForm.Update(Renderer.Window);

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
					FontTimer -= (float)DeltaTime;
				}

				//Zero out the deltatime for everything else
				if ((Paused && G.editing) || G.player->Dead || PauseMenu) DeltaTime = 0;

				Timer += (float)DeltaTime;

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
				level->Update(DeltaTime);

				//And draw all the chunks
#pragma loop(hint_parallel(8))
				for (register uP i = 0; i < ChunksToDraw.size(); i++) {
					level->DrawChunk(&Renderer, ChunksToDraw[i].X, ChunksToDraw[i].Y);
				}

				//Draw the entities too
				for (auto E : level->Entities) {
					E->Draw(&Renderer);
				}

				//Some toggles
				if (InputManager::Get().GetKeyDown(VK_F2)) G.editing = !G.editing;
				if (InputManager::Get().GetKeyDown(VK_ESCAPE) && (!G.GUIOpen || PauseMenu)) {
					PauseMenu = !PauseMenu;
					G.GUIOpen = !G.GUIOpen;
				}

				if (PauseMenu) {
					Crosshair = true;
					Arial.RenderString(10, 10, "Paused", 72, 0xffffff);
					iRect ResumeBox = Arial.RenderString(10, Renderer.Config.RenderResY - 36 * 3, "Resume", 32, 0xffffff);
					iRect OptionsBox = Arial.RenderString(10, Renderer.Config.RenderResY - 36 * 2, "Options", 32, 0xffffff);
					iRect QuitBox = Arial.RenderString(10, Renderer.Config.RenderResY - 36, "Quit to Main Menu", 32, 0xffffff);

					if (MS.Btn1) {
						if (CheckCollisionAABB(ResumeBox, MouseRect)) {
							PauseMenu = false;
							G.GUIOpen = false;
						}
						if (CheckCollisionAABB(OptionsBox, MouseRect)) {
							G.LastScreen = IN_GAME;
							G.Screen = OPTIONS_MENU;
						}
						if (CheckCollisionAABB(QuitBox, MouseRect)) G.Screen = MAIN_MENU;
					}
				}

				if (FontTimer > 0) {
					Arial.RenderString(10, 10, "Saving level->..");
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
						int X = (MS.x + CameraPos.X);
						int Y = (MS.y + CameraPos.Y);

						//If they are in bounds
						if (!(X < 0 || Y < 0 || X > UINT16_MAX || Y > UINT16_MAX)) {
							if (PlacingPickups) {
								if (!MSPrevious.Btn1) {
									GameObject * P;

									switch (SelectedEntity) {
									case ENTITY_ENEMY:
										P = new Enemy();
										P->LoadSprite("assets/assets.aaf", 0);
										break;
									case ENTITY_FLAG:
										P = new EndFlag();
										P->LoadSprite("assets/assets.aaf", 2, 4);
										P->Spr->Period = 700;
										break;
									case ENTITY_PICKUP:
										P = new Pickup();
										break;
									case ENTITY_PLAYER:
										if (G.player) delete G.player;

										G.player = new Player();
										G.player->LoadSprite("assets/assets.aaf", 0);
										level->DespawnEntity(G.player);
										P = (GameObject*)G.player;
										break;
									default:
										P = new GameObject();
										assert(false, "This shouldn't happen!");
									}

									//Grid Align if ctrl not held
									if (!InputManager::Get().GetKeyState(VK_CONTROL)) {
										X -= X % 32;
										Y -= Y % 32;
									}

									level->SpawnEntity(P, X, Y);
								}
							}
							else {
								X /= 32;
								Y /= 32;
								//Get the coordinate of the chunk
								int ChunkX = X / 16;
								int ChunkY = Y / 16;

								//And the coordinate within the chunk
								int LocalX = X % 16;
								int LocalY = Y % 16;

								//Get a pointer to the chunk from the level
								Chunk* C = level->GetChunk(ChunkX, ChunkY);

								//If it succeeded
								if (C != nullptr) {
									//Set the block at the grid location to the currently selected sprite and collision values
									C->Grid[16 * LocalY + LocalX] = { SelectedSprite, Collision };

									//If the chunk isn't listed as existing, list it now. This is for level saving purposes.
									if (!C->inIndex) {
										C->inIndex = true;
										level->ExistingChunks.push_back({ C->X, C->Y });
									}

									//Update the geometry
									level->SetChunkGeometry(C);
								}
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

							Chunk* C = level->GetChunk(ChunkX, ChunkY);

							if (C != nullptr) {
								C->Grid[16 * LocalY + LocalX] = { 0, 0 };
								level->SetChunkGeometry(C);
							}
						}

					}

					//More toggles
					if (InputManager::Get().GetKeyDown('O')) PlacingPickups = !PlacingPickups;
					if (InputManager::Get().GetKeyDown('Q')) {
						if (PlacingPickups) {
							SelectedEntity = (EntityType)(SelectedEntity - 1);
							if (SelectedEntity < 0) SelectedEntity = (EntityType)(NUMBER_OF_ENTITY_TYPES - 1);
						}
						else if (SelectedSprite > 0) SelectedSprite--;
					}
					if (InputManager::Get().GetKeyDown('E')) {
						if (PlacingPickups) {
							SelectedEntity = (EntityType)(SelectedEntity + 1);
							if (SelectedEntity >= NUMBER_OF_ENTITY_TYPES) SelectedEntity = (EntityType)0;
						}
						else if (SelectedSprite < NumSprites - 1) SelectedSprite++;
					}
					if (InputManager::Get().GetKeyDown('Z')) G.debuglos = !G.debuglos;
					if (InputManager::Get().GetKeyDown(VK_TAB)) Collision = Collision == 0 ? COLLIDE_ALL : 0;
					if (InputManager::Get().GetKeyDown('C')) DrawGeometry = !DrawGeometry;
					if (InputManager::Get().GetKeyDown('S')) {
						if (InputManager::Get().GetKeyState(VK_CONTROL)) {
							TestForm.Enabled = true;
						}
					}
					if (InputManager::Get().GetKeyDown('P')) Paused = !Paused;



					//Draw the chunk geometry if requestsed, by looping over all the chunks and calling their DrawChunkCollisionGeometry method
					if (DrawGeometry) {
						for (auto C : ChunksToDraw) {
							level->DrawChunkCollisionGeometry(&Renderer, C.X, C.Y);
						}
					}


					//Set the background color to red if collisions are enabled, or green otherwise, and then draw a sqare of that color in the bottom right
					u32 BackgroundColor = Collision ? 0x00ff0000 : 0x0000ff00;

					if (PlacingPickups) {
						Sprite Spr;
						switch (SelectedEntity) {
						case ENTITY_ENEMY:
							Spr.Load("assets/assets.aaf", 0);
							break;
						case ENTITY_FLAG:
							Spr.Load("assets/assets.aaf", 2, 4);
							Spr.Period = 700;
							break;
						case ENTITY_PICKUP:
							Spr.Load("assets/assets.aaf", 1);
							break;
						case ENTITY_PLAYER:
							Spr.Load("assets/assets.aaf", 1);
							break;
						}
						Renderer.DrawSpriteSS(&Spr, Renderer.Config.RenderResX - 64, Renderer.Config.RenderResY - 64);
					} 
					else {

						Renderer.DrawRectangle(Renderer.Config.RenderResX - 66, Renderer.Config.RenderResY - 66, 36, 36, BackgroundColor);

						//If a sprite is selected, draw it over the square (it is smaller so we get a border
						if (SelectedSprite != 0) {
							Renderer.DrawSpriteSS(&level->Sprites[SelectedSprite], Renderer.Config.RenderResX - 64, Renderer.Config.RenderResY - 64);
						}
					}
					//Render our test form
					TestForm.Render();

					Crosshair = true;
				}

				//If the player is being moved, cancel the camera pan

				if (InputManager::Get().GetKeyState('W') || InputManager::Get().GetKeyState('A') || InputManager::Get().GetKeyState('S') || InputManager::Get().GetKeyState('D') || InputManager::Get().GetKeyState(VK_SPACE)) CameraPan = false;

				//Update and render the texture browser
				TextureBrowser.Update(Renderer.Window);
				TextureBrowser.Render();

				//If the camera isn't panning, make the player "push" the screen along
				if (!CameraPan) {

					int ResX = Renderer.Config.RenderResX;
					int ResY = Renderer.Config.RenderResY;


					if (G.player->Position.X < Renderer.GetCameraPosition().X + ResX / 4) {
						Renderer.SetCameraPosition((int)(G.player->Position.X - ResX / 4), Renderer.GetCameraPosition().Y);
					}

					if (G.player->Position.X > (Renderer.GetCameraPosition().X + Renderer.Config.RenderResX) - ResX / 4) {
						Renderer.SetCameraPosition((int)(G.player->Position.X + ResX / 4 - Renderer.Config.RenderResX), Renderer.GetCameraPosition().Y);
					}

					if (G.player->Position.Y < Renderer.GetCameraPosition().Y + ResY / 4) {
						Renderer.SetCameraPosition(Renderer.GetCameraPosition().X, (int)(G.player->Position.Y - ResY / 4));
					}

					if (G.player->Position.Y > (Renderer.GetCameraPosition().Y + Renderer.Config.RenderResY) - ResY / 4) {
						Renderer.SetCameraPosition(Renderer.GetCameraPosition().X, (int)(G.player->Position.Y + ResY / 4 - Renderer.Config.RenderResY));
					}
				}

				if (G.player->Dead) {
					Crosshair = true;

					const char * Message = "You Died";
					iRect Position = Arial.GetStringRect(0, 0, Message);
					Position.H = 32;
					Position.X = Renderer.Config.RenderResX / 2 - Position.W / 2;
					Position.Y = Renderer.Config.RenderResY / 2 - Position.H / 2;
					Arial.RenderString(Position.X, Position.Y, Message);

					const char * RespawnMsg = "Respawn";
					int W = Arial.GetStringRect(0, 0, RespawnMsg).W;

					Renderer.DrawRectangle(Renderer.Config.RenderResX / 2 - 250 / 2, Renderer.Config.RenderResY / 2 + 32, 250, 34, 0xffffff);
					Arial.RenderString(Renderer.Config.RenderResX / 2 - W / 2, Renderer.Config.RenderResY / 2 + 32, "Respawn");

					if (MS.Btn1) {
						if (MS.x > Renderer.Config.RenderResX / 2 - 250 / 2 && MS.x < Renderer.Config.RenderResX / 2 - 250 / 2 + 250) {
							if (MS.y > Renderer.Config.RenderResY / 2 + 32 && MS.y < Renderer.Config.RenderResY / 2 + 32 + 34) {
								G.player->Dead = false;
							}
						}
					}
				}

				char score[256];
				sprintf_s(score, "Score: %I64d", G.player->Score);
				Arial.RenderString(Renderer.Config.RenderResX - 250, 10, score, 32, 0xffffff);

				char time[256];
				sprintf_s(time, "Time: %.2f", Timer);
				Arial.RenderString(Renderer.Config.RenderResX - 250, 52, time, 32, 0xffffff);
			} break;
		}

		if (Crosshair) {
			//Draw a crosshair
			Renderer.DrawRectangle(MS.x - 17, MS.y - 2, 34, 4, 0x000000);
			Renderer.DrawRectangle(MS.x - 2, MS.y - 17, 4, 34, 0x000000);

			Renderer.DrawRectangle(MS.x - 16, MS.y - 1, 32, 2, 0xffffff);
			Renderer.DrawRectangle(MS.x - 1, MS.y - 16, 2, 32, 0xffffff);
		}

		if (!PlatformLayer.Update(DeltaTime)) GameRunning = false;

	}
	return 0;
}