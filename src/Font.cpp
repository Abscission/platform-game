#include <Font.h>

#include <LogManager.h>
#include <MemoryManager.h>

bool Font::Load(AssetFile AssetFile, int BmfIndex, int TextureIndex) {
	GlobalLog.Write("Reading font from asset file");

	Asset FontAsset = AssetFile.GetAsset(BmfIndex);
	u8* Buffer = (u8*)FontAsset.Memory;

	int fp = 0;

	if (Buffer[0] != 'B' || Buffer[1] != 'M' || Buffer[2] != 'F') {
		GlobalLog.Write("Invalid bitmap font");
		return false;
	}

	fp += 3;

	if (Buffer[3] != 3) {
		GlobalLog.Write("Unrecognized bitmap font version");
		return false;
	}

	fp += 1;
	
	while (fp < FontAsset.Length) {
		u8 BlockType = Buffer[fp++];
		u32 BlockSize = *(u32*)&Buffer[fp];
		fp += 4;

		if (BlockType == 1) {
			//Info Block
			memcpy((void*)&Info, (void*)(Buffer + 4), 14);

			fp += 14;

			int NameLen = BlockSize - 14;
			char* FontName = (char*)malloc(NameLen);

			strcpy_s(FontName, NameLen, (const char *)Buffer + fp);
			this->name = FontName;

			fp += NameLen;
		}
		else if (BlockType == 4) {
			//Characters Block
			int numChars = BlockSize / sizeof(charInfo);
			Characters = MemoryManager::AllocateMemory<charInfo>(numChars);

			for (int i = 0; i < numChars; i++) {
				memcpy((void*)(Characters + i), (void*)(Buffer + fp), sizeof(charInfo));
				fp += sizeof(charInfo);
			}
		}
		else {

			GlobalLog.Write("Unknown BMF Block Type!");
			fp += BlockSize;
		}
	}

	BitmapData = MemoryManager::AllocateMemory<Sprite>(1);
	BitmapData->Load(AssetFile, TextureIndex);

	return true;
}

int Font::RenderString(Renderer* renderer, int X, int Y, const char * str) {
	int length = strlen(str);
	int OrigX = X;

	for (int i = 0; i < length; i++) {
		charInfo* c = &Characters[str[i] - 32];
		renderer->DrawSpriteSS(BitmapData, c->x, c->y, c->w, c->h, X, Y + c->yoffset, true);
		X += c->xadvance;
	}

	return X - OrigX;
}

iRect Font::GetStringRect(int X, int Y, const char * str) {
	int Height = Info.fontSize;
	int Width = 0;

	for (int i = 0; i < strlen(str); i++) {
		Width += Characters[str[i] - 32].xadvance;
	}

	return{ X, Y, Width, Height };
}