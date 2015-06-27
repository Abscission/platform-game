#include<Windows.h>
#include"InputManager.h"


void InputManager::Update(){
	memcpy(KeyStatePrevious, KeyStateCurrent, 255); //Copy previous frames keystate into KeyStatePrevious 
	GetKeyboardState(this->KeyStateCurrent);
}

bool InputManager::GetKeyState(unsigned char Key){
	return (KeyStateCurrent[Key] & 0xF0);
}

bool InputManager::GetKeyDown(unsigned char Key){
	return (KeyStateCurrent[Key] & 0xF0) &! (KeyStatePrevious[Key] & 0xF0);
}

bool InputManager::GetKeyUp(unsigned char Key){
	return ((KeyStatePrevious[Key] & 0xF0) &! (KeyStateCurrent[Key] & 0xF0));
} 