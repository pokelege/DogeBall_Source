#define GLM_FORCE_RADIANS
#include <GlobalItems.h>
#include <Graphics\GraphicsTextureManager.h>
#include <Life.h>
#include <Gun.h>
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
#include <Physics\Particle.h>
#include <Physics\ParticleWorld.h>
#include <Misc\RandomItemsGenerator.h>
#include <TimedVisibility.h>
#include <time.h>
#include <PlayerPreCollide.h>
#include <Graphics\AnimationRenderingInfo.h>
#include <Graphics\GraphicsLightManager.h>
#include <Graphics\Light.h>
#include <DodgeInput.h>
#include <GL\glew.h>
#include <QtCore\QDirIterator>
#include <Graphics\TextureInfo.h>
#include <DebugMemory.h>
GlobalItems GlobalItems::global;
void GlobalItems::playHit()
{
	if (!audio ) initAudio();
	audio->playSound( "assets/audio/hit.mp3" );
}
void GlobalItems::playMusic()
{
	if ( !audio ) initAudio();
	audio->playSound( "assets/audio/music.mp3" , true );
}

void GlobalItems::playFall( )
{
	if ( !audio ) initAudio( );
	audio->playSound( "assets/audio/fall.wav" );
}

void GlobalItems::playFail( )
{
	if ( !audio ) initAudio( );
	audio->playSound( "assets/audio/fail.wav" );
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
	player1Texture = GraphicsTextureManager::globalTextureManager.addTexture( "assets/textures/Player1.tex" );
	player2Texture = GraphicsTextureManager::globalTextureManager.addTexture( "assets/textures/Player2.tex" );
	dogePatternTexture = GraphicsTextureManager::globalTextureManager.addTexture( "assets/textures/DogePattern.tex" );
}

void GlobalItems::initDogeWinWords( )
{
	win.clear( );
	std::string errors;
	std::string vert = FileReader( "assets/shaders/DogeWordVertex.glsl" );
	std::string frag = FileReader( "assets/shaders/DogeWordFragment.glsl" );
	ShaderInfo* dogeShader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str( ) , frag.c_str( ) , &errors );
	std::cout << errors.c_str( ) << std::endl;
	GeometryInfo* levelGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/level.pmd" , GraphicsBufferManager::globalBufferManager );
	levelGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	levelGeo->addShaderStreamedParameter( 1 , PT_VEC4 , 0 , 0 );
	levelGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );
	levelGeo->addShaderStreamedParameter( 2 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::NORMAL_OFFSET );
	levelGeo->addShaderStreamedParameter( 4 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::TANGENT_OFFSET );
	levelGeo->addShaderStreamedParameter( 5 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::BITANGENT_OFFSET );
	levelGeo->addShaderStreamedParameter( 6 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGINDEX_OFFSET );
	levelGeo->addShaderStreamedParameter( 7 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGWEIGHT_OFFSET );

	QDirIterator winDir( "assets/textures/DogeWord/Win/" );
	while ( winDir.hasNext( ) )
	{
		QFileInfo theFile = winDir.next( );
		if ( theFile.suffix( ).compare( "tex" ) ) continue;
		TextureInfo* tex = GraphicsTextureManager::globalTextureManager.addTexture( theFile.absoluteFilePath( ).toUtf8( ) );
		for ( unsigned int i = 0; i < 5; ++i )
		{
			GameObject* object = GameObjectManager::globalGameObjectManager.addGameObject( );
			Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable( );
			renderable->initialize( 5 , 1 );
			renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
			renderable->geometryInfo = levelGeo;
			renderable->shaderInfo = dogeShader;
			renderable->alphaBlendingEnabled = true;
			renderable->culling = CT_NONE;
			renderable->addTexture( tex );
			renderable->depthTestEnabled = false;
			object->addComponent( renderable );
			TextureParams par = tex->getParams( );
			if ( std::max( par.width , par.height ) == par.width )
			{
				object->scale = 0.25f * glm::vec3( ( float ) par.width / par.width , ( float ) par.height / par.width , 1 );
			}
			else
			{
				object->scale = 0.25f * glm::vec3( ( float ) par.width / par.height , ( float ) par.height / par.height , 1 );
			}

			TimedVisibility* vis = new TimedVisibility;
			object->addComponent( vis );
			vis->makeVisible( 0 );
			win.push_back( vis );
		}
	}
}

