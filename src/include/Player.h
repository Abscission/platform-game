
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include "GameObject.h"
#include "Config.h"

class Player : public GameObject {
private:
	double JumpTime = 0;
public:
	s64 Score;
	u32 Color;
	bool Dead = false;
	virtual void LoadSprite(AssetFile AssetFile, int id);
	virtual void Update(double DeltaTime) final;
	void UpdateColor();

};