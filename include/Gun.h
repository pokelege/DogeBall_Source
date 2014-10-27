#pragma once
#include <Core\Component.h>
#include <DebugMemory.h>
class BulletComponent;
class BulletCollisionEvent;
class Particle;
class Gun :public Component
{
	GameObject* bullet;
	
public:
	BulletComponent* bulletComponent;
	BulletCollisionEvent* collisionEvent;
	Particle* particle;
	unsigned int key;
	Gun(GameObject* target);
	~Gun();
	void earlyUpdate();
};