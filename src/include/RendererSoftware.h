#pragma once

#include "Maths.h"
#include "AssetManager.h"
#include <Windows.h>

class ImageHeader {
public:
	unsigned int Width;
	unsigned int Height;
	unsigned int length;
	unsigned int HasTransparency;
};

struct Win32ScreenBuffer {
	BITMAPINFO Info;
	int Width;
	int Height;
	int *Memory;
	int BytesPerPixel;
};

struct RendererConfig{
	int RenderResX;
	int RenderResY;
	int WindowResX;
	int WindowResY;
	int BPP;
	bool Fullscreen;
};

class Sprite {
public:
	bool Load(const char * filename);
	bool Load(AssetManager::AssetFile AssetFile, int id);

	ImageHeader Header;

	unsigned long Width;
	unsigned long Height;

	bool hasTransparency;

	//May become OS specific in the future, depending on compatibility
	unsigned int *Data;
	unsigned int length;

	~Sprite();
};

bool ResizeSprite(Sprite* Sprite, int W, int H);
bool ResizeSprite(Sprite* Sprite, int W);

class Renderer {
private:
	HBITMAP BM;
	HINSTANCE Instance;
	HDC DeviceContext;
	Win32ScreenBuffer Buffer;
	RendererConfig Config;
public:
	HWND Window;
	bool OpenWindow(int Width, int Height, char* Title);

	void DrawSprite(Sprite* Spr, int X, int Y);
	void DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY);
	void DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend);

	void DrawSpriteRectangle(int X, int Y, int Width, int Height, Sprite* Spr);

	void DrawRectangle(int X, int Y, int Width, int Height, int Color);
	
	void UpdateLighting();
	void ApplyLighting();

	bool Initialize();
	bool Refresh();
};