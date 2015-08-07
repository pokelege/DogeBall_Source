#define GLM_FORCE_RADIANS
#include <DodgeInput.h>
#include <Input\KeyInput.h>
#include <Core\GameObject.h>
#include <Misc\ExtraFunctions.h>
#include <Physics\Particle.h>
#include <Misc\Clock.h>
#include <Graphics\AnimationRenderingInfo.h>
#include <GlobalItems.h>
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

	Animator* animate = parent->getComponent<Animator>();
	if ( !animate )  animate = parent->getComponentInChildren<Animator>();

	if ( glm::length( totalTranslate ) )
	{
		parent->rotate = rotationBetweenVectors( glm::vec3( 1 , 0 , 0 ) , glm::normalize( totalTranslate ) , 4 , x , -x , y , -y );
		currentSpeed += ( 1.0f - currentSpeed ) * 0.5f * Clock::dt;
		if ( !pressing && timeLastPress <= doubleTapDelta && lastrotation == parent->rotate )
		{
			totalTranslate*= dodgeForce;
			cooldown = dodgeCooldown;
			GlobalItems::global.playDodge();
			if ( animate && animate->currentlyPlaying!= 0 )animate->play( 0 );
		}
		else
		{
			totalTranslate *= currentSpeed;
			if ( animate && animate->currentlyPlaying != 1 ) animate->play( 1 );
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
		currentSpeed = 0;
		if ( animate && animate->currentlyPlaying != 0 )animate->play( 0 );
	}
	Particle* theParticle = 0;
	if ( ( theParticle = parent->getComponent<Particle>() ) != 0 ) theParticle->addToTotalForce( totalTranslate );
	else parent->translate += totalTranslate * Clock::dt;
}