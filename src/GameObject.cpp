
//Copyright (c) 2015 Jason Light
//License: MIT

#include "GameObject.h"
#include "MemoryManager.h"
#include "Renderer.h"

#include "GameLayer.h"

GameObject::GameObject() {
	this->Position = { 0, 0 };
	this->Velocity = { 0, 0 };

	this->SpawnPosition = { 0, 0 };
}

GameObject::GameObject(Vector2 Position, Vector2 Velocity) {
	this->Position = Position;
	this->Velocity = Velocity;

	this->SpawnPosition = Position;
}

GameObject::GameObject(Vector2 Position){
	this->Position = Position;
	this->Velocity = { 0, 0 };
	
	this->SpawnPosition = Position;
}

void GameObject::LoadSprite(AssetFile AssetFile, int id){
	//Create the sprite if it doesn't exist
	if (!Spr) {
		Spr = MemoryManager::AllocateMemory<Sprite>(1);
	}
	Spr->Load(AssetFile, id);
}

void GameObject::ApplyForce(Vector2 Force) {
	Velocity = Velocity + Force;
}

void GameObject::Update(double DeltaTime) {
	//The Hitbox of the object
	Rect Hitbox = { Position.X, Position.Y, (float)Spr->Width, (float)Spr->Height };

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

	//Reset these flags
	isGrounded = false;
	canJump = false;

	//For each Box in the area, first do a broad phase collision test (to see if we are near enough we could hit)
	//Then do a more precise collision test to see weather we did.
	//If we did hit we should bounce off

	//Find the current chunk. The offsets represent the nine surrounding chunks.
	IVec2 CurrentChunk = {Position.X / 32 / 16, Position.Y / 32 / 16};
	const IVec2 offsets[] = { {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0}, {1, 0}, {-1, 1}, {0, 1 }, {1, 1} };

	//Range based for loop to iterate over the offset array
	for (auto offset : offsets) {

		//Find the chunk at the current offset unless it is out of bounds, in which case skip it
		IVec2 chunkPos = CurrentChunk - offset;
		if (chunkPos.X < 0 || chunkPos.Y < 0 || chunkPos.X > UINT16_MAX || chunkPos.Y > UINT16_MAX) continue;
		Chunk* C = G.level->GetChunk((u16)chunkPos.X, (u16)chunkPos.Y);

		//Foe each box in the chunk
		for (auto Box : *C->Geometry) {
			if (CheckCollisionAABB(BroadPhase, Box)) {
				//This is the normal of the object we hit, if any
				Vector2 Normal;

				float CollisionTime = CheckCollisionSweptAABB(Hitbox, Box, DeltaPosition, Normal);
				DeltaPosition *= CollisionTime;

				if (CollisionTime < 1.0) {
					if (abs(Normal.X) > 0) {
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
	}

	Position = Position + DeltaPosition;
}

void GameObject::Draw(Renderer* renderer) {
	renderer->DrawSprite(Spr, 0, 0, Spr->Width, Spr->Height, (int)(Position.X), (int)(Position.Y), true);
}

Pickup::Pickup() {
	//Load a default pickup sprite
	AssetFile SpriteAsset = AssetFile("assets/Assets.aaf");
	LoadSprite(SpriteAsset, 1);
}

void Pickup::Update(double DeltaTime) {
	timer += DeltaTime;
	
	this->OffsetY = 16 * cos(timer * 4) - 8;

	//Check if we have been picked up
	if (CheckCollisionAABB({ Position.X, Position.Y, (float)Spr->Width, (float)Spr->Height }, { G.player->Position.X, G.player->Position.Y, (float)G.player->Spr->Width, (float)G.player->Spr->Height })) {
		G.player->Score += 200;
		destroy = true;
	}
}

void Pickup::Draw(Renderer* renderer) {
	renderer->DrawSprite(Spr, Position.X, Position.Y + OffsetY);
}