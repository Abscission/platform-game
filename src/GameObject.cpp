
#include "Renderer.h"
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

void GameObject::loadTexture(const char *filename) {
	//Create the sprite if it doesn't exist
	if (!Sprite) {
		//TODO: Some kind of factory to abstract this platform specific code
		Sprite = new Win32Sprite();
	}
	Sprite->Load(filename);
}

void GameObject::ApplyForce(Vector2 Force) {
	Velocity = Velocity + Force;
}

void GameObject::Update(double DeltaTime) {
	//The Hitbox of the object
	Rect Hitbox = { Position, { Sprite->Width, Sprite->Height } };

	//Apply gravity 
	Velocity.Y += 980 * DeltaTime;


	Acceleration *= (0.9 * DeltaTime);
	Velocity = Velocity + (Acceleration * DeltaTime);

	//Deltaposition is the ideal amount we should move
	Vector2 DeltaPosition;
	DeltaPosition = Velocity * DeltaTime;

	//This is the broad phase hitbox
	//If an object isn't in here, it will not collide with the object.
	Rect BroadPhase = GetBroadphaseRect(Hitbox, DeltaPosition);

	Rect Level[] = { { 0, 300, 220, 10 }, { 200, 500, 300, 10 }, { 490, 450, 250, 10 }, { 400, 0, 10, 500 }, { 0, 600 - 25, 800, 16 } };

	//For each Box in the level, first do a broad phase collision test (to see if we are near enough we could hit)
	//Then do a more precise collision test to see weather we did.
	//If we did hit we should bounce off
	for (auto Box : Level) {
		if (CheckCollisionAABB(BroadPhase, Box)) {
			//This is the normal of the object we hit, if any
			Vector2 Normal;

			float CollisionTime = CheckCollisionSweptAABB(Hitbox, Box, DeltaPosition, Normal);
			DeltaPosition *= CollisionTime;

			if (CollisionTime < 1.0) {
				DeltaPosition *= CollisionTime;

				if (abs(Normal.X) > 0.000001f){
					DeltaPosition.X = -DeltaPosition.X;
					Velocity.X = -0.2 * Velocity.X;
				}
				if (abs(Normal.Y) > 0.000001f) {
					DeltaPosition.Y = -DeltaPosition.Y;
					Velocity.Y = -0.2 * Velocity.Y;

					if (Velocity.Y > -5.f) {
						if (!isGrounded) {
							isGrounded = true;
							Velocity.Y = 0;
						}
						Velocity.X *= (0.99);
					}
				}
			}

		}
	}

	Velocity.X *= (0.99);

	Position = Position + DeltaPosition;
}

void GameObject::Draw(Renderer* renderer) {
	renderer->DrawSprite(Sprite, 0, 0, Sprite->Width, Sprite->Height, (int)(Position.X), (int)(Position.Y), true);
}