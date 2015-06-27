
#include <Windows.h>
#include <vector>

#include "PicoPNG.h"

#include "Utility.h"
#include "RendererSoftware.h"

#include "AssetManager.h"

bool ShouldClose = false;

//The window callback, this is what processes messages from our windows
LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_CLOSE:
		DestroyWindow(Window);
		ShouldClose = true;
		return 0;
	case WM_KEYDOWN:
		switch (wParam){
		case VK_UP:
			break;
		}

	case WM_KEYUP:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(Window);
			ShouldClose = true;
			return 0;
		}

	default:
		return DefWindowProc(Window, Message, wParam, lParam);
	}
}

//Takes a pixel X and lightens it by amount
//Use negative values to darken
inline void Lighten(int* Pixel, char Amount) {
	//This is an optimization for later, when pixels may be lightened based on the lighting engine
	//In this case there may be a light level of one on the object, therefore we can just return the color
	if (Amount == 0)
	{
		return;
	}
	else {
		//Get the channels from the colour.
		//A isn't shifted down because we don't use it in processing, and it will just be shifted back up again
		int A, R, G, B;
		A = *Pixel & 0xFF000000;
		R = (*Pixel & 0x00FF0000) >> 16;
		G = (*Pixel & 0x0000FF00) >> 8;
		B = *Pixel & 0x000000FF;
		
		//Add the amount to each channel, thereby lightening it
		R += Amount;
		G += Amount;
		B += Amount;

		//Prevent overflow problems
		if (Amount > 0) {
			R = MIN(R, 0xFF);
			G = MIN(G, 0xFF);
			B = MIN(B, 0xFF);
		}
		else {
			R = MAX(R, 0);
			G = MAX(G, 0);
			B = MAX(B, 0);
		}

		*Pixel =  (A | R << 16 | G << 8 | B);
	}
}

//A function to blend two pixels based on the 
inline void Blend(unsigned int* Source, unsigned int* Dest) {
	//Get the source channels
	byte* SA = ((byte*)Source) + 3;
	byte* SB = ((byte*)Source) + 2;
	byte* SG = ((byte*)Source) + 1;
	byte* SR = ((byte*)Source);

	//Get the destination channels
	byte* DB = ((byte*)Dest) + 2;
	byte* DG = ((byte*)Dest) + 1; 
	byte* DR = ((byte*)Dest);

	//Set the destination color based on the source and destination channel
	*DR = *SR + ((*DR * (256 - *SA)) >> 8);
	*DG = *SG + ((*DG * (256 - *SA)) >> 8);
	*DB = *SB + ((*DB * (256 - *SA)) >> 8);
}

bool ResizeSprite(Win32Sprite* Sprite, int W) {
	return ResizeSprite(Sprite, W, Sprite->Height * (W / Sprite->Width));
}


