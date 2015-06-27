#pragma once

#include "AssetManager.h"

struct RendererConfig{
	int ResX;
	int ResY;
	int BPP;
};

class ImageHeader {
	unsigned int Width;
	unsigned int Height;
	unsigned int length;
	unsigned int HasTransparency;
};

class Sprite {
public:
	virtual bool Load(const char * filename) = 0;
	virtual bool Load(AssetManager::AssetFile AssetFile, int id) = 0;

	ImageHeader Header;

	unsigned long Width;
	unsigned long Height;

	bool hasTransparency;

	//May become OS specific in the future, depending on compatibility
	unsigned int *Data;
	unsigned int  length;
};

class Renderer {
private:
	///<summary>Opens a window for rendering</summary>
	///<param name="Width">The width of the window in pixels</param>
	///<param name="Height">The height of the window in pixels</param>
	///<param name="Title">The title of the window, for the title bar</param>

protected:
	RendererConfig Config;

public:
	virtual bool Initialize() = 0;

	///<summmary>Draws a sprite on the screen</summary>
	virtual void DrawSprite(Sprite* Spr, int X, int Y) = 0;
	virtual void DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY) = 0;
	virtual void DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend) = 0;

	virtual void DrawSpriteRectangle(int X, int Y, int Width, int Height, Sprite* Spr) = 0;

	///<summary>Draws a flat coloured rectangle</summary>
	virtual void DrawRectangle(int X, int Y, int Width, int Height, int Color) = 0;
	virtual bool Refresh() = 0;
};
