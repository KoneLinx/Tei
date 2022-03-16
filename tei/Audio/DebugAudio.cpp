#include "DebugAudio.h"
#include <iostream>

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

	void DebugAudio::OnUpdate(std::span<std::reference_wrapper<resource::Sound const>> log)
	{
		for (resource::Sound const& sound : log)
			std::cout << "[audio] " <<
#if defined(DEBUG) || defined(_DEBUG)
				sound.name 
#else
				sound.pData
#endif
			<< '\n';
	}

	void DebugAudio::OnMute(bool muted)
	{
		puts(muted ? "Debug audio muted" : "Debug audio unmuted");
	}

}