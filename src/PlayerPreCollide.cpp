#include <PlayerPreCollide.h>
#include <Physics\Particle.h>
#include <Core\GameObject.h>
#include <BulletComponent.h>
bool PlayerPreCollide::preCollideEvent( Particle* particle )
{
	if ( !parent ) return false;
	if ( !particle ) return false;
	if ( particle->parent )
	{
		BulletComponent* bullet = particle->parent->getComponent<BulletComponent>();
		if ( !bullet ) return true;
		if ( parent == bullet->target ) return true;
	}
	return false;
}