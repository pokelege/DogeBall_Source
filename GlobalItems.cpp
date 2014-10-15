#include "GlobalItems.h"
#include <Graphics\GraphicsTextureManager.h>
#include "Life.h"
#include "Gun.h"
#include <Input\TwoDPlaneInput.h>
#include <Input\Auto\TwoDZoomCamera.h>
#include <Input\KeyInput.h>
#include <Graphics\GraphicsTextureManager.h>
#include <Graphics\Camera.h>
#include <Input\TwoDPlaneInput.h>
#include <Graphics\GeometryInfo.h>
#include <Graphics\ParameterType.h>
#include <Graphics\VertexInfo.h>
#include <Graphics\CommonGraphicsCommands.h>
#include <Graphics\GraphicsCameraManager.h>
#include <Graphics\GraphicsBufferManager.h>
#include <Graphics\GraphicsGeometryManager.h>
#include <Graphics\GraphicsShaderManager.h>
#include <Graphics\GraphicsSharedUniformManager.h>
#include <Graphics\GraphicsRenderingManager.h>
#include <Core\GameObject.h>
#include <Core\GameObjectManger.h>
#include <Misc\FileReader.h>
#include <Misc\Clock.h>
#include <iostream>
#include <Graphics\RenderableInfo.h>
GlobalItems GlobalItems::global;
void GlobalItems::playHit()
{
	if (!audio ) initAudio();
	audio->playSound( "Audio/hit.mp3" );
}
void GlobalItems::playMusic()
{
	if ( !audio ) initAudio();
	audio->playSound( "Audio/music.mp3" , true );
}

void GlobalItems::initAudio()
{
	if ( audio ) destroyAudio();
	audio = new AudioController;
	audio->initialize();
}

void GlobalItems::destroyAudio()
{
	if ( !audio ) return;
	audio->destroy();
	delete audio;
	audio = 0;
}

void GlobalItems::initPlayerTextures()
{
	player1Texture = GraphicsTextureManager::globalTextureManager.addTexture( "Textures/Player1.png" );
	player2Texture = GraphicsTextureManager::globalTextureManager.addTexture( "Textures/Player2.png" );
}

void GlobalItems::initLevel()
{
	CommonGraphicsCommands::initializeGlobalGraphics();
	std::string errors;
	std::string vert = FileReader( "Shaders/DiffuseVertex.glsl" );
	std::string frag = FileReader( "Shaders/DiffuseFragment.glsl" );
	ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str() , frag.c_str() , &errors );
	std::cout << errors.c_str() << std::endl;

	GlobalItems::global.initPlayerTextures();

	GeometryInfo* geometry = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "Models/player.pmd" , GraphicsBufferManager::globalBufferManager );
	geometry->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	geometry->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );

	GeometryInfo* levelGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "Models/level.pmd" , GraphicsBufferManager::globalBufferManager );
	levelGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	levelGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );

	TextureInfo* levelTexture = GraphicsTextureManager::globalTextureManager.addTexture( "Textures/Level.png" );

	Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable->initialize( 5 , 1 );
	renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable->geometryInfo = geometry;
	renderable->shaderInfo = shader;
	renderable->alphaBlendingEnabled = false;
	renderable->culling = CT_NONE;
	renderable->addTexture( GlobalItems::global.player1Texture );
	GameObjectManager::globalGameObjectManager.initialize( 10 );

	player = GameObjectManager::globalGameObjectManager.addGameObject();
	player->translate = glm::vec3( -10 , 0 , 0 );
	player->addComponent( renderable );
	planeInput = new TwoDPlaneInput;
	planeInput->moveSensitivity = 10;
	player->addComponent( planeInput );
	life1 = new Life;
	player->addComponent( life1 );

	player2 = GameObjectManager::globalGameObjectManager.addGameObject();
	player2->translate = glm::vec3( 10 , 0 , 0 );
	Renderable* renderable2 = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable2->initialize( 5 , 1 );
	renderable2->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable2->geometryInfo = geometry;
	renderable2->shaderInfo = shader;
	renderable2->alphaBlendingEnabled = false;
	renderable2->culling = CT_NONE;
	renderable2->addTexture( GlobalItems::global.player2Texture );
	player2->addComponent( renderable2 );
	planeInput2 = new TwoDPlaneInput;
	planeInput2->up = VK_UP;
	planeInput2->down = VK_DOWN;
	planeInput2->left = VK_LEFT;
	planeInput2->right = VK_RIGHT;
	planeInput2->moveSensitivity = 10;
	player2->addComponent( planeInput2 );
	life2 = new Life;
	player2->addComponent( life2 );
	player2->addComponent( gun2 = new Gun( player ) );
	gun2->key = VK_RSHIFT;

	player->addComponent( gun1 = new Gun( player2 ) );
	gun1->key = VK_SPACE;

	level = GameObjectManager::globalGameObjectManager.addGameObject();
	level->scale = glm::vec3( 100 , 100 , 100 );
	Renderable* renderable3 = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable3->initialize( 5 , 1 );
	renderable3->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable3->geometryInfo = levelGeo;
	renderable3->shaderInfo = shader;
	renderable3->alphaBlendingEnabled = false;
	renderable3->culling = CT_NONE;
	renderable3->addTexture( levelTexture );
	renderable3->setRenderableUniform( "tint" , PT_VEC4 , reinterpret_cast< const void* >( &defaultColor ) );
	level->addComponent( renderable3 );

	GameObject* view = GameObjectManager::globalGameObjectManager.addGameObject();
	camera = GraphicsCameraManager::globalCameraManager.addCamera();
	camera->initializeRenderManagers();
	camera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	camera->FOV = 60.0f;
	camera->nearestObject = 0.01f;
	view->addComponent( camera );
	view->translate = glm::vec3( 0 , 0 , 50 );
	zoomer = new TwoDZoomCamera;
	zoomer->initialize( 2 );
	zoomer->addGameObjectToTrack( player );
	zoomer->addGameObjectToTrack( player2 );
	zoomer->maxDistance = 50;
	zoomer->minDistance = 2;
	zoomer->zoomScale = 2;
	view->addComponent( zoomer );
	theTex = 0;
	defaultColor = glm::vec4( 1 , 1 , 1 , 1 );
	playMusic();
}
void GlobalItems::updateLevel()
{
	Clock::update();
	GameObjectManager::globalGameObjectManager.earlyUpdateParents();
	GameObjectManager::globalGameObjectManager.updateParents();
	GameObjectManager::globalGameObjectManager.lateUpdateParents();
	GameObjectManager::globalGameObjectManager.earlyDrawParents();
}
void GlobalItems::drawLevel()
{
	GraphicsCameraManager::globalCameraManager.drawAllCameras();
	GameObjectManager::globalGameObjectManager.lateDrawParents();
}
void GlobalItems::destroyLevel()
{

	if(planeInput2) delete planeInput2;
	planeInput2 = 0;
	if ( planeInput ) delete planeInput;
	planeInput = 0;
	if ( life1 ) delete life1;
	life1 = 0;
	if ( life2 ) delete life2;
	life2 = 0;
	if ( gun1 ) delete gun1;
	gun1 = 0;
	if ( gun2 ) delete gun2;
	gun2 = 0;
	if ( zoomer ) delete zoomer;
	zoomer = 0;

}