void GlobalItems::initDogeWords()
{
	//clear the pain
	pain.clear();
	miss.clear();
	std::string errors;
	std::string vert = FileReader( "assets/shaders/DogeWordVertex.glsl" );
	std::string frag = FileReader( "assets/shaders/DogeWordFragment.glsl" );
	ShaderInfo* dogeShader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str( ) , frag.c_str( ) , &errors );
	std::cout << errors.c_str( ) << std::endl;
	GeometryInfo* levelGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/level.pmd" , GraphicsBufferManager::globalBufferManager );
	levelGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	levelGeo->addShaderStreamedParameter( 1 , PT_VEC4 , 0 , 0 );
	levelGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );
	levelGeo->addShaderStreamedParameter( 2 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::NORMAL_OFFSET );
	levelGeo->addShaderStreamedParameter( 4 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::TANGENT_OFFSET );
	levelGeo->addShaderStreamedParameter( 5 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::BITANGENT_OFFSET );
	levelGeo->addShaderStreamedParameter( 6 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGINDEX_OFFSET );
	levelGeo->addShaderStreamedParameter( 7 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGWEIGHT_OFFSET );

	QDirIterator painDir( "assets/textures/DogeWord/Pain/" );
	while ( painDir.hasNext() )
	{
		QFileInfo theFile = painDir.next();
		if ( theFile.suffix().compare( "tex" ) ) continue;
		TextureInfo* tex = GraphicsTextureManager::globalTextureManager.addTexture(theFile.absoluteFilePath().toUtf8() );
		for ( unsigned int i = 0; i < 5; ++i )
		{
			GameObject* object = GameObjectManager::globalGameObjectManager.addGameObject( );
			Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable( );
			renderable->initialize( 5 , 1 );
			renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
			renderable->geometryInfo = levelGeo;
			renderable->shaderInfo = dogeShader;
			renderable->alphaBlendingEnabled = true;
			renderable->culling = CT_NONE;
			renderable->addTexture( tex );
			renderable->depthTestEnabled = false;
			object->addComponent( renderable );
			TextureParams par = tex->getParams( );
			if ( std::max( par.width , par.height ) == par.width )
			{
				object->scale = 0.25f * glm::vec3( ( float ) par.width / par.width , ( float ) par.height / par.width , 1 );
			}
			else
			{
				object->scale = 0.25f * glm::vec3( ( float ) par.width / par.height , ( float ) par.height / par.height , 1 );
			}

			TimedVisibility* vis = new TimedVisibility;
			object->addComponent( vis );
			vis->makeVisible( 0 );
			pain.push_back( vis );
		}
	}


	QDirIterator missDir( "assets/textures/DogeWord/Miss/" );
	while ( missDir.hasNext( ) )
	{
		
		QFileInfo theFile = missDir.next( );
		if ( theFile.suffix( ).compare( "tex" ) ) continue;
		TextureInfo* tex = GraphicsTextureManager::globalTextureManager.addTexture( theFile.absoluteFilePath( ).toUtf8( ) );
		for ( unsigned int i = 0; i < 5; ++i )
		{
			GameObject* object = GameObjectManager::globalGameObjectManager.addGameObject();
			Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable();
			renderable->initialize( 5 , 1 );
			renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
			renderable->geometryInfo = levelGeo;
			renderable->shaderInfo = dogeShader;
			renderable->alphaBlendingEnabled = true;
			renderable->culling = CT_NONE;
			renderable->addTexture( tex );
			renderable->depthTestEnabled = false;
			object->addComponent( renderable );
			TextureParams par = tex->getParams();
			if ( std::max( par.width , par.height ) == par.width )
			{
				object->scale = 0.25f * glm::vec3( ( float ) par.width / par.width , ( float ) par.height / par.width , 1 );
			}
			else
			{
				object->scale = 0.25f * glm::vec3( ( float ) par.width / par.height , ( float ) par.height / par.height , 1 );
			}

			TimedVisibility* vis = new TimedVisibility;
			object->addComponent( vis );
			vis->makeVisible( 0 );
			miss.push_back( vis );
		}

	}
}

void GlobalItems::addPain( const glm::vec3& worldPos )
{
	if ( !pain.size() ) return;
	TimedVisibility* vis = pain.at( rand() % pain.size() );
	if ( !vis || vis->parent->active )
	{
		vis = 0;
		for ( unsigned int i = 0; i < pain.size() && !vis; ++i )
		{
			if ( !pain.at( i )->parent->active ) vis = pain.at( i );
		}
	}
	if ( !vis ) return;
	vis->makeVisible();
	vis->parent->translate = worldPos;
}

void GlobalItems::addMiss( const glm::vec3& worldPos )
{
	if ( !miss.size( ) ) return;
	TimedVisibility* vis = miss.at( rand( ) % miss.size( ) );
	if ( !vis || vis->parent->active )
	{
		vis = 0;
		for ( unsigned int i = 0; i < miss.size( ) && !vis; ++i )
		{
			if ( !miss.at( i )->parent->active ) vis = miss.at( i );
		}
	}
	if ( !vis ) return;
	vis->makeVisible( );
	vis->parent->translate = worldPos;
}

