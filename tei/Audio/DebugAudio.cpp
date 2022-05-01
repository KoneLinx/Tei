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

	void DebugAudio::OnPlay(resource::Sound const& sound)
	{
		std::cout << "[audio] " <<
#if defined(DEBUG) || defined(_DEBUG)
			sound._name
#else
			Sound.pData
#endif
		<< '\n';
	}

	void DebugAudio::OnMute(bool muted)
	{
		puts(muted ? "Debug audio muted" : "Debug audio unmuted");
	}

	Chunk* DebugAudio::Load(std::filesystem::path const& path)
	{
		static size_t id{};

		std::cout << "[audio] Loading audio chunk " << id++ << ": " << path.filename() << '\n';

		return std::bit_cast<Chunk*>(id);
	}

	void DebugAudio::Free(Chunk* id)
	{
		std::cout << "[audio] Freeing audio chunk " << std::bit_cast<size_t>(id) << '\n';
	}

}