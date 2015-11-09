
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Renderer.h"
#include <Windows.h>

#include "Utility.h"
#include "MemoryManager.h"
#include "AssetManager.h"
#include "Config.h"
#include "GameLayer.h"

#include <intrin.h>

bool ShouldClose = false;

static int MonitorCount;
static Rect Monitors[8];

//The window callback, this is what processes messages from our windows
LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam) {
	//We handle a couple of events
	switch (Message) {

	//If we see a close event, close the window
	case WM_CLOSE:
		DestroyWindow(Window);
		ShouldClose = true;
		return 0;

	//Otherwise, have the Default Window Proceedure deal with it
	default:
		return DefWindowProc(Window, Message, wParam, lParam);
	}
}

//A function to blend two RGB Pixels. It assumes premultiplied alpha!
inline void Blend(unsigned int* Source, unsigned int* Dest) {
	//Get the source channels
	rgba_color* S = (rgba_color*)Source;
	rgba_color* D = (rgba_color*)Dest;

	//Set the destination color based on the source and destination channel
	D->r = S->r + ((D->r * (256 - S->a)) >> 8);
	D->g = S->g + ((D->g * (256 - S->a)) >> 8);
	D->b = S->b + ((D->b * (256 - S->a)) >> 8);
}

//A function to change the luminance value of an RGB pixel
u32 Lighten(u32 Color, float amount) {
	rgba_color color(Color);
	color.r = (u8)MIN(MAX(0, color.r * amount), 255);
	color.g = (u8)MIN(MAX(0, color.g * amount), 255);
	color.b = (u8)MIN(MAX(0, color.b * amount), 255);
	return color.color;
}

//Resize a sprite based on width, maintaining the aspect ratio
bool ResizeSprite(Sprite* Sprite, int W) {
	for (int i = 0; i <= Sprite->NumberOfFrames; i++)
		if (!ResizeSprite(&Sprite->Frames[i], W)) return false;

	Sprite->Width = Sprite->Frames->Width;
	Sprite->Height = Sprite->Frames->Height;

	return true;
}

//Resize a sprite frame based on width, maintaining the aspect ratio
bool ResizeSprite(_Sprite* Sprite, int W) {
	assert(W > 0, "Tried to resize sprite to zero width");
	assert(Sprite->Width != 0, "Tried to resize invalid sprite");

	return ResizeSprite(Sprite, W, Sprite->Height * (W / Sprite->Width));
}

//Resize a sprite frame with Width and Height
bool ResizeSprite(Sprite* Sprite, int W, int H) {
	for (int i = 0; i < Sprite->NumberOfFrames; i++)
		if (!ResizeSprite(&Sprite->Frames[i], W, H)) return false;

	Sprite->Width = Sprite->Frames->Width;
	Sprite->Height = Sprite->Frames->Height;

	return true;
}

//A function to resize a sprite frame using nearest neighbour
bool ResizeSprite(_Sprite* Sprite, int W, int H){
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
	if (Sprite->Data) VirtualFree(Sprite->Data, 0, MEM_RELEASE);
	
	//Set the sprite data to the new buffer
	Sprite->Data = TempBuffer;

	//Set up the sprites members according to the change
	Sprite->Width = W;
	Sprite->Height = H;
	Sprite->length = W * H * 4;

	return true;
}

//Function to load a sprite from an asset file
bool _Sprite::Load(AssetFile AssetFile, int id){
	//Get the asset from the file
	Asset BMP = AssetFile.GetAsset(id);
	
	//Get pointers to the info we need
	byte* Memory = (byte*)BMP.Memory;
	ImageHeader* Header = (ImageHeader*)Memory;
	
	if (Memory == nullptr || Header->Width == 0 || Header->Height == 0 || Header->length == 0) {
		const u32 InvalidSpriteColor = rgba(255, 0, 128, 255);
		this->Data = (u32*)malloc(32*32*4);

		this->Width = 32;
		this->Height = 32;
		this->length = 32 * 32 * 4;

		for (int i = 0; i < 32 * 32; i++) {
			Data[i] = InvalidSpriteColor;
		}

		return false;
	}

	//Get the info out of the header
	this->Width = Header->Width;
	this->Height = Header->Height;
	this->length = Header->length;
	this->hasTransparency = Header->HasTransparency != 0;
	
	//Allocate and memcpy the data
	this->Data = (u32*)VirtualAlloc(0, Header->length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	memcpy((void*)this->Data, (void*)(Memory + sizeof(ImageHeader)), Header->length);
	
	return true;
}

//When a sprite is deleted, delete its data
_Sprite::~_Sprite() {
	if (this->Data) VirtualFree(this->Data, 0, MEM_RELEASE);
}

//Function to open the window
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

	//Set the window style depending on weather or not the game is fullscreen
	DWORD WindowStyle = Config.Fullscreen ? (WS_POPUP | WS_VISIBLE) : (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_VISIBLE);
	
	//Set up the ClientRect
	RECT WindowRect = { 0, 0, Width, Height };
	AdjustWindowRect(&WindowRect, WindowStyle, false);

	//Create the Window
	this->Window = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, "JasonWindowClassName", Title, WindowStyle, 0, 0, WindowRect.right, WindowRect.bottom, 0, 0, Instance, 0);
	return true;
}