void GlobalItems::addWin( )
{
	if ( !win.size( ) ) return;
	TimedVisibility* vis = win.at( rand( ) % win.size( ) );
	if ( !vis || vis->parent->active )
	{
		vis = 0;
		for ( unsigned int i = 0; i < win.size( ) && !vis; ++i )
		{
			if ( !win.at( i )->parent->active ) vis = win.at( i );
		}
	}
	if ( !vis ) return;
	vis->makeVisible(2 );
	vis->parent->translate = glm::vec3( ((((float)rand() / RAND_MAX) * 2) - 1) * 0.5f,
										( ( ( ( float ) rand( ) / RAND_MAX ) * 2 ) - 1 ) * 0.5f ,
		0);
}

void GlobalItems::destroyDogeWinWords( )
{
	for each ( TimedVisibility* var in win )
	{
		delete var;
	}
	win.clear( );
}

void GlobalItems::destroyDogeWords( )
{
	for each ( TimedVisibility* var in pain )
	{
		delete var;
	}
	for each ( TimedVisibility* var in miss )
	{
		delete var;
	}
	pain.clear();
	miss.clear();
}

void GlobalItems::initLevel()
{
	
	CommonGraphicsCommands::initializeGlobalGraphics();
	GraphicsCameraManager::globalCameraManager.initialize( 2 );
	std::string errors;
	std::string vert = FileReader( "assets/shaders/DiffuseVertex.glsl" );
	std::string frag = FileReader( "assets/shaders/DiffuseFragment.glsl" );
	shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str() , frag.c_str() , &errors );
	std::cout << errors.c_str() << std::endl;

	GlobalItems::global.initPlayerTextures();

	GeometryInfo* geometry = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/doge.pmd" , GraphicsBufferManager::globalBufferManager );
	geometry->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	geometry->addShaderStreamedParameter( 1 , PT_VEC4 , 0 , 0 );
	geometry->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );
	geometry->addShaderStreamedParameter( 2 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::NORMAL_OFFSET );
	geometry->addShaderStreamedParameter( 4 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::TANGENT_OFFSET );
	geometry->addShaderStreamedParameter( 5 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::BITANGENT_OFFSET );
	geometry->addShaderStreamedParameter( 6 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGINDEX_OFFSET );
	geometry->addShaderStreamedParameter( 7 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGWEIGHT_OFFSET );
	GeometryInfo* levelGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/level.pmd" , GraphicsBufferManager::globalBufferManager );
	levelGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	levelGeo->addShaderStreamedParameter( 1 , PT_VEC4 , 0 , 0 );
	levelGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );
	levelGeo->addShaderStreamedParameter( 2 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::NORMAL_OFFSET );
	levelGeo->addShaderStreamedParameter( 4 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::TANGENT_OFFSET );
	levelGeo->addShaderStreamedParameter( 5 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::BITANGENT_OFFSET );
	levelGeo->addShaderStreamedParameter( 6 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGINDEX_OFFSET );
	levelGeo->addShaderStreamedParameter( 7 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGWEIGHT_OFFSET );

	TextureInfo* levelTexture = GraphicsTextureManager::globalTextureManager.addTexture( "assets/textures/Level.tex" );

	Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable->initialize( 5 , 1 );
	renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable->geometryInfo = geometry;
	renderable->shaderInfo = shader;
	renderable->alphaBlendingEnabled = false;
	renderable->culling = CT_NONE;
	renderable->addTexture( GlobalItems::global.player1Texture );
	GameObjectManager::globalGameObjectManager.initialize( 50 );

	player = GameObjectManager::globalGameObjectManager.addGameObject();
	GameObject* renderableGameObject = GameObjectManager::globalGameObjectManager.addGameObject();
	player->translate = glm::vec3( -50 , 0 , 3 );
	player->scale = glm::vec3( 5 , 5 , 5 );
	renderableGameObject->addComponent( renderable );
	player->addChild( renderableGameObject );
	renderableGameObject->rotate = glm::angleAxis( glm::radians( 90.0f ) , glm::vec3( 1 , 0 , 0 ) );
	planeInput = new DodgeInput;
	planeInput->moveSensitivity = 2000;
	planeInput->dodgeForce = 50.0f;
	player->addComponent( planeInput );
	life1 = new Life;
	player->addComponent( life1 );
	player1PreCollide = new PlayerPreCollide;
	player->addComponent( player1PreCollide );

	player1Particle = new Particle;
	player->addComponent( player1Particle );
	player1Particle->collisionRadius = 5;
	player1Particle->freezeZ = true;
	renderableGameObject->addComponent( animate1 = new AnimationRenderingInfo );

	GameObject* renderableGameObject2 = GameObjectManager::globalGameObjectManager.addGameObject();

	player2 = GameObjectManager::globalGameObjectManager.addGameObject();
	player2->translate = glm::vec3( 50 , 0 , 3 );
	player2->scale = glm::vec3( 5 , 5 , 5 );
	Renderable* renderable2 = GraphicsRenderingManager::globalRenderingManager.addRenderable();
	renderable2->initialize( 5 , 1 );
	renderable2->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable2->geometryInfo = geometry;
	renderable2->shaderInfo = shader;
	renderable2->alphaBlendingEnabled = false;
	renderable2->culling = CT_NONE;
	renderable2->addTexture( GlobalItems::global.player2Texture );
	renderableGameObject2->addComponent( renderable2 );
	renderableGameObject2->rotate = glm::angleAxis( glm::radians( 90.0f ) , glm::vec3( 1 , 0 , 0 ) );
	player2->addChild( renderableGameObject2 );
	planeInput2 = new DodgeInput;
	planeInput2->up = VK_UP;
	planeInput2->down = VK_DOWN;
	planeInput2->left = VK_LEFT;
	planeInput2->right = VK_RIGHT;
	planeInput2->moveSensitivity = 2000;
	planeInput2->dodgeForce = 50.0f;
	player2->addComponent( planeInput2 );
	life2 = new Life;
	player2->addComponent( life2 );
	player2PreCollide = new PlayerPreCollide;
	player2->addComponent( player2PreCollide );

	player2Particle = new Particle;
	player2->addComponent( player2Particle );
	player2Particle->collisionRadius = 5;
	player2Particle->freezeZ = true;
	renderableGameObject2->addComponent(animate2 = new AnimationRenderingInfo );
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

	Light* light = GraphicsLightManager::global.addLight();
	lightBulb = GameObjectManager::globalGameObjectManager.addGameObject();
	lightBulb->addComponent( light );
	//view->addChild( lightBulb );
	light->setColor( glm::vec4( 1 , 1 , 1 , 1 ) );
	lightBulb->translate = glm::vec3( 0 , 0 , 50);

	Camera* lightCamera = GraphicsCameraManager::globalCameraManager.addCamera();
	lightCamera->initializeRenderManagers();
	lightCamera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	lightCamera->FOV = 60.0f;
	lightCamera->nearestObject = 0.01f;
	lightCamera->direction = glm::normalize( glm::vec3( 0 , 0 , -1 ) );
	lightCamera->attatchFrameBuffer( 0 , depthTexture = GraphicsTextureManager::globalTextureManager.addTexture( 0 , 1024 , 1024 , 1 , GL_DEPTH_COMPONENT32 , GL_DEPTH_COMPONENT , GL_FLOAT ) );
	lightBulb->addComponent( lightCamera );
	lightCamera->drawFrameBuffer = true;
	lightCamera->drawCamera = false;
	light->setDepthTexture( depthTexture );
	

	
	camera = GraphicsCameraManager::globalCameraManager.addCamera();
	camera->initializeRenderManagers();
	camera->addLights( &GraphicsLightManager::global );
	camera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	camera->FOV = 60.0f;
	camera->nearestObject = 0.01f;
	view->addComponent( camera );
	view->translate = glm::vec3( 0 , 0 , 50 );
	zoomer = new TwoDZoomCamera;
	zoomer->initialize( 2 );
	zoomer->addGameObjectToTrack( player );
	zoomer->addGameObjectToTrack( player2 );
	zoomer->maxDistance = 100;
	zoomer->minDistance = 20;
	zoomer->zoomScale = 2;
	view->addComponent( zoomer );

	theTex = 0;
		
	defaultColor = glm::vec4( 1 , 1 , 1 , 1 );

	ParticleWorld::global.addParticleToManage( player1Particle );
	ParticleWorld::global.addParticleToManage( player2Particle );
	//ParticleWorld::global.addParticleToManage( unmovableParticle );
	doge1falling = false;
	doge2falling = false;
	initWalls();
	initDogeWords( );
	playMusic();
	Clock::update();
}

