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
public:
	AI_State State;
	virtual void Update(double DeltaTime) final;
};