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
	Player();
	void Update(double DeltaTime, std::vector<iRect> Collision);

};