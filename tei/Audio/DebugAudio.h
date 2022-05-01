#pragma once
#include "AudioHandler.h"

namespace tei::internal::audio
{

	class DebugAudio : public AudioHandler
	{
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnPlay(resource::Sound const&) override;
		virtual void OnMute(bool) override;
		virtual struct Chunk* Load(std::filesystem::path const&) override;
		virtual void Free(struct Chunk*) override;
	};

}