#pragma once
#include "GameObject.h"

class Player : public GameObject {
private:
	double JumpTime = 0;
public:
	void Update(double DeltaTime);
};