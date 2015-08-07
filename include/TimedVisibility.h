#pragma once
#include <Core\Component.h>
#include <glm.hpp>
class TimedVisibility : public Component
{
	float currentTime;
	glm::vec4 color;
public:
	TimedVisibility();
	void earlyDraw();
	void makeVisible( const float& time = 1 );
};