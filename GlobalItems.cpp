#include "GlobalItems.h"
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

GlobalItems::GlobalItems() :audio(0)
{
	
}

GlobalItems::~GlobalItems()
{
	if ( audio ) destroyAudio();
}