void GlobalItems::initWalls()
{
	GeometryInfo* pillarGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/pillar.pmd" , GraphicsBufferManager::globalBufferManager );
	pillarGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	pillarGeo->addShaderStreamedParameter( 1 , PT_VEC4 , 0 , 0 );
	pillarGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );
	pillarGeo->addShaderStreamedParameter( 2 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::NORMAL_OFFSET );
	pillarGeo->addShaderStreamedParameter( 4 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::TANGENT_OFFSET );
	pillarGeo->addShaderStreamedParameter( 5 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::BITANGENT_OFFSET );
	pillarGeo->addShaderStreamedParameter( 6 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGINDEX_OFFSET );
	pillarGeo->addShaderStreamedParameter( 7 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGWEIGHT_OFFSET );

	srand( ( unsigned int ) time( 0 ) );
	if ( walls.size() ) destroyWalls();
	for ( unsigned int i = 0; i < 10; ++i )
	{
		GameObject* wallInstance = GameObjectManager::globalGameObjectManager.addGameObject();
		Renderable* renderable3 = GraphicsRenderingManager::globalRenderingManager.addRenderable();
		renderable3->initialize( 5 , 1 );
		renderable3->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
		renderable3->geometryInfo = pillarGeo;
		renderable3->shaderInfo = shader;
		renderable3->alphaBlendingEnabled = false;
		renderable3->culling = CT_BACK;
		renderable3->addTexture( dogePatternTexture );
		renderable3->setRenderableUniform( "tint" , PT_VEC4 , reinterpret_cast< const void* >( &defaultColor ) );
		wallInstance->addComponent( renderable3 );
		wallInstance->translate =
			glm::vec3( Engine::RandomItemsGenerator::RandomRangedFloat(-100,100),
			Engine::RandomItemsGenerator::RandomRangedFloat( -100 , 100 ) ,
			0 );
		wallInstance->rotate = glm::angleAxis(glm::radians(90.0f),glm::vec3(1,0,0));
		wallInstance->scale = glm::vec3(20,1,20);
		Particle* theParticle = new Particle;
		theParticle->freezeX = true;
		theParticle->freezeY = true;
		theParticle->freezeZ = true;
		theParticle->mass = FLT_MAX;
		theParticle->collisionRadius = 20;

		wallInstance->addComponent( theParticle );
		ParticleWorld::global.addParticleToManage( theParticle );
		walls.push_back( theParticle );
	}
}

