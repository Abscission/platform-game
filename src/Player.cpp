
//Copyright (c) 2015 Jason Light
//License: MIT

#include "Player.h"
#include <cstdlib>
#include "InputManager.h"
#include "Config.h"
#include "Utility.h"
#include "GameLayer.h"

Player::Player() {
	GetBindings();
}

void Player::LoadSprite(AssetFile AssetFile, int id) {
	GameObject::LoadSprite(AssetFile, id);
	//UpdateColor();
}

//Get key bindings from a config file
void Player::GetBindings() {
	ConfigFile ControlsFile("config/bindings.ini");
	
	char shift [2] = { VK_SHIFT, '\0' };

	Controls.Left = *ControlsFile.Get("Left", "A").c_str();
	Controls.Right = *ControlsFile.Get("Right", "D").c_str();
	//Controls.Jump = atoi(ControlsFile.Get("Jump", " ").c_str());
	//Controls.Shift = atoi(ControlsFile.Get("Shift", shift ).c_str());
}

void Player::UpdateColor() {
	for (register uP c = 0; c < Spr->Width * Spr->Height; c++) {
		if (Spr->Frames->Data[c] == rgba(255, 255, 0, 255)) {
			Spr->Frames->Data[c] = Color;
		}
	}
}

void Player::Update(double DeltaTime) {
	ControllerState Controller = InputManager::Get().GetControllerState();
	
	int MaxSpeed = (InputManager::Get().GetKeyState(Controls.Shift) || Controller.Buttons & 0x2000) ? 400 : 600;

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
		if (InputManager::Get().GetKeyState(Controls.Jump) || Controller.Buttons & 0x1000) {
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
		if (InputManager::Get().GetKeyState(Controls.Jump) || Controller.Buttons & 0x1000) {
			JumpTime += DeltaTime;
			if (JumpTime < 1.0f) {
				Velocity.Y -= 500 * static_cast<float>(DeltaTime);
			}
		}
		else {
			JumpTime = 0;
		}
	}

	if (InputManager::Get().GetKeyDown('E')) {
		for (auto Entity : G.level->Entities) {
			if (CheckCollisionAABB({ Position.X, Position.Y, (float)Spr->Width, (float)Spr->Height }, { Entity->Position.X, Entity->Position.Y, (float)Entity->Spr->Width, (float)Entity->Spr->Height })) Entity->OnInteract();
		}
	}

	//Respawn
	if (Position.Y > 20000) {
		Position = SpawnPosition;
		Velocity = { 0, 0 };
	}

	//Update the base GameObject
	GameObject::Update(DeltaTime);
}