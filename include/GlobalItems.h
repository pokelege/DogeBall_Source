#pragma once
#include <Audio\AudioController.h>
#include <DebugMemory.h>
class GameObject;
struct TextureInfo;
class Life;
class Camera;
class TwoDZoomCamera;
class TwoDPlaneInput;
class Gun;
class Particle;
enum GameStates { None, Start , Level , Player1 , Player2 };
struct GlobalItems
{
	GameObject* player , *player2 , *level;
	Life* life1 , *life2;
	AudioController* audio;
	TextureInfo* player1Texture;
	TextureInfo* player2Texture;
	Gun* gun1 , *gun2;
	Camera* camera;
	TwoDZoomCamera* zoomer;
	TwoDPlaneInput* planeInput , *planeInput2;
	Particle* player1Particle , *player2Particle;
	GameStates state;
	int theTex;
	glm::vec4 defaultColor;
	void initPlayerTextures();
	void initLevel();
	void drawLevel();
	void updateLevel();
	void destroyLevel();

	void initStart();
	void drawStart();
	void updateStart();
	void destroyStart();

	void initPlayer1Win();
	void drawPlayer1Win();
	void updatePlayer1Win();
	void destroyPlayer1Win();

	void initPlayer2Win();
	void drawPlayer2Win();
	void updatePlayer2Win();
	void destroyPlayer2Win();

	void playHit();
	void playMusic();
	void initAudio();
	void destroyAudio();
	void destroyAll();
	void changeState(GameStates state);
	void update();
	void draw();
	GlobalItems();
	~GlobalItems();

	static GlobalItems global;
};