//A struct to store monitors from the EnumDisplayMonitors function
struct MonitorEnumResult {
	HMONITOR Monitors[8];
	int Primary = 0;
	int Count = 0;
};

//This callback will be called once for each monitor attached to the PC, each monitor is assed to the list, and the primary and count values are edited as required
BOOL CALLBACK MonitorEnumProc(HMONITOR Monitor, HDC DeviceContext, LPRECT Rect, LPARAM Data) {
	MonitorEnumResult* Monitors = (MonitorEnumResult*)Data;
	Monitors->Monitors[Monitors->Count++] = Monitor;

	MONITORINFO MonitorInfo;
	MonitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(Monitor, &MonitorInfo);

	if (MonitorInfo.dwFlags & MONITORINFOF_PRIMARY) Monitors->Primary = Monitors->Count - 1;
	return true;
}

//Functions to set the camera position, self explanatory.
void Renderer::SetCameraPosition(int X, int Y) {
	this->CameraPos = { X, Y };
}

void Renderer::SetCameraPosition(IVec2 Position) {
	this->CameraPos = Position;
}


bool Renderer::Initialize() {
	Buffer = new Win32ScreenBuffer;

	//Center the camera
	SetCameraPosition({ 0, 0 });

	//Get the graphics config file
	ConfigFile GraphicsConfig("config/Graphics.ini");

	//Get the "Fullscreen" key, default to 1
	std::string sFullscreen = GraphicsConfig.Get("Fullscreen", "1");
	Config.Fullscreen = sFullscreen != "0";

	//Get the Resolution
	std::string sRenderResX = GraphicsConfig.Get("RenderResolutionX");
	std::string sRenderResY = GraphicsConfig.Get("RenderResolutionY");

	if (sRenderResX != "") {
		//If the resolution is set, then parse it
		Config.RenderResX = std::atoi(sRenderResX.c_str());
	}
	else {
		//Otherwise default to 1024 x 768 if windowed, or the resolution of the monitor if fullscreen
		Config.RenderResX = Config.Fullscreen ? 0 : 1024;
	}

	//Same for Y resolution
	if (sRenderResY != "") {
		Config.RenderResY = std::atoi(sRenderResY.c_str());
	}
	else {
		Config.RenderResY = Config.Fullscreen ? 0 : 768;
	}

	//If the game should be fullscreen
	if (Config.Fullscreen) {
		//Get the Monitor info
		MonitorEnumResult Monitors;
		EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&Monitors);

		//Read which monitor to use from the config, default to the primary
		int Monitor = std::atoi(GraphicsConfig.Get("Monitor", std::to_string(Monitors.Primary)).c_str());

		//Get the monitornifo for the selected monitor
		MONITORINFO MonitorInfo;
		MonitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(Monitors.Monitors[Monitor], &MonitorInfo);

		//Set the windowres to the monitor resolution
		Config.WindowResY = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;
		Config.WindowResX = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;

		//If the render res isn't set, set it to the window res
		if (Config.RenderResX == 0) Config.RenderResX = Config.WindowResX;
		if (Config.RenderResY == 0) Config.RenderResY = Config.WindowResY;
	}
	else {
		//If the game is windowed, then make the window the same size as the render resolution
		Config.WindowResX = Config.RenderResX;
		Config.WindowResY = Config.RenderResY;
	}

	//Save the resolution determined
	GraphicsConfig.Set("RenderResolutionX", std::to_string(Config.RenderResX));
	GraphicsConfig.Set("RenderResolutionY", std::to_string(Config.RenderResY));

	//Set up the bytes per pixel value
	Config.BPP = 4;

	//Open the Window, and set the Device context and Module Handle
	this->OpenWindow(Config.WindowResX, Config.WindowResY, "Title");
	this->DeviceContext = GetWindowDC(this->Window);
	this->Instance = GetModuleHandle(NULL);

	SetProcessDPIAware(); //true
	HDC screen = GetDC(NULL);
	Config.DPI_X = GetDeviceCaps(screen, LOGPIXELSX);
	Config.DPI_Y = GetDeviceCaps(screen, LOGPIXELSY);
	ReleaseDC(NULL, screen);

	//Hide the cursor
	while (ShowCursor(false) >= 0);

	//Create a DIB to render to by:

	//Setting up the Buffer object
	Buffer->Width = Config.RenderResX;
	Buffer->Height = Config.RenderResY;
	Buffer->BytesPerPixel = 4;

	//Including the BITMAPINFO struct
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;
	Buffer->Info.bmiHeader.biClrImportant = 0;
	Buffer->Info.bmiHeader.biClrUsed = 0;
	Buffer->Info.bmiHeader.biSizeImage = Buffer->Width * Buffer->Height * 4;

	Buffer->Info.bmiColors->rgbRed = 0;
	Buffer->Info.bmiColors->rgbGreen = 0;
	Buffer->Info.bmiColors->rgbBlue = 0;
	Buffer->Info.bmiColors->rgbReserved = 0;

	//CreateDIBSection allocates memory, and gives us a handle we can render
	hbmp = CreateDIBSection(DeviceContext, &Buffer->Info, DIB_RGB_COLORS, (void**)&Buffer->Memory, NULL, 0);

	//Set up a memory DC for the bitmap
	HdcMem = CreateCompatibleDC(DeviceContext);
	SelectObject(HdcMem, hbmp);

	//this is the clear color
	u32 color = rgba(99, 148, 255, 255);


	if (InstructionSet::AVX()) {
		//If the CPU supports AVX instructions, we will clear 8 colors (256 bits) at a time,
		//fill up a value we can use for this with the colors
		u32* c = (u32*)&clearval;
		for (int i = 0; i < 8; i++)
			c[i] = color;
		}
	else if (InstructionSet::SSE()) {
		//Otherwise we will use SSE for the same thing, but we can only fill 4 colors (128 bits) at a time
		u32* c = (u32*)&clearval_sse;
		for (int i = 0; i < 4; i++)
			c[i] = color;
	}

	return 0;
}

