#pragma once
#include <Core\Component.h>
class BulletComponent;
class Gun :public Component
{
	GameObject* bullet;
	
public:
	BulletComponent* bulletComponent;
	unsigned int key;
	Gun();
	void earlyUpdate();
};