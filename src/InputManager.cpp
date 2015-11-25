
//Copyright (c) 2015 Jason Light
//License: MIT

#include "InputManager.h"
#include <Windows.h>
#include <Xinput.h>
#include "Config.h"
#include "Utility.h"

void InputManager::Update(){
	memcpy(&KeyStatePrevious[0], &KeyStateCurrent[0], 255); //Copy previous frames keystate into KeyStatePrevious 
	GetKeyboardState(this->KeyStateCurrent);
}

bool InputManager::GetKeyState(unsigned char Key){
	return (KeyStateCurrent[Key] & 0xF0) != 0;  //If the key is down
}

bool InputManager::GetKeyDown(unsigned char Key){
	return ((KeyStateCurrent[Key] & 0xF0) != 0) &! ((KeyStatePrevious[Key] & 0xF0) != 0); //If the key is down, but wasn't last frame
}

bool InputManager::GetKeyUp(unsigned char Key){
	return ((KeyStatePrevious[Key] & 0xF0) &! (KeyStateCurrent[Key] & 0xF0)); //If the key is up, but wasn't last frame
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
	return XInputGetState(0, &ControllerState) == ERROR_SUCCESS;
}

ControllerState InputManager::GetControllerState(){
	XINPUT_STATE ControllerState;
	if (XInputGetState(0, &ControllerState) == ERROR_SUCCESS) {
		return{
			{ ControllerState.Gamepad.sThumbLX, ControllerState.Gamepad.sThumbLY },
			{ 0, 0 },
			ControllerState.Gamepad.bLeftTrigger,
			ControllerState.Gamepad.bRightTrigger,
			ControllerState.Gamepad.wButtons
		};
	}
	else {
		return{};
	}
}

std::string InputManager::GetTypedText() {
	const char Keys[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\\', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', VK_OEM_COMMA, VK_OEM_PERIOD, '/', ' ' };
	const char Keys_Lower[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ' };
	const char Keys_Upper[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', ' ' };

	std::string ret = "";

	for (int i = 0; i < 47; i++) {
		if (InputManager::GetKeyDown(Keys[i])) {
			ret += (InputManager::GetKeyState(VK_SHIFT) ? Keys_Upper[i] : Keys_Lower[i]);
		}
	}

	return ret;
}

KeyBinds::KeyBinds(){
	ConfigFile ControlsFile("config/bindings.ini");

	for (int i = 0; i < KB_Length; i++) {
		if (DefaultBindings[i] != nullptr)
			PrimaryBindings[i] = get_vk_code(ControlsFile.Get(BindingNames[i], DefaultBindings[i]));
		else
			PrimaryBindings[i] = get_vk_code(ControlsFile.Get(BindingNames[i]));

		char AltBindName[256];
		sprintf_s(AltBindName, "alt_%s", BindingNames[i]);
		AltBindings[i] = get_vk_code(ControlsFile.Get(AltBindName));
	}
}

bool KeyBinds::GetKey(Binding KB){
	InputManager& IM = InputManager::Get();
	return IM.GetKeyState(PrimaryBindings[KB]) || IM.GetKeyState(AltBindings[KB]);
}

bool KeyBinds::GetKeyUp(Binding KB){
	InputManager& IM = InputManager::Get();
	return IM.GetKeyUp(PrimaryBindings[KB]) || IM.GetKeyUp(AltBindings[KB]);
}

bool KeyBinds::GetKeyDown(Binding KB){
	InputManager& IM = InputManager::Get();
	return IM.GetKeyDown(PrimaryBindings[KB]) || IM.GetKeyDown(AltBindings[KB]);
}

KeyBinds Bindings;