void GlobalItems::destroyWalls()
{
	for ( unsigned int i = 0; i < walls.size(); ++i )
	{
		ParticleWorld::global.removeParticleToManage( walls[i] );
		delete walls[i];
	}
	walls.clear();
}

void GlobalItems::updateLevel()
{
	Clock::update();
	//std::cout << player->rotate.x << ", " << player->rotate.y << ", " << player->rotate.z << std::endl;
	if ( !doge1falling && (player->getWorldTranslate().x > 100 || player->getWorldTranslate().x < -100 || player->getWorldTranslate().y > 100 || player->getWorldTranslate().y < -100) )
	{
		playFall();
		doge1falling = true;
	}
	if ( !doge2falling && ( player2->getWorldTranslate( ).x > 100 || player2->getWorldTranslate( ).x < -100 || player2->getWorldTranslate( ).y > 100 || player2->getWorldTranslate( ).y < -100) )
	{
		playFall();
		doge2falling = true;
	}

	if ( doge1falling )
	{
		player->translate.z -= 50 * Clock::dt;
	}
	if ( doge2falling )
	{
		player2->translate.z -= 50 * Clock::dt;
	}

	GameObjectManager::globalGameObjectManager.earlyUpdateParents();
	GameObjectManager::globalGameObjectManager.updateParents();
	ParticleWorld::global.update();
	GameObjectManager::globalGameObjectManager.lateUpdateParents();
	GameObjectManager::globalGameObjectManager.earlyDrawParents();
	lightBulb->getComponent<Camera>()->direction = glm::normalize( ( 0.5f * ( player->translate + player2->translate ) ) - lightBulb->translate );
	if ( !player->getComponent<Life>( )->changeLife( 0 ) || player->getWorldTranslate( ).z < -100 )
	{
		changeState( GameStates::Player2 );
	}
	else if ( !player2->getComponent<Life>( )->changeLife( 0 ) || player2->getWorldTranslate( ).z < -100 )
	{
		changeState( GameStates::Player1 );
	}
}
void GlobalItems::drawLevel()
{
	GraphicsSharedUniformManager::globalSharedUniformManager.updateLights();
	GraphicsCameraManager::globalCameraManager.drawAllCameras();
	GameObjectManager::globalGameObjectManager.lateDrawParents();
}
void GlobalItems::destroyLevel()
{
	destroyDogeWords();
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
	if ( animate1 ) delete animate1;
	animate1 = 0;
	if ( animate2 ) delete animate2;
	animate2 = 0;

	ParticleWorld::global.removeParticleToManage( player1Particle );
	ParticleWorld::global.removeParticleToManage( player2Particle );
	if ( player1Particle ) delete player1Particle;
	player1Particle = 0;
	if ( player2Particle ) delete player2Particle;
	player2Particle = 0;
	if ( player1PreCollide ) delete player1PreCollide;
	player1PreCollide = 0;
	if ( player2PreCollide ) delete player2PreCollide;
	player2PreCollide = 0;

	destroyWalls();
}

