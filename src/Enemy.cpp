#include "Enemy.h"
#include "GameLayer.h"
#include "LogManager.h"
#include "Maths.h"

#include <math.h>


void Enemy::Update(double DeltaTime) {
	//some basic parameters, MaxSpeed is the maximum velocity of the enemy, while MaxDist is the distance it can see
	float MaxSpeed = 150.f;
	float MaxDist = 1000;

	//Basic raycasting algorithm, press z in edit mode to debug
	Vector2 Offset = {(G.player->Position.X + 32) - Position.X ,  (G.player->Position.Y) - Position.Y};
	Vector2 Dist = Offset;

	//Find the distance to the player
	float DistToPlayer = Dist.magnitude();

	//Normalize the offsets to get a 1 length vector in the direction of the player
	Offset = Offset / DistToPlayer;

	//The distance travelled along the "ray" is 0
	float Distance = 0;

	//Set a vector for the ray, starting from the enemies position
	Vector2 RayPos = Position;

	//Start by assuming the player is visible
	bool CanSeePlayer = true;

	//If the player is out of range we know straight away we cant see him
	if (DistToPlayer > MaxDist) {
		CanSeePlayer = false;
	}
	else {
		//Until we hit the player
		while (Distance < DistToPlayer) {
			//Get a pointer to the chunk the ray is in
			Chunk* C = G.level->GetChunk((u16)(RayPos.X / 32 / 16), (u16)(RayPos.Y / 32 / 16));

			//Find the local position within the chunk
			int LocalX = (int)RayPos.X / 32 - C->X * 16; 
			int LocalY = (int)RayPos.Y / 32 - C->Y * 16;

			//If we are debugging the Line of Sight, and in edit mode then plot the point
			if (G.debuglos && G.editing) G.renderer->DrawRectangleWS((u32)RayPos.X - 1, (u32)RayPos.Y - 1, 3, 3, State == AI_AGGRO ? 0xff0000 : 0xff00);

			//If we hit something then break and set canseeplayer to false
			if (C->Grid[ChunkLoc(LocalX, LocalY)].Collision) {
				CanSeePlayer = false;
				break;
			}

			//Add 16 to the distance, and the vector components
			Distance += 16;

			RayPos = RayPos + Offset * 16;

		}
	}


	switch (State) {
	case AI_ALERT:
		//If the ai can see the player, it should become aggressive

		if (CanSeePlayer) State = AI_AGGRO;

		TargetVelocity.X = 0;
		TargetVelocity.Y = 0;
	break;
	case AI_AGGRO:
	
		//If the AI is aggroed on the player

		if (GetTickCount() - bullet_time > 1000) {
			bullet_time = GetTickCount();

			GameObject* Bullet = new GameObject;
			Bullet->LoadSprite("Assets/assets.aaf", 0);
			G.level->SpawnEntity(Bullet, Position.X, Position.Y);
			Bullet->Velocity = Offset * 1000;
			Bullet->gravity = false;
		}
	
		//If the ai can no longer see the player, it should become alert
		if (!CanSeePlayer) {
			State = AI_ALERT;
		}

		//If the AI is far away from the player it should move toward it
		if (ABS(Dist.X) > 16 && isGrounded) {
			s8 Direction = Position.X < (G.player->Position.X + 16) ? 1 : -1;
			TargetVelocity.X = Direction * MaxSpeed;
		}
		else {
			//If it is very close it should stop
			TargetVelocity.X = 0;
		}

		if (CheckCollisionAABB({ Position.X, Position.Y, (float)Spr->Width, (float)Spr->Height }, { G.player->Position.X, G.player->Position.Y, (float)G.player->Spr->Width, (float)G.player->Spr->Height })) {
			G.player->Dead = true;
		}
	break;
	}

	//Update the base GameObject class to handle physics etc.
	GameObject::Update(DeltaTime);
}

void Bullet::Update(double DeltaTime) {
	
}
