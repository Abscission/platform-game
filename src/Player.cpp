
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Player.h"
#include <cstdlib>
#include "InputManager.h"
#include "Config.h"

Player::Player() {
	GetBindings();
}

void Player::GetBindings(){
	ConfigFile ControlsFile("config/bindings.ini");
	
	Controls.Left = atoi(ControlsFile.Get("Left", (char*)0x41).c_str());
	Controls.Right = atoi(ControlsFile.Get("Right", (char*)0x44).c_str());
	//Controls.Jump = atoi(ControlsFile.Get("Jump", (char*)0x20).c_str());
	//Controls.Shift = atoi(ControlsFile.Get("Shift", (char*)0xA0).c_str());
}

void Player::Update(double DeltaTime, std::vector<iRect> Collision) {

	//InputManager InputManager::Get() = InputManager::Get();
	ControllerState Controller = InputManager::Get().GetControllerState();
	

	int MaxSpeed = (InputManager::Get().GetKeyState(/*Controls.Shift*/VK_SHIFT) || Controller.Buttons & 0x2000) ? 400 : 600;

	if (InputManager::Get().GetKeyState(Controls.Right) || Controller.Buttons & 0x8) {
		TargetVelocity.X = static_cast<float>(MaxSpeed);
	}
	else if (InputManager::Get().GetKeyState(Controls.Left) || Controller.Buttons & 0x4) {
		TargetVelocity.X = -static_cast<float>(MaxSpeed);
	}
	else {
		TargetVelocity.X = (MaxSpeed / 32767.f) * Controller.LeftStick.X;
	}

	if (JumpTime == 0) {
		if (InputManager::Get().GetKeyState(/*Controls.Jump*/VK_SPACE) || Controller.Buttons & 0x1000) {
			if (canJump) {
				if (!isGrounded) {
					//walljump
					Velocity.X = static_cast<float>(600 * wallJumpDirection);
				}

				Velocity.Y = -600;
				JumpTime += DeltaTime;
			}
		}
	}

	if (JumpTime > 0){
		if (InputManager::Get().GetKeyState(/*Controls.Jump*/VK_SPACE) || Controller.Buttons & 0x1000) {
			JumpTime += DeltaTime;
			if (JumpTime < 1.0f) {
				Velocity.Y -= 500 * static_cast<float>(DeltaTime);
			}
		}
		else {
			JumpTime = 0;
		}
	}

	//Respawn
	if (Position.Y > 2000) {
		Position = { 50, -100 };
		Velocity = { 0, 0 };
	}

	GameObject::Update(DeltaTime, Collision);
}