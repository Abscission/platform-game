
#include <Windows.h>
#include <vector>

#include "PicoPNG.h"

#include "Utility.h"

#include "RendererSoftware.h"
#include "MemoryManager.h"
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

bool ResizeSprite(Sprite* Sprite, int W) {
	return ResizeSprite(Sprite, W, Sprite->Height * (W / Sprite->Width));
}


//A function to resize a sprite using nearest neighbour
bool ResizeSprite(Sprite* Sprite, int W, int H){
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

bool Sprite::Load(const char * Filename) {
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

bool Sprite::Load(AssetManager::AssetFile AssetFile, int id){
	AssetManager::Asset BMP = AssetFile.GetAsset(id);
	this->Data = (unsigned int*)BMP.Memory;

	//TODO(Jason): Implement

	return true;
}

Sprite::~Sprite() {
	if (this->Data) VirtualFree(this->Data, this->Width * this->Height * 4, MEM_RELEASE);
}


bool Renderer::OpenWindow(int Width, int Height, char* Title){
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
	WindowStyle = WS_POPUP | WS_VISIBLE;
	
	RECT WindowRect = { 0, 0, Width, Height };
	AdjustWindowRect(&WindowRect, WindowStyle, false);

	this->Window = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, "JasonWindowClassName", Title, WindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, WindowRect.right, WindowRect.bottom, 0, 0, Instance, 0);
	return true;
}

bool Renderer::Initialize() {
	Config.ResX = 1680;
	Config.ResY = 1050;
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

	Buffer.Memory = (int*)MemoryManager::AllocateMemoryAligned(MemorySize, 16);

	return 0;
}

bool Renderer::Refresh() {
	//Now update the screen
	StretchDIBits(DeviceContext, 0, 0, Buffer.Width, Buffer.Height, 0, 0, Buffer.Width, Buffer.Height, Buffer.Memory, &Buffer.Info, DIB_RGB_COLORS, SRCCOPY);

	//Clear the backbuffer
	ZeroMemory(Buffer.Memory, Buffer.Width * Buffer.Height * Buffer.BytesPerPixel);

	return !ShouldClose;
}

void Renderer::DrawRectangle(int X, int Y, int Width, int Height, int Color) {
	for (int y = 0; y < Height; y++){
		int Down = Buffer.Width * (Y + y);
		for (int x = 0; x < Width; x++) {
			((int*)Buffer.Memory)[Down + (X + x)] = Color;
		}
	}
}


void Renderer::DrawSpriteRectangle(int X, int Y, int Width, int Height, Sprite* Spr) {
	for (int y = Y; y < Y + Height; y+= Spr->Width){
		int Down = Buffer.Width * (Y + y);
		for (int x = X; x < X + Width; x += Spr->Width) {
			DrawSprite(Spr, x, y);
		}
	}
}

void Renderer::DrawSprite(Sprite* Spr, int X, int Y) {
	DrawSprite(Spr, 0, 0, Spr->Width, Spr->Height, X, Y, true);
}

void Renderer::DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY){
	DrawSprite(Spr, SrcX, SrcY, Width, Height, DstX, DstY, true);
}

void Renderer::DrawSprite(Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool ShouldBlend){
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