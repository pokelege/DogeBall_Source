#pragma once
#include <Physics\CollisionEvent.h>

class BulletCollisionEvent : public CollisionEvent
{
public:
	virtual void collisionEvent( Particle* other = 0 );
};