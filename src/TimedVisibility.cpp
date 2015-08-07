#include <TimedVisibility.h>
#include <Misc\Clock.h>
#include <Core\GameObject.h>
TimedVisibility::TimedVisibility() : currentTime( 0 ) {}

void TimedVisibility::earlyDraw()
{
	if ( currentTime <= 0 ) return;
	currentTime -= Clock::dt;
	if ( currentTime <= 0 && parent ) parent->active = false;
}
void TimedVisibility::makeVisible( const float& time )
{
	currentTime = time;
	if ( currentTime <= 0 && parent ) parent->active = false;
	else if ( currentTime > 0 && parent )parent->active = true;
}