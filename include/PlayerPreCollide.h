#pragma once
#include <Physics\PreCollideEvent.h>

class PlayerPreCollide : public PreCollideEvent
{
public:
	virtual bool preCollideEvent( Particle* particle );
};