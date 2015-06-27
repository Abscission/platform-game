// InputManager.h
// This is the header file that provides an input interface to the game
// Author: Daniel Alexander

#pragma once

//The InputManager class provides the game with keyboard and mouse input
class InputManager {
private:
	unsigned char KeyStateCurrent[255];
	unsigned char KeyStatePrevious[255];
public:
	//This should be run once per frame to update the keyboard states
	void Update();

	///<summary>Fnd out the state of a key</summary>
	///<param name="Key">The key to test</param>
	///<returns>A boolean with the keys state</returns>
	bool GetKeyState(unsigned char Key);

	///<summary>Fnd out if a key has just been released</summary>
	///<param name="Key">The key to test</param>
	bool GetKeyDown(unsigned char Key);

	///<summary>Fnd out if a key has just been released</summary>
	///<param name="Key">The key to test</param>
	bool GetKeyUp(unsigned char Key);
};