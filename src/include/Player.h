
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include "GameObject.h"
#include "Config.h"
#include "LogManager.h"

class Player : public GameObject {
private:
	double JumpTime = 0;
	char Left = 'A'; 
	char Right = 'D';
	char Jump = VK_SPACE;
	void GetBindings();
public:
	Player();
	void Update(double DeltaTime, std::vector<iRect> Collision);

};