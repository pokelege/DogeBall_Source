#pragma once
#include <Core\Component.h>

class TimedVisibility : public Component
{
	float currentTime;
public:
	TimedVisibility();
	void earlyDraw();
	void makeVisible( const float& time = 1 );
};