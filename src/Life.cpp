#include <Life.h>
#include <Graphics\RenderableInfo.h>
#include <Core\GameObject.h>
#include <DebugMemory.h>
Life::Life() : fullLife(20) , lowLifeColor(glm::vec4(1,0,0,1))
{
	fullRecover();
}

bool Life::fullRecover()
{
	currentLife = fullLife;
	return true;
}
bool Life::changeLife( const float& dl )
{
	currentLife += dl;
	return currentLife > 0;
}

void Life::earlyDraw()
{
	if ( !parent ) return;
	Renderable* renderable = parent->getComponent<Renderable>();
	if ( !renderable )
	{
		renderable = parent->getComponentInChildren<Renderable>();
	}
	if ( !renderable ) return;
	color = glm::mix( lowLifeColor , glm::vec4( 1 , 1 , 1 , 1 ) , currentLife / fullLife );
	renderable->setRenderableUniform( "tint" , PT_VEC4 , reinterpret_cast< const void* >( &color ) );
	if ( currentLife <= 0 ) parent->active = false;
}