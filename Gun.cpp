#include "Gun.h"
#include "BulletComponent.h"
#include <Core\GameObjectManger.h>
#include <Core\GameObject.h>
#include <Input\KeyInput.h>
#include <gtc\matrix_transform.hpp>
#include <gtc\quaternion.hpp>
Gun::Gun()
{
	bullet = GameObjectManager::globalGameObjectManager.addGameObject();
	bullet->addComponent( bulletComponent = new BulletComponent() );
	bulletComponent->damage = 1;
	bulletComponent->lifeTime = 5;
	bulletComponent->range = 3;
	bulletComponent->speed = 2;
}
void Gun::earlyUpdate()
{
	if ( KeyInput::isDown( key ) )
	{
		glm::quat quaternion = glm::rotate( glm::quat() , parent->rotate.x , glm::vec3( 1 , 0 , 0 ) ) *
			glm::rotate( glm::quat() , parent->rotate.y , glm::vec3( 0 , 1 , 0 ) ) *
			glm::rotate( glm::quat() , parent->rotate.z , glm::vec3( 0 , 0 , 1 ) );
		glm::vec3 direction( glm::mat4_cast( quaternion ) * glm::vec4( parent->translate , 1 ) );
			bulletComponent->fire( parent->translate , direction );
	}
}