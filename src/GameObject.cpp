
#include "GameObject.h"
#include "MemoryManager.h"
#include "Renderer.h"

GameObject::GameObject() {
	this->Position = { 0, 0 };
	this->Velocity = { 0, 0 };
}

GameObject::GameObject(Vector2 Position, Vector2 Velocity) {
	this->Position = Position;
	this->Velocity = Velocity;
}

GameObject::GameObject(Vector2 Position){
	this->Position = Position;
	this->Velocity = { 0, 0 };
}

void GameObject::LoadSprite(AssetFile AssetFile, int id){
	//Create the sprite if it doesn't exist
	if (!_Sprite) {
		_Sprite = MemoryManager::AllocateMemory<Sprite>();
	}
	_Sprite->Load(AssetFile, id);
}

void GameObject::ApplyForce(Vector2 Force) {
	Velocity = Velocity + Force;
}

void GameObject::Update(double DeltaTime, std::vector<iRect> CollisionGeometry){
	//The Hitbox of the object
	Rect Hitbox = { Position.X, Position.Y, (float)_Sprite->Width, (float)_Sprite->Height };

	//Apply gravity 
	if (isGrounded) {
		TargetVelocity.Y = 0;
	}
	else {
		TargetVelocity.Y += 2000;
		if (TargetVelocity.Y > 10000) {
			TargetVelocity.Y = 10000;
		}
	}

	IVec2 Direction = { Sign(TargetVelocity.X - Velocity.X), Sign(TargetVelocity.Y - Velocity.Y) };

	Velocity.Y = Velocity.Y + (Direction.Y * 1000) * static_cast<float>(DeltaTime);
	Velocity.X = Velocity.X + (Direction.X * (isGrounded ? 10000 : 1500)) * static_cast<float>(DeltaTime);

	if (Sign(TargetVelocity.X - Velocity.X) != Direction.X) {
		Velocity.X = TargetVelocity.X;
	}

	if (Sign(TargetVelocity.Y - Velocity.Y) != Direction.Y) {
		Velocity.Y = TargetVelocity.Y;
	}

	//Deltaposition is the ideal amount we should move
	Vector2 DeltaPosition;
	DeltaPosition = Velocity * DeltaTime;

	//This is the broad phase hitbox
	//If an object isn't in here, it will not collide with the object.
	Rect BroadPhase = GetBroadphaseRect(Hitbox, DeltaPosition);

	isGrounded = false;
	canJump = false;

	//For each Box in the level, first do a broad phase collision test (to see if we are near enough we could hit)
	//Then do a more precise collision test to see weather we did.
	//If we did hit we should bounce off
	for (auto Box : CollisionGeometry) {
		if (CheckCollisionAABB(BroadPhase, (iRect)Box)) {
			//This is the normal of the object we hit, if any
			Vector2 Normal;

			float CollisionTime = CheckCollisionSweptAABB(Hitbox, Box, DeltaPosition, Normal);
			DeltaPosition *= CollisionTime;

			if (CollisionTime < 100.0 * DeltaTime) {
				if (abs(Normal.X) > 0){
					wallJumpDirection = -Sign(Velocity.X);

					canJump = true;
				}
			}

			if (CollisionTime < 1.0) {
				if (abs(Normal.X) > 0){
					wallJumpDirection = -Sign(Velocity.X);

					Velocity.X = 0;
					DeltaPosition.X = DeltaPosition.X * CollisionTime;
					canJump = true;
				}
				if (abs(Normal.Y) > 0) {
					DeltaPosition.Y = DeltaPosition.Y * CollisionTime;
					Velocity.Y = 0;
					isGrounded = true;
					canJump = true;
				}
			}

		}
	}

	Position = Position + DeltaPosition;
}

void GameObject::Draw(Renderer* renderer) {
	renderer->DrawSprite(_Sprite, 0, 0, _Sprite->Width, _Sprite->Height, (int)(Position.X), (int)(Position.Y), true);
}