bool Renderer::Refresh() {
	//Now update the screen
	//Blit to the screen

	BitBlt(DeviceContext, 0, 0, Buffer->Width, Buffer->Height, HdcMem, 0, 0, SRCCOPY);
		
	if (InstructionSet::AVX()) {
		//Use AVX instrictions to loop over the buffer, filling it with our color, 256 bits at a time
		uP len = (Buffer->Width * Buffer->Height) / 8;

		for (register uP i = 0; i < len; i++)
			_mm256_stream_si256(&((__m256i*)Buffer->Memory)[i], clearval);
	}
	else if (InstructionSet::SSE()) {
		//Use SSE instrictions to loop over the buffer, filling it with our color, 128 bits at a time
		uP len = (Buffer->Width * Buffer->Height) / 4;
		for (register uP i = 0; i < len; i++)
			_mm_stream_si128(&((__m128i*)Buffer->Memory)[i], clearval_sse);
	}
	else {
		//If the CPU is ancient, just fill it 32 bits at a time (SSE runs on Pentium III, the game probably won't run on any hardware that needs this
		//But it is here just in case! It may serve a role if alternate archutectures are supported in the future.
		DrawRectangle(0, 0, Config.RenderResX, Config.RenderResY, rgba(102, 102, 204, 255));
	}

	return !ShouldClose;
}

