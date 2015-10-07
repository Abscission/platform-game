
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include "GameObject.h"
#include "Config.h"

struct Controls {
	char Left = 'A';
	char Right = 'D';
	char Jump = VK_SPACE;
	char Shift = VK_SHIFT;
};

class Player : public GameObject {
private:
	double JumpTime = 0;
	Controls Controls;
	void GetBindings();
public:
	bool Dead = false;
	s64 Score;
	Player();
	virtual void Update(double DeltaTime) final;

};