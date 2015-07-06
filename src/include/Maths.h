#pragma once

#include <limits>
#include <Types.h>

#define MAX(A, B) (A > B ? A : B)
#define MIN(A, B) (A < B ? A : B)
#define SQR(A) (A*A)

///<summary>
///Function to compute the absolute value of a two's complement integer
///See: http://en.wikipedia.org/wiki/Two%27s_complement
///</summary>
///<param name="x">A signed integer</param>
inline int Abs(int x) {
	return x > 0 ? x : ~x + 1;
}

inline float Abs(float x) {
	return x > 0 ? x : -x;
}


inline int Sign(int X) {
	return X > 0 ? 1 : -1;
}

inline int Sign(float X) {
	return X > 0 ? 1 : -1;
}

//Checks for collisions between two axis aligned bounding boxes
inline bool CheckCollisionAABB(Rect a, Rect b){
	if (a.X < b.X + b.W &&
		a.X + a.W > b.X &&
		a.Y < b.Y + b.H &&
		a.H + a.Y > b.Y){
		return true;
	}
	return false;
}

//Checks for collisions using the swept axis align bounding box algorithm
//See http://www.gamedev.net/page/resources/_/technical/game-programming/swept-aabb-collision-detection-and-response-r3084
//Returns T, a value which tells us how long untill a collision occurs.
//If no collision will occur in the next frame, T will be greater than 1 or less than 0
inline float CheckCollisionSweptAABB(Rect A, Rect B, Vector2 Velocity, Vector2& Normal) {
	//Find out the distance to the near and far sides of each object
	float xInvEntry, yInvEntry;
	float xInvExit, yInvExit;

	if (Velocity.X > 0.0f) {
		xInvEntry = B.X - (A.X + A.W);
		xInvExit = (B.X + B.W) - A.X;
	}
	else{
		xInvEntry = (B.X + B.W) - A.X;
		xInvExit = B.X - (A.X + A.W);
	}

	if (Velocity.Y > 0.0f) {
		yInvEntry = B.Y - (A.Y + A.H);
		yInvExit = (B.Y + B.H) - A.Y;
	}
	else{
		yInvEntry = (B.Y + B.H) - A.Y;
		yInvExit = B.Y - (A.Y + A.H);
	}

	//Find out how long it will take to get to the object in a fraction of movetime
	float xEntry, yEntry;
	float xExit, yExit;

	if (Velocity.X == 0.0f){
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else {
		xEntry = xInvEntry / Velocity.X;
		xExit = xInvExit / Velocity.X;
	}
	
	if (Velocity.Y == 0.0f) {
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else {
		yEntry = yInvEntry / Velocity.Y;
		yExit = yInvExit / Velocity.Y;
	}

	float entryTime = std::fmax(xEntry, yEntry);
	float exitTime = std::fmax(xExit, yExit);

	//If there is no collision
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f) {
		Normal.X = 0.0f;
		Normal.Y = 0.0f;
		return 1.0f;
	}
	else {
		if (xEntry > yEntry) {
			//Collided with X face

			if (xInvEntry < 0.0f) {
				Normal.X = 1.0f;
				Normal.Y = 0.0f;
			}
			else {
				Normal.X = -1.0f;
				Normal.Y = 0.0f;
			}
		}
		else {
			//Collided with Y face
			if (yInvEntry < 0.0f) {
				Normal.X = 0.0f;
				Normal.Y = 1.0f;
			}
			else {
				Normal.X = 0.0f;
				Normal.Y = -1.0f;
			}
		}
	}
	return entryTime;
}

inline Rect GetBroadphaseRect(Rect A, Vector2 V)
{
	Rect BroadRect;
	BroadRect.X = V.X > 0 ? A.X : A.X + V.X;
	BroadRect.Y = V.Y > 0 ? A.Y : A.Y + V.Y;
	BroadRect.W = V.X > 0 ? A.X + A.W : A.W - V.X;
	BroadRect.H = V.Y > 0 ? V.Y + A.H : A.H - V.Y;

	return BroadRect;
}

inline Rect GetBroadphaseRect(iRect A, Vector2 V)
{
	Rect BroadRect;
	BroadRect.X = V.X > 0 ? A.X : A.X + V.X;
	BroadRect.Y = V.Y > 0 ? A.Y : A.Y + V.Y;
	BroadRect.W = V.X > 0 ? A.X + A.W : A.W - V.X;
	BroadRect.H = V.Y > 0 ? V.Y + A.H : A.H - V.Y;

	return BroadRect;
}