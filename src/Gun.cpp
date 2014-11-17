#define GLM_FORCE_RADIANS
#include <Core\CommonWarningDisables.h>
#include <Gun.h>
#include <BulletComponent.h>
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
#include <GlobalItems.h>
#include <BulletCollisionEvent.h>
#include <Physics\Particle.h>
#include <Physics\ParticleWorld.h>
Gun::Gun( GameObject* target )
{
	std::string errors;
	std::string vert = FileReader( "assets/shaders/DiffuseVertex.glsl" );
	std::string frag = FileReader( "assets/shaders/DiffuseFragment.glsl" );
	ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str() , frag.c_str() , &errors );
	std::cout << errors.c_str() << std::endl;
	GeometryInfo* ballGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/ball.pmd" , GraphicsBufferManager::globalBufferManager );
	ballGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	ballGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );
	ballGeo->addShaderStreamedParameter( 2 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::NORMAL_OFFSET );
	ballGeo->addShaderStreamedParameter( 4 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::TANGENT_OFFSET );
	ballGeo->addShaderStreamedParameter( 5 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::BITANGENT_OFFSET );
	ballGeo->addShaderStreamedParameter( 6 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGINDEX_OFFSET );
	ballGeo->addShaderStreamedParameter( 7 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGWEIGHT_OFFSET );
	Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable->initialize( 5 , 1 );
	renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable->geometryInfo = ballGeo;
	renderable->shaderInfo = shader;
	renderable->addTexture( GlobalItems::global.dogePatternTexture );
	renderable->alphaBlendingEnabled = false;
	renderable->culling = CT_NONE;

	bullet = GameObjectManager::globalGameObjectManager.addGameObject();
	bullet->addComponent( bulletComponent = new BulletComponent() );
	bullet->addComponent( collisionEvent = new BulletCollisionEvent );
	bullet->addComponent( particle = new Particle );
	ParticleWorld::global.addParticleToManage( particle );
	particle->collisionRadius = 1;
	particle->mass = 0.5;
	bulletComponent->damage = 1;
	bulletComponent->lifeTime = 0.5;
	bulletComponent->speed = 1000;
	bulletComponent->target = target;
	bullet->scale *= particle->collisionRadius;
	bullet->addComponent( renderable );
}
void Gun::earlyUpdate()
{
	if ( !bulletComponent->parent->active && KeyInput::isDown( key ) )
	{
		glm::vec3 direction( glm::mat4_cast( parent->rotate ) * glm::vec4( 1,0,0 , 0 ) );
			bulletComponent->fire( parent->translate , direction );
	}
}

Gun::~Gun()
{
	delete bulletComponent;
	delete collisionEvent;
	ParticleWorld::global.removeParticleToManage( particle );
	delete particle;
}