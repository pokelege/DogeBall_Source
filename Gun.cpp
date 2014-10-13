#include "Gun.h"
#include "BulletComponent.h"
#include <Core\GameObjectManger.h>
#include <Core\GameObject.h>
#include <Input\KeyInput.h>
#include <gtc\matrix_transform.hpp>
#include <gtc\quaternion.hpp>
#include <Graphics\GeometryInfo.h>
#include <Graphics\GraphicsGeometryManager.h>
#include <Graphics\GraphicsBufferManager.h>
#include <Graphics\VertexInfo.h>
#include <Graphics\RenderableInfo.h>
#include <Graphics\GraphicsRenderingManager.h>
#include <Graphics\GraphicsSharedUniformManager.h>
#include <Misc\FileReader.h>
#include <Graphics\GraphicsShaderManager.h>
#include <iostream>
Gun::Gun( GameObject* target )
{
	std::string errors;
	std::string vert = FileReader( "Shaders/DiffuseVertex.glsl" );
	std::string frag = FileReader( "Shaders/DiffuseFragment.glsl" );
	ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str() , frag.c_str() , &errors );
	std::cout << errors.c_str() << std::endl;
	GeometryInfo* ballGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "Models/ball.pmd" , GraphicsBufferManager::globalBufferManager );
	ballGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	ballGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );

	Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable->initialize( 5 , 1 );
	renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable->geometryInfo = ballGeo;
	renderable->shaderInfo = shader;
	renderable->alphaBlendingEnabled = false;
	renderable->culling = CT_NONE;

	bullet = GameObjectManager::globalGameObjectManager.addGameObject();
	bullet->addComponent( bulletComponent = new BulletComponent() );
	bulletComponent->damage = 1;
	bulletComponent->lifeTime = 0.5;
	bulletComponent->range = 1;
	bulletComponent->speed = 0.75;
	bulletComponent->target = target;
	bullet->scale *= bulletComponent->range;
	bullet->addComponent( renderable );
}
void Gun::earlyUpdate()
{
	if ( !bulletComponent->parent->active && KeyInput::isDown( key ) )
	{
		glm::quat quaternion = glm::rotate( glm::quat() , parent->rotate.x , glm::vec3( 1 , 0 , 0 ) ) *
			glm::rotate( glm::quat() , parent->rotate.y , glm::vec3( 0 , 1 , 0 ) ) *
			glm::rotate( glm::quat() , parent->rotate.z , glm::vec3( 0 , 0 , 1 ) );
		glm::vec3 direction( glm::mat4_cast( quaternion ) * glm::vec4( 1,0,0 , 1 ) );
			bulletComponent->fire( parent->translate , direction );
	}
}

Gun::~Gun()
{
	delete bulletComponent;
}