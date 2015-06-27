// InputManager.h
// This is the header file that provides an input interface to the game
// Author: Daniel Alexander

#pragma once

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

