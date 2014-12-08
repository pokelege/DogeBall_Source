#pragma once
#include <Input\TwoDPlaneInput.h>
#include <gtc\quaternion.hpp>
class DodgeInput : public TwoDPlaneInput
{
	float timeLastPress;
	float cooldown;
	float currentSpeed;
	glm::quat lastrotation;
public:
	float dodgeCooldown;
	float dodgeForce;
	float doubleTapDelta;
	bool pressing;
	DodgeInput() { dodgeForce = 10.0f; doubleTapDelta = 0.25f; pressing = false; timeLastPress = 100; cooldown = 0; dodgeCooldown = 0.5f; currentSpeed = 0; }
	virtual void update();
};