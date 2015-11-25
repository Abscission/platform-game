
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
	///<summary>Move the player to the left</summary>
	KB_Left,
	///<summary>Default: d</summary>
	///<summary>Move the player to the right</summary>
	KB_Right,
	///<summary>Default: space</summary>
	///<summary>Make the player jump</summary>
	KB_Jump,
	///<summary>Default: shift</summary>
	///<summary>Slow the player to a walk</summary>
	KB_Walk,
	//Game
	///<summary>Default: left click</summary>
	///<summary>Player primary attack</summary>
	KB_Attack,
	///<summary>Default: right click</summary>
	///<summary>Player secondary attack</summary>
	KB_AttackSecondary,
	///<summary>Default: i</summary>
	///<summary>Open the players inventory</summary>
	KB_Inventory,
	///<summary>Default: e</summary>
	///<summary>Interact with a player chosen object</summary>
	KB_Interact,
	///<summary>Default: escape</summary>
	///<summary>Pause the game</summary>
	KB_Pause,
	//Level Editer
	///<summary>Default: f2</summary>
	///<summary>Toggle to open the editer</summary>
	KB_Editor,
	///<summary>Default: f3</summary>
	///<summary>Toggle to open the editer and pause/ close editer and unpause</summary>
	KB_EditorAndPause,
	///<summary>Default: p</summary>
	///<summary>Pause the game in the editor</summary>
	KB_EditorPause,
	///<summary>Default: e</summary>
	///<summary>Cycle the selected texure forwards</summary>
	KB_CycleForward,
	///<summary>Default: q</summary>
	///<summary>Cycle the selected texure backwards</summary>
	KB_CycleBackward,
	///<summary>Default: o</summary>
	///<summary>Toggle if the objects are being placeds</summary>
	KB_ToggleObject,
	///<summary>Default: tab</summary>
	///<summary>Toggle if the placed texure has collision</summary>
	KB_ToggleCollision,
	///<summary>Default: c</summary>
	///<summary>Toggle to show the collision debug overlay</summary>
	KB_CollisionDebug,
	///<summary>Default: z</summary>
	///<summary>Toggle the line of sight debug overlay</summary>
	KB_LineOfSightDebug,
	///<summary>Default: left arrow</summary>
	///<summary>Pan the camera to the left</summary>
	KB_PanLeft,
	///<summary>Default: right arrow</summary>
	///<summary>Pan the camera to the right</summary>
	KB_PanRight,
	///<summary>Default: up arrow</summary>
	///<summary>Pan the camera up</summary>
	KB_PanUp,
	///<summary>Default: down arrow</summary>
	///<summary>Pan the camera down</summary>
	KB_PanDown,

	KB_Length
};

static const char* BindingNames[] {
	//Movement
	"left",
	"right",
	"jump",
	"walk",
	//Game
	"attack",
	"attack_secondary",
	"inventory",
	"interact",
	"pause",
	//Level Editer
	"editor_toggle",
	"editor_toggle_and_pause",
	"editor_pause",
	"editor_cycle_forward",
	"editor_cycle_backward",
	"editor_object_toggle",
	"editor_collision_toggle",
	"editor_collision_debug",
	"editor_line_of_sight_debug",
	"editor_pan_left",
	"editor_pan_right",
	"editor_pan_up",
	"editor_pan_down"
};

static const char* DefaultBindings[] {
	//Movement
	"a",
	"d",
	"space",
	"shift",
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
	"z",
	"left",
	"right",
	"up",
	"down"
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
