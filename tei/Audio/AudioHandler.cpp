#include "teipch.h"
#include "AudioHandler.h"

namespace tei::internal::audio
{

	AudioHandler::Service Audio{};

	void AudioHandler::Enable()
	{
		this->OnEnable();
	}

	void AudioHandler::Disable()
	{
		this->OnDisable();
	}

	void AudioHandler::Update()
	{
		if (!m_Muted && !empty(m_Requests))
		{
			this->OnUpdate(m_Requests);
		}
		m_Requests.clear();
		if (m_Muted != m_SetMuted)
			this->OnMute(m_Muted = m_SetMuted);
	}

	void AudioHandler::ServiceRegisterer::operator()(AudioHandler* old, AudioHandler* young) const
	{
		if (old) old->Disable();
		if (young) young->Enable();
		if (old && young) young->Mute(old->IsMuted());
	}

}