
//Copyright (c) 2015 Jason Light
//License: MIT

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
	bool isDrawing;
	bool shouldDraw;
};

struct RendererConfig{
	int RenderResX;
	int RenderResY;
	int WindowResX;
	int WindowResY;
	int BPP;
	bool Fullscreen;
};

class _Sprite {
public:
	bool Load(AssetFile AssetFile, int id);

	unsigned long Width;
	unsigned long Height;

	bool hasTransparency;

	unsigned int *Data;
	unsigned int length;

	~_Sprite();
};

class Sprite {
public:
	bool Load(AssetFile Asset, int id);
	bool Load(AssetFile Asset, int start, int amount);

	bool isAnimated;

	int CurrentFrame;
	int NumberOfFrames;
	int Period;

	int Width;
	int Height;

	u32 CreationTime;

	_Sprite* Frames;
};

bool ResizeSprite(Sprite* Sprite, int W, int H);
bool ResizeSprite(Sprite* Sprite, int W);


bool ResizeSprite(_Sprite* Sprite, int W, int H);
bool ResizeSprite(_Sprite* Sprite, int W);

class Renderer;

struct SlaveArgs {
	Win32ScreenBuffer* Buffer1;
	Win32ScreenBuffer* Buffer2;
	RendererConfig* Config;
	HDC* DeviceContext;
	Renderer* Renderer;
	bool* ShouldClose;
};

class Renderer {
private:
	HBITMAP BM;
	HINSTANCE Instance;
	HDC DeviceContext;
	Win32ScreenBuffer* Buffer;

	Win32ScreenBuffer* Buffer1;
	Win32ScreenBuffer* Buffer2;

	SlaveArgs SlaveArguments;


	IVec2 CameraPos;

public:
	HWND Window;
	RendererConfig Config;

	bool OpenWindow(int Width, int Height, char* Title);

	void SetCameraPosition(IVec2 Position);
	void SetCameraPosition(int X, int Y);

	IVec2 TransformPosition(IVec2 Position) {
		return{ Position.X - CameraPos.X, Position.Y - CameraPos.Y };
	}

	IVec2 GetCameraPosition() { return CameraPos; };

	void DrawSprite(_Sprite* Spr, int X, int Y);
	void DrawSprite(_Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY);
	void DrawSprite(_Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend);
	
	void DrawSprite(Sprite* Spr, int X, int Y);
	void DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY);
	void DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend);

	void DrawSpriteSS(_Sprite* Spr, int X, int Y);
	void DrawSpriteSS(_Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY);
	void DrawSpriteSS(_Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend);

	void DrawSpriteSS(Sprite* Spr, int X, int Y);
	void DrawSpriteSS(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY);
	void DrawSpriteSS(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend);

	//void DrawSpriteWC(_Sprite* Spr, int X, int Y);
	
	void DrawSpriteRectangle(int X, int Y, int Width, int Height, _Sprite* Spr);

	void DrawRectangle(int X, int Y, int Width, int Height, unsigned int Color);
	void DrawRectangleBlend(int X, int Y, int Width, int Height, unsigned int Color);

	void DrawRectangleWS(int X, int Y, int Width, int Height, unsigned int Color);
	void DrawRectangleBlendWS(int X, int Y, int Width, int Height, unsigned int Color);


	bool Initialize();
	bool Refresh();
};

