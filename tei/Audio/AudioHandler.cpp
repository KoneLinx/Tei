#include "teipch.h"
#include "AudioHandler.h"

namespace tei::internal::audio
{
	struct Queue
	{
		static constexpr inline unsigned QUEUE_SIZE{ 8 };
		std::array<resource::Sound const*, QUEUE_SIZE> buffer{};
		unsigned head{}, tail{};
		bool open{ true }, setOpen{ true };
	};

	AudioHandler::Service Audio{};

	AudioHandler::AudioHandler()
		: m_Queue{ std::make_unique<Queue>() }
	{}

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
		if (m_Queue->open != std::exchange(m_Queue->open, m_Queue->setOpen))
			this->OnMute(m_Queue->open);

		if (m_Queue->open)
			for (; m_Queue->head != m_Queue->tail; m_Queue->head = (m_Queue->head + 1) % Queue::QUEUE_SIZE)
				OnPlay(*std::exchange(m_Queue->buffer[m_Queue->head], {}));
	}

	void AudioHandler::Play(resource::Sound const& sound) const
	{
		if (m_Queue->open)
		{
			if (m_Queue->buffer[m_Queue->tail] != nullptr)
			{
				puts("[WARNING] Audio sound queue is full. Skipping current call");
				return;
			}
			m_Queue->buffer[m_Queue->tail] = &sound;
			m_Queue->tail = (m_Queue->tail + 1) % Queue::QUEUE_SIZE;
		}
	}

	void AudioHandler::ServiceRegisterer::operator()(AudioHandler* old, AudioHandler* young) const
	{
		if (old) old->Disable();
		if (young) young->Enable();
		if (old && young) young->Mute(old->IsMuted());
	}

	void AudioHandler::Mute(bool mute = true) const
	{
		m_Queue->setOpen = !mute;
	}

	bool AudioHandler::IsMuted() const noexcept
	{
		return m_Queue->open;
	}

}