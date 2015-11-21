
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

	///<summary>Find out if a key has just been pressed</summary>
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
	///<summary>Default: a</summary>
	KB_Left,
	///<summary>Default: d</summary>
	KB_Right,
	///<summary>Default: space</summary>
	KB_Jump,
	///<summary>Default: shift</summary>
	KB_Walk,
	///<summary>Default: unbound</summary>
	KB_Stop,
	//Game
	///<summary>Default: left click</summary>
	KB_Attack,
	///<summary>Default: right click</summary>
	KB_AttackSecondary,
	///<summary>Default: i</summary>
	KB_Inventory,
	///<summary>Default: e</summary>
	KB_Interact,
	///<summary>Default: escape</summary>
	KB_Pause,
	//Level Editer
	///<summary>Default: f2</summary>
	KB_Editor,
	///<summary>Default: f3</summary>
	KB_EditorAndPause,
	///<summary>Default: p</summary>
	KB_EditorPause,
	///<summary>Default: e</summary>
	KB_CycleForward,
	///<summary>Default: q</summary>
	KB_CycleBackward,
	///<summary>Default: o</summary>
	KB_ToggleCollision,
	///<summary>Default: tab</summary>
	KB_ToggleObject,
	///<summary>Default: c</summary>
	KB_CollisionDebug,
	///<summary>Default: z</summary>
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

	///<summary>Find out the state of a key</summary>
	///<param name="KB">The key to test</param>
	///<returns>A boolean with the keys state</returns>
	bool GetKey(Binding KB);
	///<summary>Find out if a key has just been released</summary>
	///<param name="KB">The key to test</param>
	bool GetKeyUp(Binding KB);
	///<summary>Find out if a key has just been pressed</summary>
	///<param name="KB">The key to test</param>
	bool GetKeyDown(Binding KB);
};

extern KeyBinds Bindings;
