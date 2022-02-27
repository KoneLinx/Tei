#pragma once
#include "AudioHandler.h"

namespace tei::internal::audio
{

	class DebugAudio : public AudioHandler
	{
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnUpdate(std::span<std::reference_wrapper<resource::Sound const>>) override;
		virtual void OnMute(bool) override;
	};

}