#include <BulletCollisionEvent.h>
#include <GlobalItems.h>
#include <Life.h>
#include <Core\GameObject.h>
#include <Physics\Particle.h>
#include <BulletComponent.h>
void BulletCollisionEvent::collisionEvent( Particle* other )
{
	if ( !parent ) return;
	GlobalItems::global.playHit();
	BulletComponent* thisComponent = parent->getComponent<BulletComponent>();
	if ( !thisComponent ) return;
	if ( thisComponent->currentLifeTime <= 0 ) return;
	
	thisComponent->currentLifeTime = 0;
	
	if ( !thisComponent->target ) return;
	if ( ( thisComponent->target != other->parent ) ) return;
	Life* life = thisComponent->target->getComponent<Life>();
	if ( !life ) return;
	life->changeLife( -thisComponent->damage );
	thisComponent->parent->getComponent<Particle>( )->velocity = glm::vec3( );
	thisComponent->parent->active = false;
	GlobalItems::global.addPain( thisComponent->target->translate );
}