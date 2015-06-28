
#include "RendererSoftware.h"
#include "GameObject.h"

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

void GameObject::loadTexture(AssetManager::AssetFile AssetFile, int id){
	//Create the sprite if it doesn't exist
	if (!_Sprite) {
		//TODO: Some kind of factory to abstract this platform specific code
		_Sprite = new Sprite();
	}
	_Sprite->Load(AssetFile, id);
}


void GameObject::loadTexture(const char *filename) {
	//Create the sprite if it doesn't exist
	if (!_Sprite) {
		//TODO: Some kind of factory to abstract this platform specific code
		_Sprite = new Sprite();
	}
	_Sprite->Load(filename);
}

void GameObject::ApplyForce(Vector2 Force) {
	Velocity = Velocity + Force;
}

void GameObject::Update(double DeltaTime) {
	//The Hitbox of the object
	Rect Hitbox = { Position, { _Sprite->Width, _Sprite->Height } };

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

	Vector2 Direction = Vector2{ Sign(TargetVelocity.X - Velocity.X), Sign(TargetVelocity.Y - Velocity.Y) };

	Velocity.Y = Velocity.Y + (Direction.Y * 1000) * DeltaTime;
	Velocity.X = Velocity.X + (Direction.X * (isGrounded ? 10000 : 1500)) * DeltaTime;

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

	Rect Level[] = { { 0, 300, 224, 64 }, { 320, 512, 320, 64 }, { 512, 0, 64, 512 }, { 0, 768 - 32, 1024, 64 } };

	isGrounded = false;
	canJump = false;

	//For each Box in the level, first do a broad phase collision test (to see if we are near enough we could hit)
	//Then do a more precise collision test to see weather we did.
	//If we did hit we should bounce off
	for (auto Box : Level) {
		if (CheckCollisionAABB(BroadPhase, Box)) {
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