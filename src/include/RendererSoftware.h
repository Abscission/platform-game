#pragma once

#include "Renderer.h"
#include "Maths.h"
#include <Windows.h>

struct Win32ScreenBuffer {
	BITMAPINFO Info;
	int Width;
	int Height;
	int *Memory;
	int BytesPerPixel;
};

class Win32Sprite : public Sprite {
private:
public:
	~Win32Sprite();
	bool Load(const char * filename);
	bool Load(AssetManager::AssetFile, int);

};

bool ResizeSprite(Win32Sprite* Sprite, int W, int H);
bool ResizeSprite(Win32Sprite* Sprite, int W);

bool ResizeSpriteInterpolated(Win32Sprite* Sprite, int W, int H);

struct Light {
public:
	IVec2 Position;
	int Radius;
};

struct LightMap {
public:
	int Width;
	int Height;

	char *Buffer;

	LightMap(int W, int H);
	~LightMap();
};

class RendererWin32Software : public Renderer {
private:
	Win32Sprite BG;
	HBITMAP BM;
	HINSTANCE Instance;
	HDC DeviceContext;
	Win32ScreenBuffer Buffer;
	LightMap *RendererLightMap;
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