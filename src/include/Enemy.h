#pragma once

#include "Types.h"
#include "GameObject.h"
#include "Level.h"

enum AI_State {
	AI_ALERT = 0,
	AI_AGGRO
};

class Enemy : public GameObject {
private:
	double bullet_time;
public:
	AI_State State;
	virtual void Update(double DeltaTime) final;
};

class Bullet : public GameObject {
	virtual void Update(double DeltaTime) final;
};