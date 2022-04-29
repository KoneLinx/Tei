#include "teipch.h"
#include "AudioHandler.h"

namespace tei::internal::audio
{

	AudioHandler::Service Audio{};

	AudioHandler::AudioHandler()
	{
		m_pRequests.store(std::make_shared<RequestQueue>());
	}

	AudioHandler::~AudioHandler()
	{}

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
		if (m_Muted != std::exchange(m_Muted, m_SetMuted))
			this->OnMute(m_Muted);

		if (!m_Muted)
		{
			auto pQueue{ m_pRequests.exchange(std::make_shared<RequestQueue>()) };
			while (pQueue.use_count() != 1) 
				std::this_thread::yield(); // Pending request
			if (!pQueue->empty())
				this->OnUpdate(*pQueue);
			// pQueue use_count -> 0: gets deleted
		}
	}

	void AudioHandler::Play(resource::Sound const& sound) const
	{
		if (!m_Muted)
			m_pRequests.load()->push_back(&sound);
	}

	void AudioHandler::ServiceRegisterer::operator()(AudioHandler* old, AudioHandler* young) const
	{
		if (old) old->Disable();
		if (young) young->Enable();
		if (old && young) young->Mute(old->IsMuted());
	}

}