//Here are a bunch of functions to draw rectangles with varying arguments, some WS, some SS, some blend, some don't
void Renderer::DrawRectangle(int X, int Y, int Width, int Height, unsigned int Color) {

	if (X < 0) {
		Width += X;
		X = 0;
		if (Width <= 0) return;
	}

	if (Y < 0) {
		Height += Y;
		Y = 0;
		if (Height <= 0) return;
	}

	if (X + Width > Buffer->Width) {
		Width = Buffer->Width - X;
		X = Buffer->Width - Width;

		if (Width <= 0) return;
	}

	if (Y + Height > Buffer->Height) {
		Height = Buffer->Height - Y;
		Y = Buffer->Height - Height;

		if (Height <= 0) return;
	}

	for (int y = 0; y < Height; y++) {
		int Down = Buffer->Width * (Y + y);
		for (int x = 0; x < Width; x++) {
			((unsigned int*)Buffer->Memory)[Down + (X + x)] = Color;
		}
	}
}

void Renderer::DrawRectangleBlend(int X, int Y, int Width, int Height, unsigned int Color) {

	if (X < 0) {
		Width += X;
		X = 0;
		if (Width <= 0) return;
	}

	if (Y < 0) {
		Height += Y;
		Y = 0;
		if (Height <= 0) return;
	}

	if (X + Width > Buffer->Width) {
		Width = Buffer->Width - X;
		X = Buffer->Width - Width;

		if (Width <= 0) return;
	}

	if (Y + Height > Buffer->Height) {
		Height = Buffer->Height - Y;
		Y = Buffer->Height - Height;

		if (Height <= 0) return;
	}

	for (int y = 0; y < Height; y++){
		int Down = Buffer->Width * (Y + y);
		for (int x = 0; x < Width; x++) {
			Blend(&Color, (unsigned int*)(Buffer->Memory + Down + (X + x)));
		}
	}
}

void Renderer::DrawRectangleWS(int X, int Y, int Width, int Height, unsigned int Color) {
	X -= CameraPos.X;
	Y -= CameraPos.Y;

	DrawRectangle(X, Y, Width, Height, Color);
}

void Renderer::DrawRectangleBlendWS(int X, int Y, int Width, int Height, unsigned int Color) {

	X -= CameraPos.X;
	Y -= CameraPos.Y;

	DrawRectangleBlend(X, Y, Width, Height, Color);

}

void Renderer::DrawGlyph(FT_Bitmap* Glyph, int X, int Y, u32 C) {
	rgba_color Color(C);
	for (register u64 y = 0; y < Glyph->rows; y++) {
		u64 row = Y + y;
		for (u64 x = 0; x < Glyph->width; x++) {
			u64 col = X + x;
			if (col < 0 || row < 0 || col >= Config.RenderResX || row >= Config.RenderResY) continue;

			uint8_t a = Glyph->buffer[y * Glyph->pitch + x];
			u32 c = rgba(Color.r, Color.g, Color.b, a);
			Blend(&c, &Buffer->Memory[col + row * Buffer->Width]);
		}
	}
}

void Renderer::DrawSpriteRectangle(int X, int Y, int Width, int Height, _Sprite* Spr) {
	for (int y = Y; y < Y + Height; y+= Spr->Width){
		int Down = Buffer->Width * (Y + y);
		for (int x = X; x < X + Width; x += Spr->Width) {
			x -= CameraPos.X;
			y -= CameraPos.Y;

			DrawSprite(Spr, 0, 0, Spr->Width, Spr->Height, x, y);
		}
	}
}


//Function overloads for DrawSprite
void Renderer::DrawSprite(Sprite* Spr, int X, int Y) {
	DrawSprite(Spr, 0, 0, Spr->Width, Spr->Height, X, Y, true);
}

void Renderer::DrawSprite(_Sprite* Spr, int X, int Y) {
	DrawSprite(Spr, 0, 0, Spr->Width, Spr->Height, X, Y, true);
}

void Renderer::DrawSprite(_Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY){
	DrawSprite(Spr, SrcX, SrcY, Width, Height, DstX, DstY, true);
}

void Renderer::DrawSprite(_Sprite* Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool ShouldBlend){
	//Camera transform, then screenspace draw

	DstX -= CameraPos.X;
	DstY -= CameraPos.Y;

	DrawSpriteSS(Spr, SrcX, SrcY, Width, Height, DstX, DstY, ShouldBlend);
}

void Renderer::DrawSprite(Sprite * Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend) {
	int Frame;

	if (Spr->isAnimated) {
		Frame = ((GetTickCount() - Spr->CreationTime) / (Spr->Period / Spr->NumberOfFrames)) % Spr->NumberOfFrames;
	}
	else {
		Frame = 0;
	}
	DrawSprite(Spr->Frames + Frame, SrcX, SrcY, Width, Height, DstX, DstY, Blend);
}


