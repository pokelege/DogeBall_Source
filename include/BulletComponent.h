#pragma once
#include <Core\CommonWarningDisables.h>
#include <Core\Component.h>
#include <glm.hpp>
class BulletComponent : public Component
{
	friend class BulletCollisionEvent;
	glm::vec3 direction;
	float currentLifeTime;
protected:
	friend class GameObject;
	virtual void attatch( GameObject* parent );
	virtual void detatch();
public:
	GameObject* parent;
	GameObject* target;
	float lifeTime;
	float speed;
	float damage;
	void fire( glm::vec3& position , glm::vec3& direction );
	virtual void earlyUpdate();
	virtual void update();
	virtual void lateUpdate();
	virtual void earlyDraw();
	virtual void draw();
	virtual void lateDraw();
};