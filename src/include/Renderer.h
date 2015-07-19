#pragma once

#include "Maths.h"
#include "AssetManager.h"
#include <Windows.h>

#include <d3d9.h>

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
	bool Load(AssetFile AssetFile, int id);

	unsigned long Width;
	unsigned long Height;

	bool hasTransparency;

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
	IVec2 CameraPos;

public:
	HWND Window;
	bool OpenWindow(int Width, int Height, char* Title);

	void SetCameraPosition(IVec2 Position);
	void SetCameraPosition(int X, int Y);

	IVec2 TransformPosition(IVec2 Position) {
		return{ Position.X - CameraPos.X, Position.Y - CameraPos.Y };
	}

	IVec2 GetCameraPosition() { return CameraPos; };

	void DrawSprite(Sprite* Spr, int X, int Y);
	void DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY);
	void DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend);

	void DrawSpriteWC(Sprite* Spr, int X, int Y);
	
	void DrawSpriteRectangle(int X, int Y, int Width, int Height, Sprite* Spr);

	void DrawRectangle(int X, int Y, int Width, int Height, int Color);
	
	bool Initialize();
	bool Refresh();
};