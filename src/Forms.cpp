
//Copyright (c) 2015 Jason Light
//License: MIT

#include <Forms.h>
#include <Utility.h>
#include <InputManager.h>

Form::Form(int W, int H, class Renderer * R) {
	Position = { R->Config.RenderResX / 2 - W / 2, R->Config.RenderResY / 2 - H / 2, W, H };
	Color = rgba(0x33, 0x33, 0x33, 128);
}

void Form::Update(HWND Window) {

	if (Enabled) {
		auto MS = InputManager::Get().GetMouseState(Window);

		for (auto Control : Controls) {
			Control->Update(this);
		}

		if (MS.Btn1) {
			if (MS.x > Position.X && MS.x < Position.X + Position.W && MS.y > Position.Y && MS.y < Position.Y + Position.H) {
				for (auto C : Controls) {
					if (MS.x > Position.X + C->Pos.X && MS.x < Position.X + C->Pos.X + C->Pos.W && MS.y > Position.Y + C->Pos.Y && MS.y < Position.Y + C->Pos.Y + C->Pos.H) {
						for (auto D : Controls) {
							D->isActive = false;
						}
						C->isActive = true;
						C->OnClick(C->Arg);
					}
				}
			}
		}
	}
}

void Form::Render(Renderer* R) {
	if (Enabled) {
		R->DrawRectangleBlend(Position.X, Position.Y, Position.W, Position.H, Color);

		for (auto Control : Controls) {
			Control->Draw(R, this);
		}
	}
}

void Label::SetFont(Font * font){
	this->font = font;
	Pos = font->GetStringRect(Pos.X, Pos.Y, Text.c_str());
}

Label::Label(int x, int y, std::string text) {
	Pos.X = x;
	Pos.Y = y;
	Text = text;
}

void Label::Draw(Renderer* R, Form* F) {
	font->RenderString(R, F->Position.X + Pos.X, F->Position.Y + Pos.Y, Text.c_str());
}

void TextBox::SetFont(Font * font) {
	this->font = font;
}

TextBox::TextBox(int x, int y, int w, int h, std::string* text) {
	Pos = { x, y, w, h };
	Text = text;
}

void TextBox::Update(Form * F) {
	if (isActive) {
		*Text += InputManager::Get().GetTypedText();
		if (InputManager::Get().GetKeyDown(VK_BACK)) {
			if (Text->length() > 0)
				Text->pop_back();
		}
	}
}

void TextBox::Draw(Renderer * R, Form * F) {
	R->DrawRectangle(F->Position.X + Pos.X, F->Position.Y + Pos.Y, Pos.W, Pos.H, 0xffffff);
	int xoffset = font->RenderString(R, F->Position.X + Pos.X + 10, F->Position.Y + Pos.Y, Text->c_str());

	if (isActive) {
		if ((GetTickCount() / (1000 / 2)) % 2 == 0)
			R->DrawRectangle(F->Position.X + Pos.X + xoffset + 10, F->Position.Y + Pos.Y + 2, 2, 32, 0x000000);
	}
}

Button::Button(int x, int y, Font* f, std::string* text, void_func callback)
{
	font = f;
	ClickHandler = callback;
	Pos.X = x;
	Pos.Y = y;
	Pos.W = font->GetStringRect(Pos.X, Pos.Y, text->c_str()).W + 10;
	Pos.H = 32 + 10;
	Text = text;
}

void Button::Draw(Renderer * R, Form * F) {

	R->DrawRectangle(F->Position.X + Pos.X, F->Position.Y + Pos.Y, Pos.W, Pos.H, 0xffffff);
	font->RenderString(R, F->Position.X + Pos.X + 5, F->Position.Y + Pos.Y + 5, Text->c_str());
}

void Button::OnClick(void* A)
{
	this->ClickHandler(A);
}
