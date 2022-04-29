#include "teipch.h"
#include "DebugAudio.h"
#include <iostream>

#include "../Resources/Audio.h"

namespace tei::internal::audio
{

	void DebugAudio::OnEnable()
	{
		puts("Debug audio enabled");
	}

	void DebugAudio::OnDisable()
	{
		puts("Debug audio disabled");
	}

	void DebugAudio::OnUpdate(std::span<resource::Sound const*> log)
	{
		for (resource::Sound const* pSound : log)
			std::cout << "[audio] " <<
#if defined(DEBUG) || defined(_DEBUG)
				pSound->_name 
#else
				pSound->pData
#endif
			<< '\n';
	}

	void DebugAudio::OnMute(bool muted)
	{
		puts(muted ? "Debug audio muted" : "Debug audio unmuted");
	}

}