#pragma once
#include <Audio\AudioController.h>
#include "DebugMemory.h"

struct GlobalItems
{
	AudioController* audio;
	void playHit();
	void playMusic();
	void initAudio();
	void destroyAudio();
	GlobalItems();
	~GlobalItems();

	static GlobalItems global;
};