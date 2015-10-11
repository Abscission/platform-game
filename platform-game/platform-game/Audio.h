
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include <Windows.h>
#include <xaudio2.h>

class Audio {
	IXAudio2* XAudio;
	IXAudio2MasteringVoice* MasterVoice;
	
public:
	Audio();
	~Audio();
};

