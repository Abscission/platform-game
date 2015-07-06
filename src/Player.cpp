#include "Player.h"
#include "InputManager.h"

void Player::Update(double DeltaTime, std::vector<iRect> Collision) {

	//InputManager InputManager::Get() = InputManager::Get();
	ControllerState Controller = InputManager::Get().GetControllerState();
	

	int MaxSpeed = (InputManager::Get().GetKeyState(VK_SHIFT) || Controller.Buttons & 0x2000) ? 400 : 600;

	if (InputManager::Get().GetKeyState('D') || Controller.Buttons & 0x8) {
		TargetVelocity.X = MaxSpeed;
	}
	else if (InputManager::Get().GetKeyState('A') || Controller.Buttons & 0x4) {
		TargetVelocity.X = -MaxSpeed;
	}
	else {
		TargetVelocity.X = (MaxSpeed / 32767.f) * Controller.LeftStick.X;
	}

	if (JumpTime == 0) {
		if (InputManager::Get().GetKeyState(VK_SPACE) || Controller.Buttons & 0x1000) {
			if (canJump) {
				if (!isGrounded) {
					//walljump
					Velocity.X = 600 * wallJumpDirection;
				}

				Velocity.Y = -600;
				JumpTime += DeltaTime;
			}
		}
	}

	if (JumpTime > 0){
		if (InputManager::Get().GetKeyState(VK_SPACE) || Controller.Buttons & 0x1000) {
			JumpTime += DeltaTime;
			if (JumpTime < 1.0f) {
				Velocity.Y -= 500 * DeltaTime;
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