#pragma once 

#include "maths.h"
#include "Renderer.h"

class GameObject {
protected:


public:
	bool isGrounded = false;

	Vector2 Position;
	Vector2 Velocity;
	Vector2 Acceleration;

	Vector2 TargetVelocity;

	//Constructors for GameObject
	GameObject();
	GameObject(Vector2 position);
	GameObject(Vector2 position, Vector2 velocity);

	float Mass;

	///<summary>
	///Loads a texture from a file, and assigns it to the GameObject
	///</summary>
	///<param name="filename">The filename of the image to load</param>
	void loadTexture(const char * filename);

	Sprite* Sprite;

	void ApplyForce(Vector2 Force);

	///<summary>
	///Advance a GameObjects state to a new point in time
	///</summary>
	///<param name="DeltaTime">The amount of time to advance in seconds typically the time taken to render the last frame</param>
	virtual void Update(double DeltaTime);

	void Draw(Renderer*);
};