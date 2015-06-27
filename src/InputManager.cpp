#include<Windows.h>
#include <Xinput.h>

#include"InputManager.h"

#include "Utility.h"

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

MouseState InputManager::GetMouseState(HWND Window){
	POINT CursorPos;
	//Mouse cursor pos
	GetCursorPos(&CursorPos);
	ScreenToClient(Window, &CursorPos);

	//Are left or right mouse buttons beening pressed
	bool btn1 = GetKeyState(VK_LBUTTON);
	bool btn2 = GetKeyState(VK_RBUTTON);

	return { CursorPos.x, CursorPos.y, btn1, btn2 };
}

bool InputManager::IsControllerConnected() {
	XINPUT_STATE ControllerState;
	DWORD Success = XInputGetState(1, &ControllerState);

	DisplayMessage(Success);

	return Success == ERROR_SUCCESS;
}
