#pragma once
#include "AudioHandler.h"

namespace tei::internal::audio
{

	class SDLAudio : public AudioHandler
	{

		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnMute(bool) override;
		virtual void OnUpdate(std::span<std::reference_wrapper<resource::Sound const>> requests) override;

	};

}

