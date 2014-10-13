#pragma once
#include <Core\CommonWarningDisables.h>
#include <Core\Component.h>
#include <glm.hpp>
#include "DebugMemory.h"
class Life : public Component
{
	float currentLife;
	glm::vec4 color;
public:
	glm::vec4 lowLifeColor;
	float fullLife;
	Life();
	bool fullRecover();
	bool changeLife(const float& dl);
	virtual void earlyDraw();
};