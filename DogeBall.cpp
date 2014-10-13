#include "DogeBall.h"
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
#include <Qt\qtimer.h>
#include <Graphics\Camera.h>
#include <Input\TwoDPlaneInput.h>
#include <Graphics\GeometryInfo.h>
#include <Graphics\ParameterType.h>
#include <Graphics\VertexInfo.h>
#include <Core\WindowInfo.h>
#include <Input\TwoDPlaneInput.h>
#include <Input\Auto\TwoDZoomCamera.h>
#include <Input\KeyInput.h>
#include <Graphics\GraphicsTextureManager.h>
#include "DebugMemory.h"
#include "Life.h"
void DogeBall::initializeGL()
{
	CommonGraphicsCommands::initializeGlobalGraphics();
	std::string errors;
	std::string vert = FileReader( "Shaders/DiffuseVertex.glsl" );
	std::string frag = FileReader( "Shaders/DiffuseFragment.glsl" );
	ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str() , frag.c_str() , &errors );
	std::cout << errors.c_str() << std::endl;
	
	GeometryInfo* geometry = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "Models/player.pmd" , GraphicsBufferManager::globalBufferManager );
	geometry->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	geometry->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );

	GeometryInfo* levelGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "Models/level.pmd" , GraphicsBufferManager::globalBufferManager );
	levelGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	levelGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );

	TextureInfo* player1Texture = GraphicsTextureManager::globalTextureManager.addTexture( "Textures/Player1.png" );
	TextureInfo* player2Texture = GraphicsTextureManager::globalTextureManager.addTexture( "Textures/Player2.png" );

	TextureInfo* levelTexture = GraphicsTextureManager::globalTextureManager.addTexture( "Textures/Level.png" );

	Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable->initialize( 5 , 1 );
	renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable->geometryInfo = geometry;
	renderable->shaderInfo = shader;
	renderable->alphaBlendingEnabled = false;
	renderable->culling = CT_NONE;
	renderable->addTexture( player1Texture );
	GameObjectManager::globalGameObjectManager.initialize( 5 );

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
	renderable2->addTexture( player2Texture );
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
	
	level = GameObjectManager::globalGameObjectManager.addGameObject();
	level->scale = glm::vec3( 100 , 100 , 100);
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
	//GraphicsSharedUniformManager::globalSharedUniformManager.setSharedUniform( "colorTexture" , PT_INT , &theTex );
	timer = new QTimer();
	connect( timer , SIGNAL( timeout() ) , this , SLOT( update() ) );
	timer->start( 0 );
	defaultColor = glm::vec4( 1 , 1 , 1 , 1 );
}
void DogeBall::paintGL()
{
	GraphicsCameraManager::globalCameraManager.drawAllCameras();
}

void DogeBall::update()
{
	WindowInfo::width = width();
	WindowInfo::height = height();
	Clock::update();
	GameObjectManager::globalGameObjectManager.earlyUpdateParents();
	GameObjectManager::globalGameObjectManager.updateParents();
	GameObjectManager::globalGameObjectManager.lateUpdateParents();
	GameObjectManager::globalGameObjectManager.earlyDrawParents();
	repaint();
	GameObjectManager::globalGameObjectManager.lateDrawParents();
}

DogeBall::~DogeBall()
{
	CommonGraphicsCommands::destroyGlobalGraphics();
	GameObjectManager::globalGameObjectManager.destroy();
	delete zoomer;
	delete timer;
	delete planeInput2;
	delete planeInput;
	delete life1;
	delete life2;
}