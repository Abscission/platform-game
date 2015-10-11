
//Copyright (c) 2015 Jason Light
//License: MIT

#include <Forms.h>
#include <Utility.h>
#include <InputManager.h>
#include <GameLayer.h>

Form::Form(int W, int H) {
	Position = { G.renderer->Config.RenderResX / 2 - W / 2, G.renderer->Config.RenderResY / 2 - H / 2, W, H };
	Color = rgba(0x33, 0x33, 0x33, 128);
}

Form::Form(int X, int Y, int W, int H) {
	if (X < 0) X = G.renderer->Config.RenderResX + X;
	if (Y < 0) Y = G.renderer->Config.RenderResY + Y;

	Position = { X, Y, W, H };
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

void Form::Render() {
	if (Enabled) {
		G.renderer->DrawRectangleBlend(Position.X, Position.Y, Position.W, Position.H, Color);

		for (auto Control : Controls) {
			Control->Draw(this);
		}
	}
}

void Label::SetFont(Font * font){
	this->font = font;
	iRect StringRect = font->GetStringRect(Pos.X, Pos.Y, Text.c_str(), TextSize);
	Pos.W = (int)StringRect.W;
	Pos.H = (int)StringRect.H;
}

Label::Label(int x, int y, std::string text, int size, Font* font) {
	Pos.X = x;
	Pos.Y = y;

	this->font = font;
	this->TextSize = size;
	Text = text;

	iRect StringRect = font->GetStringRect(x, y, Text.c_str(), size);
	Pos.W = (int)StringRect.W;
	Pos.H = (int)StringRect.H;

}

void Label::Draw(Form* F) {
	font->RenderString(F->Position.X + Pos.X, F->Position.Y + Pos.Y, Text.c_str(), TextSize, Color);
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

void TextBox::Draw( Form * F) {
	G.renderer->DrawRectangle(F->Position.X + Pos.X, F->Position.Y + Pos.Y, Pos.W, Pos.H, Background);
	int xoffset = font->RenderString(F->Position.X + Pos.X + 10, F->Position.Y + Pos.Y, Text->c_str(), TextSize, Color).W;

	if (isActive) {
		if ((GetTickCount() / (1000 / 2)) % 2 == 0)
			G.renderer->DrawRectangle(F->Position.X + Pos.X + xoffset + 10, F->Position.Y + Pos.Y + 2, 2, 32, Color);
	}
}

Button::Button(int x, int y, Font* f, std::string* text, void_func callback) {
	font = f;
	ClickHandler = callback;
	Pos.X = x;
	Pos.Y = y;
	Pos.W = font->GetStringRect(Pos.X, Pos.Y, text->c_str()).W + 10;
	Pos.H = 32 + 10;
	Text = text;
}

void Button::Draw(Form * F) {
	G.renderer->DrawRectangle(F->Position.X + Pos.X, F->Position.Y + Pos.Y, Pos.W, Pos.H, 0xffffff);
	font->RenderString(F->Position.X + Pos.X + 5, F->Position.Y + Pos.Y + 5, Text->c_str());
}

void Button::OnClick(void* A) {
	this->ClickHandler(A);
}