//A function to resize a sprite using nearest neighbour
bool ResizeSprite(Win32Sprite* Sprite, int W, int H){
	//Initialize a temporary buffer for the sprite
	unsigned int *TempBuffer = (unsigned int*)VirtualAlloc(0, W * H * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	
	//Calculate the scaling ratios
	//I shift the values up by 16 bits to effectively create a fixed point decimal number
	//The top 16 bits are the whole part, and the bottom are the fractional part
	//This is cheaper and more precise than floating point numbers 
	//(Also the FPU is likely under heavy load from the physics engine)
	unsigned long Scale_X = (int)((Sprite->Width << 16) / W) + 1;
	unsigned long Scale_Y = (int)((Sprite->Height << 16) / H) + 1;

	//Location of the original pixel
	int PixelX, PixelY;
	
	for (int Y = 0; Y < H; Y++) {
		for (int X = 0; X < W; X++) {
			//Find the pixel at the correct location
			//At this point we shift the numbers back to normal integers
			PixelX = (int)(X * Scale_X) >> 16;
			PixelY = (int)(Y * Scale_Y) >> 16;

			TempBuffer[Y * W + X] = Sprite->Data[(PixelY * Sprite->Width) + PixelX];

		}
	}

	//Delete the old Sprite data
	if (Sprite->Data) VirtualFree(Sprite->Data, Sprite->Width * Sprite->Height * 4, MEM_RELEASE);
	
	//Set the sprite data to the new buffer
	Sprite->Data = TempBuffer;

	//Set up the sprites members according to the change
	Sprite->Width = W;
	Sprite->Height = H;
	Sprite->length = W * H * 4;

	return true;
}

union RGB_Pixel {
	unsigned int Val;
	struct{
		unsigned char B, G, R, A;
	};
};

struct Pixel {
	int X, Y;
	RGB_Pixel Value;
};


inline unsigned char Interpolate(char A, char B, char D) {
	return (A * D) >> 8 + (B * (256 - D)) >> 8;
}

bool ResizeSpriteInterpolated(Win32Sprite* Sprite, int W, int H) {
	//Initialize a temporary buffer for the sprite
	unsigned int *TempBuffer = (unsigned int*)VirtualAlloc(0, W * H * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	//Calculate the scaling ratios
	double ScaleX = (double)Sprite->Width / W;
	double ScaleY = (double)Sprite->Height / H;

	//Location of the original pixel
	double PixelX, PixelY;
	double DistanceX, DistanceY;

	int TmpX, TmpY;
	Pixel TL, TR, BL, BR;

	for (int Y = 0; Y < H; Y++) {
		for (int X = 0; X < W; X++) {
			//Find the pixel at the correct location
			//At this point we shift the numbers back to normal integers
			PixelX = (double)X * ScaleX;
			PixelY = (double)Y * ScaleY;

			//Round the pixel values
			TmpX = (int)(PixelX + 0.5);
			TmpY = (int)(PixelY + 0.5);

			//Get the adjacent Pixel locations

			//Deal with edges
			//This method is a lot of code, but fast

			
			if (TmpX <= 0 && TmpY <= 0) {
				//Top Left Corner
				TL = { TmpX, TmpY }; //Bottom Right
				TR = { TmpX, TmpY }; //Bottom Right
				BL = { TmpX, TmpY }; //Bottom Right
				BR = { TmpX, TmpY }; //Bottom Right
			}
			else if (TmpX <= 0 && TmpY >= H) {
				//Bottom Left Corner
				TL = { TmpX, TmpY - 1 }; //Top Right
				TR = { TmpX, TmpY - 1 }; //Top Right
				BL = { TmpX, TmpY - 1 }; //Top Right
				BR = { TmpX, TmpY - 1 }; //Top Right
			}
			else if (TmpX >= W && TmpY <= 0){
				//Top Right Corner
				TL = { TmpX - 1, TmpY }; //Bottom Left
				TR = { TmpX - 1, TmpY }; //Bottom Left
				BL = { TmpX - 1, TmpY }; //Bottom Left
				BR = { TmpX - 1, TmpY }; //Bottom Left
			}
			else if (TmpX >= W && TmpY >= H) {
				//Bottom Right Corner
				TL = { TmpX - 1, TmpY - 1 }; //Top Left
				TR = { TmpX - 1, TmpY - 1 }; //Top Left
				BL = { TmpX - 1, TmpY - 1 }; //Top Left
				BR = { TmpX - 1, TmpY - 1 }; //Top Left
			}
			else if (TmpX <= 0) {
				//Left Edge
				TL = { TmpX, TmpY - 1 };  //Top Right
				TR = { TmpX, TmpY - 1 }; //Top Right
				BL = { TmpX, TmpY }; //Bottom Right
				BR = { TmpX, TmpY }; //Bottom Right
			}
			else if (TmpY <= 0) {
				//Top Edge
				TL = { TmpX - 1, TmpY }; //Bottom Left
				TR = { TmpX, TmpY }; //Bottom Right
				BL = { TmpX - 1, TmpY }; //Bottom Left
				BR = { TmpX, TmpY }; //Bottom Right
			}
			else if (TmpY >= H)
			{
				//Bottom Edge
				TL = { TmpX - 1, TmpY - 1 }; //Top Left
				TR = { TmpX, TmpY - 1 }; //Top Right
				BL = { TmpX - 1, TmpY - 1}; //Top Left
				BR = { TmpX, TmpY - 1 }; //Top Right
			}
			else if (TmpX >= W) {
				//Right Edge
				TL = { TmpX - 1, TmpY - 1 }; //Top Left
				TR = { TmpX - 1, TmpY - 1 }; //Top Left
				BL = { TmpX - 1, TmpY }; //Bottom Left
				BR = { TmpX - 1, TmpY }; //Bottom Left
			}
			else {
				TL = { TmpX - 1, TmpY - 1 }; //Top Left
				TR = { TmpX, TmpY - 1 }; //Top Right
				BL = { TmpX - 1, TmpY }; //Bottom Left
				BR = { TmpX, TmpY }; //Bottom Right
			}

			TL.Value.Val = Sprite->Data[TL.X * Sprite->Width + TL.Y];
			TR.Value.Val = Sprite->Data[TR.X * Sprite->Width + TR.Y];
			BL.Value.Val = Sprite->Data[BL.X * Sprite->Width + BL.Y];
			BR.Value.Val = Sprite->Data[BR.X * Sprite->Width + BR.Y];

			DistanceX = (PixelX) - (TL.X);
			DistanceY = (PixelY) - (TL.Y);

			//X Axis interpolation
			//Y Axis interpolation
		}
	}

	//Delete the old Sprite data
	if (Sprite->Data) VirtualFree(Sprite->Data, Sprite->Width * Sprite->Height * 4, MEM_RELEASE);
	
	//Set the sprite data to the new buffer
	Sprite->Data = TempBuffer;

	//Set up the sprites members according to the change
	Sprite->Width = W;
	Sprite->Height = H;
	Sprite->length = W * H * 4;

	return true;
	//return false; //Change to true when working
}



bool Win32Sprite::Load(const char * Filename) {
	Win32FileContents FileContents = ReadEntireFile(Filename);

	if (this->Data) VirtualFree(this->Data, this->Width * this->Height * 4, MEM_RELEASE);

	std::vector<unsigned char> Image;

	decodePNG(Image, this->Width, this->Height, FileContents.Data, (unsigned long)FileContents.Size);

	this->Data = (unsigned int *)VirtualAlloc(0, Image.size() * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	std::copy(Image.begin(), Image.end(), (unsigned char*)this->Data);

	hasTransparency = false;

	for (int i = 0; i < this->Width * this->Height * 4; i++) {
		//Transformations to be done on each pixel of the loaded sprite
		unsigned int* P = Data + i;

		//Swap the Red and Blue bytes by masking out the relevant bits, shifting them into place, and using bitwise or to combine the bytes
		//Doing this now is a lot faser than doing it in the rendering code later as I used to
		*P = (*P & 0xff00ff00) | ((*P & 0xff0000) >> 16) | ((*P & 0xff) << 16);

		//Do premultiplied alpha, should make graphics scale and blend better
		float Alpha = (((*P & 0xff000000) >> 24) / 255.f);
		*P = (*P & 0xff000000) | ((int)((*P & 0xff0000) * Alpha) & 0xff0000) | ((int)((*P & 0xff00) * Alpha) & 0xff00) | ((int)((*P & 0xff) * Alpha) & 0xff);

		if (Alpha != 0) {
			hasTransparency = true;
		}
	}
	
	return true;
}

bool Win32Sprite::Load(AssetManager::AssetFile AssetFile, int id){
	AssetManager::Asset BMP = AssetFile.GetAsset(id);
	this->Data = (unsigned int*)BMP.Memory;
	/*hasTransparency = false;

	for (int i = 0; i < this->Width * this->Height * 4; i++) {
		//Transformations to be done on each pixel of the loaded sprite
		unsigned int* P = Data + i;

		//Swap the Red and Blue bytes by masking out the relevant bits, shifting them into place, and using bitwise or to combine the bytes
		//Doing this now is a lot faser than doing it in the rendering code later as I used to
		*P = (*P & 0xff00ff00) | ((*P & 0xff0000) >> 16) | ((*P & 0xff) << 16);

		//Do premultiplied alpha, should make graphics scale and blend better
		float Alpha = (((*P & 0xff000000) >> 24) / 255.f);
		*P = (*P & 0xff000000) | ((int)((*P & 0xff0000) * Alpha) & 0xff0000) | ((int)((*P & 0xff00) * Alpha) & 0xff00) | ((int)((*P & 0xff) * Alpha) & 0xff);

		if (Alpha != 0) {
			hasTransparency = true;
		}
	}*/

	return true;
}

Win32Sprite::~Win32Sprite() {
	if (this->Data) VirtualFree(this->Data, this->Width * this->Height * 4, MEM_RELEASE);
}


bool RendererWin32Software::OpenWindow(int Width, int Height, char* Title){
	WNDCLASSEX WindClass = {}; //Create a Window Class structure
	
	//Check if the class has been registered aleady	
	if (!GetClassInfoEx(Instance, "JasonWindowClassName", &WindClass)) {
		//Create and fill a Window Class structure
		WindClass.hInstance = Instance;
		WindClass.lpfnWndProc = WindowProc;
		WindClass.cbSize = sizeof(WindClass);
		WindClass.style = CS_HREDRAW | CS_VREDRAW; //Redraw the Window if it is moved or resized horizontally or vertically
		WindClass.lpszClassName = "JasonWindowClassName"; //Unique identifying class name

		if (!RegisterClassEx(&WindClass)) {
			DWORD error = GetLastError();
			DisplayMessage(error);
			return false;
		}
	}

	DWORD WindowStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_VISIBLE;
	
	RECT WindowRect = { 0, 0, Width, Height };
	AdjustWindowRect(&WindowRect, WindowStyle, false);

	this->Window = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, "JasonWindowClassName", Title, WindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, WindowRect.right, WindowRect.bottom, 0, 0, Instance, 0);
	return true;
}

bool RendererWin32Software::Initialize() {
	Config.ResX = 800;
	Config.ResY = 600;
	Config.BPP = 4;

	Instance = GetModuleHandle(NULL);
	this->OpenWindow(Config.ResX, Config.ResY, "Title");
	this->DeviceContext = GetWindowDC(this->Window);

	//Create a DIB to render to
	if (Buffer.Memory) {
		VirtualFree(Buffer.Memory, 0, MEM_RELEASE);
	}

	Buffer.Width = Config.ResX;
	Buffer.Height = Config.ResY;
	Buffer.BytesPerPixel = 4;

	Buffer.Info.bmiHeader.biSize = sizeof(Buffer.Info.bmiHeader);
	Buffer.Info.bmiHeader.biWidth = Buffer.Width;
	Buffer.Info.bmiHeader.biHeight = -Buffer.Height;
	Buffer.Info.bmiHeader.biPlanes = 1;
	Buffer.Info.bmiHeader.biBitCount = 32;
	Buffer.Info.bmiHeader.biCompression = BI_RGB;

	int MemorySize = Buffer.Width * Buffer.Height * Buffer.BytesPerPixel;
	Buffer.Memory = (int*)VirtualAlloc(0, MemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	RendererLightMap = new LightMap(Config.ResX, Config.ResX);

	return 0;
}

bool RendererWin32Software::Refresh() {

	static float test = 0;

	//Now update the screen
	StretchDIBits(DeviceContext, 0, 0, Buffer.Width, Buffer.Height, 0, 0, Buffer.Width, Buffer.Height, Buffer.Memory, &Buffer.Info, DIB_RGB_COLORS, SRCCOPY);

	//Clear the backbuffer
	memset(Buffer.Memory, 0, Buffer.Width * Buffer.Height * Buffer.BytesPerPixel);

	//DrawRectangle(0, 0, Buffer.Width, Buffer.Height, 0x222222);
	
	return !ShouldClose;
}

void RendererWin32Software::DrawRectangle(int X, int Y, int Width, int Height, int Color) {
	for (int y = 0; y < Height; y++){
		int Down = Buffer.Width * (Y + y);
		for (int x = 0; x < Width; x++) {
			((int*)Buffer.Memory)[Down + (X + x)] = Color;
		}
	}
}


void RendererWin32Software::DrawSpriteRectangle(int X, int Y, int Width, int Height, Sprite* Spr) {
	for (int y = Y; y < Y + Height; y+= Spr->Width){
		int Down = Buffer.Width * (Y + y);
		for (int x = X; x < X + Width; x += Spr->Width) {
			DrawSprite(Spr, x, y);
		}
	}
}

void RendererWin32Software::DrawSprite(Sprite* Spr, int X, int Y) {
	DrawSprite(Spr, 0, 0, Spr->Width, Spr->Height, X, Y, true);
}

void RendererWin32Software::DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY){
	DrawSprite(Spr, SrcX, SrcY, Width, Height, DstX, DstY, true);
}

void RendererWin32Software::DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool ShouldBlend){
	//Out of bounds checks
	//If the sprite is drawn partially offscreen, draw a section
	//If it is fully offscreen, don't draw it.
	if (DstX < 0) {
		SrcX -= DstX;
		Width += DstX;

		DstX = 0;
		if (Width <= 0) return;
	}

	if (DstY < 0){	
		SrcY += -DstY;
		Height += DstY;

		DstY = 0;
		if (Height <= 0) return;
	}

	if (DstX + Width > Buffer.Width) {
		Width = Buffer.Width - DstX;
		DstX = Buffer.Width - Width;

		if (Width <= 0) {
			return;
		}
	}

	if (DstY + Height > Buffer.Height) {
		Height = Buffer.Height - DstY;
		DstY = Buffer.Height - Height;

		if (Height <= 0) return;
	}

	for (unsigned int y = SrcY; y < (SrcY + Height); y++){
		if (ShouldBlend && Spr->hasTransparency) {
			//If the pixel should be drawn with transparency itterate over each pixel
			for (unsigned int x = SrcX; x < (SrcX + Width); x++) {
				unsigned int ARGB = Spr->Data[y * Spr->Width + x];
				unsigned char* SA = ((unsigned char*)&ARGB) + 3;

				if (*SA == 0) { //If the pixel is fully transparent, skip to the next loop itteration
					continue;
				}
				else if (*SA == 255) { //If the pixel has no transparency, copy it into the destination
					((unsigned int*)Buffer.Memory)[((y - SrcY) + DstY) * Buffer.Width + ((x - SrcX) + DstX)] = ARGB;
				}
				else { //Otherwise blend it properly
					Blend(&ARGB, &((unsigned int*)Buffer.Memory)[((y - SrcY) + DstY) * Buffer.Width + ((x - SrcX) + DstX)]);
				}
			}
		}
		else { 
			//If the sprite has no transparency, or we are drawing without blending enabled, use memcpy to copy entire rows at once for speed
			memcpy((void*)&((unsigned int*)Buffer.Memory)[((y - SrcY) + DstY) * Buffer.Width + DstX], (void*)&(Spr->Data[y * Spr->Width]), Width * 4);
		}
	}
}

//Lighting code

//Constructor for the lightmap
LightMap::LightMap(int W, int H) {
	Width = W;
	Height = H;

	Buffer = (char*)VirtualAlloc(0, Width * Height, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	memset((void*)Buffer, 0, Width * Height);
};

LightMap::~LightMap(){
	VirtualFree(Buffer, Width * Height, MEM_RELEASE | MEM_FREE);
}

void RendererWin32Software::UpdateLighting() {
	//Draw a Light, Move to own function soon

	Light Lights[] = { 
		{ { 200, 100 }, 200 },
		{ { 600, 600 }, 400 }
	};

	for (int i = 0; i < RendererLightMap->Width * RendererLightMap->Height; i++) {
		RendererLightMap->Buffer[i] = 0;
	}

	for (auto Light : Lights) {
		//Center position of light and radius
		int CX, CY, R;

		CX = Light.Position.X;
		CY = Light.Position.Y;
		R = Light.Radius;

		int X1, X2, Y1, Y2; // Define a rectangle in which the light resides

		X1 = Light.Position.X - Light.Radius;
		Y1 = Light.Position.Y - Light.Radius;
		Y2 = Light.Position.Y + Light.Radius;
		X2 = Light.Position.X + Light.Radius;
		 
		for (int Y = Y1; Y < Y2; Y++) {
			for (int X = X1; X < X2; X++) {
				if (X >= 0 && X < Buffer.Width && Y >= 0 && Y < Buffer.Height) {
					//Use Pythagoras' Theorem to find the distance to the ceneter of the circle
					int DistX = CX - X;
					int DistY = CY - Y;
					long Distance = DistX * DistX + DistY * DistY;

					if (Distance < R * R) {
						char* LightLevel = RendererLightMap->Buffer + (Y * Buffer.Width + X);
						*LightLevel = MAX(*LightLevel, (char)(127 * (1 - ((float)Distance / R))));
					}
				}
			}
		}
	}
}

struct ApplyLightConfig{
	int From;
	int To;
	LightMap *Map;
	Win32ScreenBuffer *Buffer;
};

DWORD WINAPI ApplyLightingThread(void* Config) {
	int From = ((ApplyLightConfig*)Config)->From;
	int To = ((ApplyLightConfig*)Config)->To;
	LightMap *RendererLightMap = ((ApplyLightConfig*)Config)->Map;
	Win32ScreenBuffer *Buffer = ((ApplyLightConfig*)Config)->Buffer;

	for (int i = From; i <= To; i++) {
		char *LightLevel = RendererLightMap->Buffer + i;
		if (LightLevel != 0) {
			int *Pixel = Buffer->Memory + i;
			Lighten(Pixel, *LightLevel);
		}
	}
	return 0;
}

void RendererWin32Software::ApplyLighting() {
	const int ThreadCount = 1;
	int PixelsPerThread = (Buffer.Width * Buffer.Height) / ThreadCount;

	static ApplyLightConfig* Configs[ThreadCount];
	static HANDLE Threads[ThreadCount];
	static DWORD ThreadIDs[ThreadCount];

	for (int i = 0; i < ThreadCount; i++) {

		if (!Configs[i]) {
			int From = PixelsPerThread * i;
			int To = (PixelsPerThread * i) + (PixelsPerThread - 1);

			Configs[i] = (ApplyLightConfig*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ApplyLightConfig));

			Configs[i]->From = From;
			Configs[i]->To = To;
			Configs[i]->Buffer = &Buffer;
			Configs[i]->Map = RendererLightMap;
		}

		Threads[i] = CreateThread(0, 0, ApplyLightingThread, (void*)(Configs[i]), 0, &ThreadIDs[i]);
	}

	WaitForMultipleObjects(ThreadCount, Threads, TRUE, INFINITE);
	
	for (int i = 0; i < ThreadCount; i++) {
		//HeapFree(GetProcessHeap(), 0, Configs[i]);
	}
}