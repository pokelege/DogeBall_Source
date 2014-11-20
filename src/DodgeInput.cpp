#define GLM_FORCE_RADIANS
#include <DodgeInput.h>
#include <Input\KeyInput.h>
#include <Core\GameObject.h>
#include <Misc\ExtraFunctions.h>
#include <Physics\Particle.h>
#include <Misc\Clock.h>
void DodgeInput::update()
{
	cooldown -= Clock::dt;
	if ( !parent || (cooldown > 0) ) return;
	glm::vec3 totalTranslate;
	if ( KeyInput::isDown( up ) )
	{
		totalTranslate += moveSensitivity * y;
	}
	if ( KeyInput::isDown( down ) )
	{
		totalTranslate -= moveSensitivity * y;
	}
	if ( KeyInput::isDown( right ) )
	{
		totalTranslate += moveSensitivity * x;
	}
	if ( KeyInput::isDown( left ) )
	{
		totalTranslate -= moveSensitivity * x;
	}
	if ( glm::length( totalTranslate ) )
	{
		parent->rotate = rotationBetweenVectors( glm::vec3( 1 , 0 , 0 ) , glm::normalize( totalTranslate ) , 4 , x , -x , y , -y );
		
		if ( !pressing && timeLastPress <= doubleTapDelta && lastrotation == parent->rotate )
		{
			totalTranslate*= dodgeForce;
			cooldown = dodgeCooldown;
		}
		lastrotation = parent->rotate;
		pressing = true;
		//parent->rotate = glm::quat_cast( glm::lookAt( parent->translate , parent->translate + glm::normalize( totalTranslate ) , glm::normalize( glm::vec3(0,0,1) ) ) );
	}
	else
	{
		if ( pressing ) timeLastPress = 0;
		pressing = false;
		timeLastPress += Clock::dt;
	}
	Particle* theParticle = 0;
	if ( ( theParticle = parent->getComponent<Particle>() ) != 0 ) theParticle->addToTotalForce( totalTranslate );
	else parent->translate += totalTranslate * Clock::dt;
}