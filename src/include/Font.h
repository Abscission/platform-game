
//Copyright (c) 2015 Jason Light
//License: MIT


#pragma once

#include <string>
#include <Types.h>
#include <AssetManager.h>
#include <Renderer.h>

#pragma pack(push, 1)
struct fontInfo {
	s16 fontSize;
	u8 bitField;
	u8 charSet;
	u16 stretchH;
	u8 aa;
	u8 paddingUp;
	u8 paddingRight;
	u8 paddingDown;
	u8 paddingLeft;
	u8 spacingHoriz;
	u8 spacingVert;
	u8 outline;
};

struct charInfo {
	u32 id;
	u16 x;
	u16 y;
	u16 w;
	u16 h;
	s16 xoffset;
	s16 yoffset;
	s16 xadvance;
	u8 page;
	u8 chnl;
};
#pragma pack(pop)

class Font {
private:
	fontInfo Info;
	charInfo* Characters;
	Sprite* BitmapData;
public:

	u32 fontSize;
	std::string name;

	bool Load(AssetFile AssetFile, int BmfIndex, int TextureIndex);

	iRect Font::GetStringRect(int X, int Y, const char * str);
	int RenderString(Renderer* renderer, int X, int Y, const char * string);
};