void GlobalItems::initStart()
{
	CommonGraphicsCommands::initializeGlobalGraphics();
	std::string errors;
	std::string vert = FileReader( "assets/shaders/FlatVertex.glsl" );
	std::string frag = FileReader( "assets/shaders/FlatFragment.glsl" );
	ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str() , frag.c_str() , &errors );
	std::cout << errors.c_str() << std::endl;

	GeometryInfo* levelGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/level.pmd" , GraphicsBufferManager::globalBufferManager );
	levelGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	levelGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );

	TextureInfo* levelTexture = GraphicsTextureManager::globalTextureManager.addTexture( "assets/textures/Title.tex" );

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

void GlobalItems::initPlayer1Win()
{
	CommonGraphicsCommands::initializeGlobalGraphics();
	GraphicsCameraManager::globalCameraManager.initialize( 2 );
	std::string errors;
	std::string vert = FileReader( "assets/shaders/DiffuseVertex.glsl" );
	std::string frag = FileReader( "assets/shaders/DiffuseFragment.glsl" );
	ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str() , frag.c_str() , &errors );
	std::cout << errors.c_str() << std::endl;

	initPlayerTextures();
	GeometryInfo* geometry = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/doge.pmd" , GraphicsBufferManager::globalBufferManager );
	geometry->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	geometry->addShaderStreamedParameter( 1 , PT_VEC4 , 0 , 0 );
	geometry->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );
	geometry->addShaderStreamedParameter( 2 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::NORMAL_OFFSET );
	geometry->addShaderStreamedParameter( 4 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::TANGENT_OFFSET );
	geometry->addShaderStreamedParameter( 5 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::BITANGENT_OFFSET );
	geometry->addShaderStreamedParameter( 6 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGINDEX_OFFSET );
	geometry->addShaderStreamedParameter( 7 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGWEIGHT_OFFSET );


	Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable( );
	renderable->initialize( 5 , 1 );
	renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable->geometryInfo = geometry;
	renderable->shaderInfo = shader;
	renderable->alphaBlendingEnabled = false;
	renderable->culling = CT_NONE;
	renderable->addTexture( GlobalItems::global.player1Texture );

	GameObjectManager::globalGameObjectManager.initialize( );

	player = GameObjectManager::globalGameObjectManager.addGameObject( );

	player->addComponent( renderable );

	player->addComponent( animate1 = new AnimationRenderingInfo );
	theTex = 0;
	player->rotate = glm::rotate( glm::quat() , glm::radians(-100.0f) , glm::vec3( 0 , 1 , 0 ) );
	player->translate = glm::vec3(0 , -0.5f , -3.0f);
	GameObject* view = GameObjectManager::globalGameObjectManager.addGameObject( );

	Light* light = GraphicsLightManager::global.addLight( );
	lightBulb = GameObjectManager::globalGameObjectManager.addGameObject( );
	lightBulb->addComponent( light );
	//view->addChild( lightBulb );
	light->setColor( glm::vec4( 1 , 1 , 1 , 1 ) );
	lightBulb->translate = glm::vec3( 0 , 0 , 10 );

	Camera* lightCamera = GraphicsCameraManager::globalCameraManager.addCamera( );
	lightCamera->initializeRenderManagers( );
	lightCamera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	lightCamera->FOV = 60.0f;
	lightCamera->nearestObject = 0.01f;
	lightCamera->direction = glm::normalize( glm::vec3( 0 , 0 , -1 ) );
	lightCamera->attatchFrameBuffer( 0 , depthTexture = GraphicsTextureManager::globalTextureManager.addTexture( 0 , 1024 , 1024 , 1 , GL_DEPTH_COMPONENT32 , GL_DEPTH_COMPONENT , GL_FLOAT ) );
	lightBulb->addComponent( lightCamera );
	lightCamera->drawFrameBuffer = true;
	lightCamera->drawCamera = false;
	light->setDepthTexture( depthTexture );



	camera = GraphicsCameraManager::globalCameraManager.addCamera( );
	camera->initializeRenderManagers( );
	camera->addLights( &GraphicsLightManager::global );
	camera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	camera->FOV = 60.0f;
	camera->nearestObject = 0.01f;
	view->addComponent( camera );
	view->translate = glm::vec3( 0 , 0 , 1.0f );
	winSpawnTime = 0.5f;
	initDogeWinWords();
	//CommonGraphicsCommands::initializeGlobalGraphics( );
	//std::string errors;
	//std::string vert = FileReader( "assets/shaders/FlatVertex.glsl" );
	//std::string frag = FileReader( "assets/shaders/FlatFragment.glsl" );
	//ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str( ) , frag.c_str( ) , &errors );
	//std::cout << errors.c_str( ) << std::endl;

	//GeometryInfo* levelGeo = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/level.pmd" , GraphicsBufferManager::globalBufferManager );
	//levelGeo->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	//levelGeo->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );

	//TextureInfo* levelTexture = GraphicsTextureManager::globalTextureManager.addTexture( "assets/textures/Player1Win.tex" );

	//GameObjectManager::globalGameObjectManager.initialize( 2 );

	//level = GameObjectManager::globalGameObjectManager.addGameObject( );
	//Renderable* renderable3 = GraphicsRenderingManager::globalRenderingManager.addRenderable( );
	//renderable3->initialize( 5 , 1 );
	//renderable3->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	//renderable3->geometryInfo = levelGeo;
	//renderable3->shaderInfo = shader;
	//renderable3->alphaBlendingEnabled = false;
	//renderable3->culling = CT_NONE;
	//renderable3->addTexture( levelTexture );
	//level->addComponent( renderable3 );
	//theTex = 0;

	//GameObject* view = GameObjectManager::globalGameObjectManager.addGameObject( );
	//camera = GraphicsCameraManager::globalCameraManager.addCamera( );
	//camera->initializeRenderManagers( );
	//camera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	//camera->FOV = 60.0f;
	//camera->nearestObject = 0.01f;
	//view->addComponent( camera );
}
void GlobalItems::drawPlayer1Win()
{
	GraphicsSharedUniformManager::globalSharedUniformManager.updateLights( );
	GraphicsCameraManager::globalCameraManager.drawAllCameras();
	GameObjectManager::globalGameObjectManager.lateDrawParents();
}
void GlobalItems::updatePlayer1Win()
{
	Clock::update();
	winSpawnTime -= Clock::dt;
	if ( winSpawnTime <= 0 )
	{
		addWin();
		winSpawnTime = 0.5f;
	}
	GameObjectManager::globalGameObjectManager.earlyUpdateParents();
	GameObjectManager::globalGameObjectManager.updateParents();
	GameObjectManager::globalGameObjectManager.lateUpdateParents();
	GameObjectManager::globalGameObjectManager.earlyDrawParents();
	if ( KeyInput::isDown( VK_SPACE ) ) changeState( GameStates::Start );
}
void GlobalItems::destroyPlayer1Win()
{
	destroyDogeWinWords();
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

void GlobalItems::initPlayer2Win()
{
	CommonGraphicsCommands::initializeGlobalGraphics( );
	GraphicsCameraManager::globalCameraManager.initialize( 2 );
	std::string errors;
	std::string vert = FileReader( "assets/shaders/DiffuseVertex.glsl" );
	std::string frag = FileReader( "assets/shaders/DiffuseFragment.glsl" );
	ShaderInfo* shader = GraphicsShaderManager::globalShaderManager.createShaderInfo( vert.c_str( ) , frag.c_str( ) , &errors );
	std::cout << errors.c_str( ) << std::endl;

	initPlayerTextures( );
	GeometryInfo* geometry = GraphicsGeometryManager::globalGeometryManager.addPMDGeometry( "assets/models/doge.pmd" , GraphicsBufferManager::globalBufferManager );
	geometry->addShaderStreamedParameter( 0 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::POSITION_OFFSET );
	geometry->addShaderStreamedParameter( 1 , PT_VEC4 , 0 , 0 );
	geometry->addShaderStreamedParameter( 3 , PT_VEC2 , VertexInfo::STRIDE , VertexInfo::UV_OFFSET );
	geometry->addShaderStreamedParameter( 2 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::NORMAL_OFFSET );
	geometry->addShaderStreamedParameter( 4 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::TANGENT_OFFSET );
	geometry->addShaderStreamedParameter( 5 , PT_VEC3 , VertexInfo::STRIDE , VertexInfo::BITANGENT_OFFSET );
	geometry->addShaderStreamedParameter( 6 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGINDEX_OFFSET );
	geometry->addShaderStreamedParameter( 7 , PT_VEC4 , VertexInfo::STRIDE , VertexInfo::BLENDINGWEIGHT_OFFSET );


	Renderable* renderable = GraphicsRenderingManager::globalRenderingManager.addRenderable( );
	renderable->initialize( 5 , 1 );
	renderable->sharedUniforms = &GraphicsSharedUniformManager::globalSharedUniformManager;
	renderable->geometryInfo = geometry;
	renderable->shaderInfo = shader;
	renderable->alphaBlendingEnabled = false;
	renderable->culling = CT_NONE;
	renderable->addTexture( GlobalItems::global.player2Texture );

	GameObjectManager::globalGameObjectManager.initialize( );

	player = GameObjectManager::globalGameObjectManager.addGameObject( );

	player->addComponent( renderable );

	player->addComponent( animate1 = new AnimationRenderingInfo );
	theTex = 0;
	player->rotate = glm::rotate( glm::quat( ) , glm::radians( -100.0f ) , glm::vec3( 0 , 1 , 0 ) );
	player->translate = glm::vec3( 0 , -0.5f , -3.0f );
	GameObject* view = GameObjectManager::globalGameObjectManager.addGameObject( );

	Light* light = GraphicsLightManager::global.addLight( );
	lightBulb = GameObjectManager::globalGameObjectManager.addGameObject( );
	lightBulb->addComponent( light );
	//view->addChild( lightBulb );
	light->setColor( glm::vec4( 1 , 1 , 1 , 1 ) );
	lightBulb->translate = glm::vec3( 0 , 0 , 10 );

	Camera* lightCamera = GraphicsCameraManager::globalCameraManager.addCamera( );
	lightCamera->initializeRenderManagers( );
	lightCamera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	lightCamera->FOV = 60.0f;
	lightCamera->nearestObject = 0.01f;
	lightCamera->direction = glm::normalize( glm::vec3( 0 , 0 , -1 ) );
	lightCamera->attatchFrameBuffer( 0 , depthTexture = GraphicsTextureManager::globalTextureManager.addTexture( 0 , 1024 , 1024 , 1 , GL_DEPTH_COMPONENT32 , GL_DEPTH_COMPONENT , GL_FLOAT ) );
	lightBulb->addComponent( lightCamera );
	lightCamera->drawFrameBuffer = true;
	lightCamera->drawCamera = false;
	light->setDepthTexture( depthTexture );



	camera = GraphicsCameraManager::globalCameraManager.addCamera( );
	camera->initializeRenderManagers( );
	camera->addLights( &GraphicsLightManager::global );
	camera->addRenderList( &GraphicsRenderingManager::globalRenderingManager );
	camera->FOV = 60.0f;
	camera->nearestObject = 0.01f;
	view->addComponent( camera );
	view->translate = glm::vec3( 0 , 0 , 1.0f );
	winSpawnTime = 0.5f;
	initDogeWinWords( );
}
void GlobalItems::drawPlayer2Win()
{
	GraphicsSharedUniformManager::globalSharedUniformManager.updateLights( );
	GraphicsCameraManager::globalCameraManager.drawAllCameras();
	GameObjectManager::globalGameObjectManager.lateDrawParents();
}
void GlobalItems::updatePlayer2Win()
{
	Clock::update();
	winSpawnTime -= Clock::dt;
	if ( winSpawnTime <= 0 )
	{
		addWin( );
		winSpawnTime = 0.5f;
	}
	GameObjectManager::globalGameObjectManager.earlyUpdateParents();
	GameObjectManager::globalGameObjectManager.updateParents();
	GameObjectManager::globalGameObjectManager.lateUpdateParents();
	GameObjectManager::globalGameObjectManager.earlyDrawParents();
	if ( KeyInput::isDown( VK_SPACE ) ) changeState( GameStates::Start );
}
void GlobalItems::destroyPlayer2Win()
{
	destroyDogeWinWords();
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
			initPlayer1Win();
			break;
		case Player2:
			initPlayer2Win();
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
			updatePlayer1Win();
			break;
		case Player2:
			updatePlayer2Win();
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
			drawPlayer1Win();
			break;
		case Player2:
			drawPlayer2Win();
			break;
		default:
			break;
	}
}

GlobalItems::GlobalItems() :audio(0) , player1Texture(0) , player2Texture(0) , dogePatternTexture(0) , player(0) , player2(0) , level(0) , life1(0) , life2(0) , gun1(0) , gun2(0) , camera(0) , zoomer(0) , planeInput(0) , planeInput2(0) , player1Particle(0) , player2Particle(0) , player1PreCollide(0) , player2PreCollide(0) , state(GameStates::None) , animate1(0) , animate2(0)
{
	ParticleWorld::global.initialize();
}

void GlobalItems::destroyAll()
{
	destroyAudio();
	destroyLevel();
	destroyStart();
	destroyPlayer1Win();
	destroyPlayer2Win();
	CommonGraphicsCommands::destroyGlobalGraphics();
	GameObjectManager::globalGameObjectManager.destroy();
}

GlobalItems::~GlobalItems()
{
	destroyAll();
}