
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include <Types.h>
#include <Renderer.h>
#include <Font.h>
#include <string>
#include <vector>
#include "GameLayer.h"

typedef void void_func(void*);

class Form;

class Control {
public:
	iRect Pos;

	bool isActive;
	virtual void Draw(Form* F) = 0;
	virtual void Update(Form* F) = 0;
	virtual void OnClick(void*) = 0;

	void* Arg = 0;
};

class Label : public Control {
private:
	Font* font;

public:
	void SetFont(Font* font);

	u32 TextSize;
	u32 Color;

	Label(int x, int y, std::string text, int size=36, Font* font=G.font);
	
	virtual void Draw(Form* F) final;

	void Update(Form* F) {};
	void OnClick(void*) {};

	std::string Text;
};

class Button : public Control {
public:
	std::string* Text;
	Font* font;

	u32 Background;
	u32 Color;
	u32 TextSize;

	Button(int x, int y, Font* font, std::string* t, void_func callback);
	virtual void Draw(Form* F) final;
	void Update(Form* F) {};
	virtual void OnClick(void*) final;
	void_func* ClickHandler;

};

class TextBox : public Control {
private: 
	Font* font;
public:
	u32 Background = 0xffffffff;
	u32 Color;
	u32 TextSize;

	void SetFont(Font* font);
	std::string* Text;

	TextBox(int x, int y, int w, int h, std::string* text);
	virtual void Draw(Form* F) final;

	void Update(Form* F);
	void OnClick(void*) { isActive = true; };

};

class Form {
public:
	bool Enabled;

	iRect Position;
	u32 Color;

	std::vector<Control*> Controls;

	Form(int W, int H);
	Form(int X, int Y, int W, int H);

	void Update(HWND Window);
	void Render();
};