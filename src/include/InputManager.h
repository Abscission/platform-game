
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
	//Game
	KB_Attack,
	KB_AttackSecondary,
	KB_Inventory,
	//
	KB_Interact,
	KB_Pause,
	//Level Editer
	KB_Editor,
	KB_EditorAndPause,
	KB_EditorPause,
	KB_CycleForward,
	KB_CycleBackward,
	KB_ToggleCollision,
	KB_ToggleObject,
	KB_CollistionDebug,
	KB_LineOfSightDebug,

	KB_Length
};

static const char* BindingNames[] {
	//Movement
	"left",
	"right",
	"jump",
	"walk",
	"stop",
	//Game
	"attack",
	"attack_secondary",
	"inventory",
	"interact",
	"pause",
	//Level Editer
	"editor_toggle",
	"editor_toggle_and_pause"
	"editor_pause"
	"editor_cycle_forward",
	"editor_cycle_backward",
	"editor_object_toggle",
	"editor_collision_toggle",
	"editor_collision_debug",
	"editor_line_of_sight_debug"
};

static const char* DefaultBindings[] {
	//Movement
	"a",
	"d",
	"space",
	"shift",
	nullptr,
	//Game
	"left_mouse",
	"right_mouse",
	"i",
	"e",
	"escape",
	//Level Editer
	"f2",
	"f3",
	"p",
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
	bool GetKeyUp(Binding KB);
	bool GetKeyDown(Binding KB);
};

extern KeyBinds Bindings;