#include <BulletComponent.h>
#include <Misc\Clock.h>
#include <Core\GameObject.h>
#include <Life.h>
#include <GlobalItems.h>
#include <Physics\Particle.h>
#include <DebugMemory.h>
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
	currentLifeTime -= Clock::dt;
	if ( currentLifeTime <= 0 )
	{
		parent->active = false;
		parent->getComponent<Particle>()->velocity = glm::vec3();
	}
}
void BulletComponent::lateUpdate()
{
}
void BulletComponent::earlyDraw() {}
void BulletComponent::draw() {}
void BulletComponent::lateDraw() {}

void BulletComponent::fire( glm::vec3& position , glm::vec3& direction )
{
	parent->translate = position;
	this->direction = direction;
	parent->active = true;
	currentLifeTime = lifeTime;
	parent->getComponent<Particle>()->addToTotalForce(direction *speed );
}