void Renderer::DrawSpriteSS(Sprite * Spr, int X, int Y) {
	DrawSpriteSS(Spr, 0, 0, Spr->Width, Spr->Height, X, Y, true);
}

void Renderer::DrawSpriteSS(Sprite * Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool Blend) {
	int Frame;

	if (Spr->isAnimated) {
		Frame = ((GetTickCount() - Spr->CreationTime) / (Spr->Period / Spr->NumberOfFrames)) % Spr->NumberOfFrames;
	}
	else {
		Frame = 0;
	}
	DrawSpriteSS(Spr->Frames + Frame, SrcX, SrcY, Width, Height, DstX, DstY, Blend);
}

void Renderer::DrawSpriteSS(_Sprite * Spr, int X, int Y) {
	DrawSpriteSS(Spr, 0, 0, Spr->Width, Spr->Height, X, Y, true);
}

void Renderer::DrawSpriteSS(_Sprite * Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY) {
	DrawSpriteSS(Spr, SrcX, SrcY, Width, Height, DstX, DstY, true);
}

void Renderer::DrawSpriteSS(_Sprite * Spr, int SrcX, int SrcY, int Width, int Height, int DstX, int DstY, bool ShouldBlend) {
	//Out of bounds checks
	//If the sprite is drawn partially offscreen, draw a section
	//If it is fully offscreen, don't draw it.

	if (DstX < 0) {
		SrcX -= DstX;
		Width += DstX;

		DstX = 0;
		if (Width <= 0) return;
	}

	if (DstY < 0) {
		SrcY -= DstY;
		Height += DstY;

		DstY = 0;
		if (Height <= 0) return;
	}

	if (DstX + Width > Buffer->Width) {
   		Width = Buffer->Width - DstX;
		if (Width <= 0) return;
	}

	if (DstY + Height > Buffer->Height) {
		Height = Buffer->Height - DstY;
		if (Height <= 0) return;
	}

	for (register int y = SrcY; y < (SrcY + Height); y++) {
		if (ShouldBlend && Spr->hasTransparency) {
			//If the pixel should be drawn with transparency itterate over each pixel
			for (register int x = SrcX; x < (SrcX + Width); x++) {
				unsigned int ARGB = Spr->Data[y * Spr->Width + x];
				unsigned char* SA = ((unsigned char*)&ARGB) + 3;

				if (*SA == 0) { //If the pixel is fully transparent, skip to the next loop itteration as no rendering is needed
					continue;
				}
				else if (*SA == 255) { //If the pixel has no transparency, copy it into the destination
					((u32*)Buffer->Memory)[((y - SrcY) + DstY) * Buffer->Width + ((x - SrcX) + DstX)] = ARGB;
				}
				else { //Otherwise blend it properly
					Blend(&ARGB, &((unsigned int*)Buffer->Memory)[((y - SrcY) + DstY) * Buffer->Width + ((x - SrcX) + DstX)]);
				}
			}
		}
		else {
			//If the sprite has no transparency, or we are drawing without blending enabled, use memcpy to copy entire rows at once for speed
			memcpy((void*)&((u32*)Buffer->Memory)[((y - SrcY) + DstY) * Buffer->Width + DstX], (void*)&Spr->Data[y * Spr->Width + SrcX], Width * 4);

		}
	}
}

//Function to load a new style static sprite from an assetfile
bool Sprite::Load(AssetFile Asset, int id) {
	Frames = MemoryManager::AllocateMemory<_Sprite>(1);

	bool Success = true;

	if (!Frames->Load(Asset, id)) Success = false;

	Width = Frames->Width;
	Height = Frames->Height;

	CurrentFrame = 0;
	NumberOfFrames = 0;
	isAnimated = false;

	return Success;
}

//Function to load an animated sprite from an asset file
bool Sprite::Load(AssetFile Asset, int start, int amount) {
	Frames = MemoryManager::AllocateMemory<_Sprite>(amount);

	for (int i = 0; i < amount; i++) {
		if (!Frames[i].Load(Asset, start + i)) return false;
	}

	Width = Frames->Width;
	Height = Frames->Height;

	CurrentFrame = 0;

	if (amount == 1) {
		NumberOfFrames = 0;
		isAnimated = false;
	}
	else {
		isAnimated = true;
		NumberOfFrames = amount;
		CreationTime = GetTickCount();
	}

	return true;
}
