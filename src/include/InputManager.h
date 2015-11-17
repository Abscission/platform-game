
//Copyright (c) 2015 Jason Light
//License: MIT

// InputManager.h
// This is the header file that provides an input interface to the game
// Author: Daniel Alexander

#pragma once

#include <string>
#include <Windows.h>
#include "Types.h"

//Mouse state struct provides the data structure containing mouse related variables 
struct MouseState {
	int x;
	int y;
	bool Btn1;
	bool Btn2;
};

//Structure for accessing game controllers
struct ControllerState {
	V2_16 LeftStick;
	V2_16 RightStick;

	byte LeftTrigger;
	byte RightTrigger;

	u16  Buttons;
};


//The InputManager class provides the game with keyboard and mouse input
class InputManager {
private:
	byte KeyStateCurrent[255];
	byte KeyStatePrevious[255];
public:

	static InputManager& Get(){
		static InputManager IM;
		return IM;
	}


	std::string GetTypedText();

	//This should be run once per frame to update the keyboard states
	void Update();

	///<summary>Find out the state of a key</summary>
	///<param name="Key">The key to test</param>
	///<returns>A boolean with the keys state</returns>
	bool GetKeyState(unsigned char Key);

	///<summary>Find out if a key has just been released</summary>
	///<param name="Key">The key to test</param>
	bool GetKeyDown(unsigned char Key);

	///<summary>Find out if a key has just been released</summary>
	///<param name="Key">The key to test</param>
	bool GetKeyUp(unsigned char Key);
	
	bool IsControllerConnected();
	
	ControllerState GetControllerState();

	///<summary>Find out where the mouse cursor is and if a mouse button is pressed</summary>
	MouseState GetMouseState(HWND Window);
};

enum Binding {
	//Movement
	KB_Left,
	KB_Right,
	KB_Jump,
	KB_Walk,
	KB_Stop,
	//Level Editer
	KB_Editor,
	KB_CycleForward,
	KB_CycleBackward,
	KB_ToggleCollision,
	KB_ToggleObject,
	KB_CollistionDebug,
	KB_LineOfSightDebug,

	KB_Length
};

const char* BindingNames[] {
	//Movement
	"left",
	"right",
	"jump",
	"walk",
	"stop",
	//Level Editer
	"editer_toggle",
	"editer_cycle_forward",
	"editer_cycle_backward",
	"editer_object_toggle",
	"editer_collision_toggle",
	"editer_collision_debug",
	"editer_line_of_sight_debug"
};

const char* DefaultBindings[] {
	//Movement
	"a",
	"d",
	"space",
	"shift",
	nullptr,
	//Level Editer
	"f2",
	"e",
	"q",
	"o",
	"tab",
	"c",
	"z"
};

class KeyBinds {
private:
	char PrimaryBindings[KB_Length];
	char AltBindings[KB_Length];

public:
	KeyBinds();

	bool GetKey(Binding KB);
};