
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once 

#include <vector>
#include "Maths.h"
#include "AssetManager.h"
#include "Renderer.h"

class GameObject {
protected:
	bool isGrounded = false;
	bool canJump = false;
	int wallJumpDirection;

	Vector2 Velocity;
	Vector2 Acceleration;

	Vector2 TargetVelocity;
public:
	Vector2 Position;


	//Constructors for GameObject
	GameObject();
	GameObject(Vector2 position);
	GameObject(Vector2 position, Vector2 velocity);

	float Mass;

	///<summary>
	///Loads a sprite from a file, and assigns it to the GameObject
	///</summary>
	///<param name="AssetFile">The AAF file from which to load the image</param>
	///<param name="id">The index of the asset from which to get the texture</param>
	void LoadSprite(AssetFile AssetFile, int id);


	Sprite* Spr;
	
	void ApplyForce(Vector2 Force);

	///<summary>
	///Advance a GameObjects state to a new point in time
	///</summary>
	///<param name="DeltaTime">The amount of time to advance in seconds typically the time taken to render the last frame</param>
	virtual void Update(double DeltaTime, std::vector<iRect> CollisionGeometry);

	void Draw(Renderer*);
};