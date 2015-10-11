#include "Font.h"
#include "LogManager.h"
#include "MemoryManager.h"
#include "GameLayer.h"
#include "Utility.h"

#include FT_GLYPH_H 

Font::Font() {
	if (!FreetypeLoaded) {
		FT_Error error = FT_Init_FreeType(&Freetype);
		if (error) {
		}
		FreetypeLoaded = true;
	}
}

bool Font::Load(AssetFile AssetFile, int Index) {
	Asset asset = AssetFile.GetAsset(Index);

	FT_Error error = FT_New_Memory_Face(Freetype, (u8*)asset.Memory, asset.Length, 0, &FontFace);
	error = FT_Set_Pixel_Sizes(FontFace, 30, 30);
	return true;
}

iRect Font::RenderString(int X, int Y, const char * str, int Size, u32 C) {
	FT_GlyphSlot Slot = FontFace->glyph;
	size_t length = strlen(str);
	FT_Vector Kerning;
	int initialX = X;

	if (Size != 0 && Size != FontSize) {
		FT_Set_Pixel_Sizes(FontFace, Size, Size);
		FontSize = Size;
	}

	iRect StringRect = GetStringRect(X, Y, str);

	int Baseline = ((StringRect.Y + StringRect.H) - Y);

	Y += Baseline;

	for (int i = 0; i < length; i++) {
		FT_Error error = FT_Load_Char(FontFace, str[i], FT_LOAD_RENDER);
		if (error) continue;

		G.renderer->DrawGlyph(&Slot->bitmap, X + Slot->bitmap_left, Y - Slot->bitmap_top, C);

		FT_Get_Kerning(FontFace, str[i], str[i + 1], FT_KERNING_DEFAULT, &Kerning);

		X += (Kerning.x >> 6) + (Slot->advance.x >> 6);
		Y += (Kerning.y >> 6) + (Slot->advance.y >> 6);
	}

	return{ StringRect.X, StringRect.Y, X - initialX, StringRect.H };
}

iRect Font::GetStringRect(int X, int Y, const char * str, int Size) { 
	int W = 0;
	int H = 0;

	if (Size != 0 && Size != FontSize) {
		FT_Set_Pixel_Sizes(FontFace, Size, Size);
		FontSize = Size;
	}

	size_t Length = strlen(str);

	FT_BBox BBox;
	FT_BBox GlyphBBox;
	FT_Glyph Glyph;
	FT_Vector Kerning;

	BBox.xMin = BBox.yMin = 32000;
	BBox.xMax = BBox.yMax = -32000;

	for (register u64 i = 0; i < Length; i++) {
		FT_Load_Glyph(FontFace, str[i], FT_LOAD_DEFAULT);
		FT_Get_Glyph(FontFace->glyph, &Glyph);
		FT_Glyph_Get_CBox(Glyph, ft_glyph_bbox_pixels, &GlyphBBox);
		
		GlyphBBox.xMin += X;
		GlyphBBox.xMax += X;
		GlyphBBox.yMin += Y;
		GlyphBBox.yMax += Y;

		if (GlyphBBox.xMin < BBox.xMin) BBox.xMin = GlyphBBox.xMin;
		if (GlyphBBox.yMin < BBox.yMin) BBox.yMin = GlyphBBox.yMin;
		if (GlyphBBox.xMax > BBox.xMax) BBox.xMax = GlyphBBox.xMax;
		if (GlyphBBox.yMax > BBox.yMax) BBox.yMax = GlyphBBox.yMax;

		FT_Get_Kerning(FontFace, str[i], str[i + 1], FT_KERNING_DEFAULT, &Kerning);

		X += (Kerning.x >> 6) + (FontFace->glyph->advance.x >> 6);
	}

	return{ BBox.xMin, BBox.yMin, BBox.xMax - BBox.xMin, BBox.yMax - BBox.yMin };
}

FT_Library Font::Freetype;
bool Font::FreetypeLoaded;
