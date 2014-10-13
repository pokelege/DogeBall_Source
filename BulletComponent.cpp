#include "BulletComponent.h"
#include <Misc\Clock.h>
#include <Core\GameObject.h>
#include "Life.h"
void BulletComponent::attatch( GameObject* parent )
{
	this->parent = parent;
}
void BulletComponent::detatch()
{
	parent = 0;
}

void BulletComponent::earlyUpdate() {}
void BulletComponent::update()
{
	if ( !parent ) return;
	parent->translate += speed * direction;

	currentLifeTime -= Clock::dt;
	if ( currentLifeTime <= 0 ) parent->active = false;
}
void BulletComponent::lateUpdate()
{
	if ( !parent || !target ) return;
	float distance = glm::length( parent->translate - target->translate );
	if ( distance <= range )
	{
		Life* life = parent->getComponent<Life>();
		if ( life )
		{
			life->changeLife( -damage );
		}
	}
}
void BulletComponent::earlyDraw() {}
void BulletComponent::draw() {}
void BulletComponent::lateDraw() {}

void BulletComponent::fire( glm::vec3& position , glm::vec3& direction )
{
	parent->translate = position;
	this->direction = direction;
	parent->active = true;
}