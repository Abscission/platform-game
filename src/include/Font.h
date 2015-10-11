
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include <string>
#include <Types.h>
#include <AssetManager.h>
#include <Renderer.h>

#include "ft2build.h"
#include FT_FREETYPE_H


class Font {
private:
	static FT_Library Freetype;
	static bool FreetypeLoaded;
	FT_Face FontFace;

public:

	Font();

	u32 FontSize;
	std::string name;

	bool Load(AssetFile AssetFile, int Index);

	iRect Font::GetStringRect(int X, int Y, const char * str, int size=0);
	iRect RenderString(int X, int Y, const char * string, int size=0, u32 C=0);
};