void GlobalItems::initStart()
{
	CommonGraphicsCommands::initializeGlobalGraphics();
	std::string errors;
	std::string vert = FileReader( "Shaders/FlatVertex.glsl" );
	std::string frag = FileReader( "Shaders/FlatFragment.glsl" );
	ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str() , frag.c_str() , &errors );
	std::cout << errors.c_str() << std::endl;

	GeometryInfo* levelGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "Models/level.pmd" , GraphicsBufferManager::globalBufferManager );
	levelGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	levelGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );

	TextureInfo* levelTexture = GraphicsTextureManager::globalTextureManager.addTexture( "Textures/Title.png" );

	GameObjectManager::globalGameObjectManager.initialize(2);

	level = GameObjectManager::globalGameObjectManager.addGameObject();
	Renderable* renderable3 = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable3->initialize( 5 , 1 );
	renderable3->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable3->geometryInfo = levelGeo;
	renderable3->shaderInfo = shader;
	renderable3->alphaBlendingEnabled = false;
	renderable3->culling = CT_NONE;
	renderable3->addTexture( levelTexture );
	level->addComponent( renderable3 );
	theTex = 0;

	GameObject* view = GameObjectManager::globalGameObjectManager.addGameObject();
	camera = GraphicsCameraManager::globalCameraManager.addCamera();
	camera->initializeRenderManagers();
	camera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	camera->FOV = 60.0f;
	camera->nearestObject = 0.01f;
	view->addComponent( camera );
}
void GlobalItems::drawStart()
{
	GraphicsCameraManager::globalCameraManager.drawAllCameras();
	GameObjectManager::globalGameObjectManager.lateDrawParents();
}
void GlobalItems::updateStart()
{
	Clock::update();
	GameObjectManager::globalGameObjectManager.earlyUpdateParents();
	GameObjectManager::globalGameObjectManager.updateParents();
	GameObjectManager::globalGameObjectManager.lateUpdateParents();
	GameObjectManager::globalGameObjectManager.earlyDrawParents();
	if ( KeyInput::isDown( VK_SPACE ) ) changeState( GameStates::Level );
}
void GlobalItems::destroyStart()
{
	if ( planeInput2 ) delete planeInput2;
	planeInput2 = 0;
	if ( planeInput ) delete planeInput;
	planeInput = 0;
	if ( life1 ) delete life1;
	life1 = 0;
	if ( life2 ) delete life2;
	life2 = 0;
	if ( gun1 ) delete gun1;
	gun1 = 0;
	if ( gun2 ) delete gun2;
	gun2 = 0;
	if ( zoomer ) delete zoomer;
	zoomer = 0;

}

void GlobalItems::changeState( GameStates state )
{
	this->state = GameStates::None;
	destroyAll();
	switch ( state )
	{
		case Start:
			initStart();
			break;
		case Level:
			initLevel();
			break;
		case Player1:
			break;
		case Player2:
			break;
		default:
			break;
	}
	this->state = state;
}

void GlobalItems::update()
{
	switch ( state )
	{
		case Start:
			updateStart();
			break;
		case Level:
			updateLevel();
			break;
		case Player1:
			break;
		case Player2:
			break;
		default:
			break;
	}
}
void GlobalItems::draw()
{
	switch ( state )
	{
		case Start:
			drawStart();
			break;
		case Level:
			drawLevel();
			break;
		case Player1:
			break;
		case Player2:
			break;
		default:
			break;
	}
}

GlobalItems::GlobalItems() :audio(0) , player1Texture(0) , player2Texture(0) , player(0) , player2(0) , level(0) , life1(0) , life2(0) , gun1(0) , gun2(0) , camera(0) , zoomer(0) , planeInput(0) , planeInput2(0) , state(GameStates::None)
{
	
}

void GlobalItems::destroyAll()
{
	destroyAudio();
	destroyLevel();
	destroyStart();
	CommonGraphicsCommands::destroyGlobalGraphics();
	GameObjectManager::globalGameObjectManager.destroy();
}

GlobalItems::~GlobalItems()
{
	destroyAll();
}