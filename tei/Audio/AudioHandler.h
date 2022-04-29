#pragma once

#include <tei/utility.h>

#include <span>
#include <vector>

namespace tei::internal::resource
{
	struct Sound;
}

namespace tei::internal::audio
{

	class AudioHandler
	{
	public:

		AudioHandler();
		virtual ~AudioHandler();

		virtual void Play(resource::Sound const& sound) const final;

		virtual void Mute(bool mute) const final;
		virtual bool IsMuted() const noexcept final;

		virtual void Enable() final;
		virtual void Disable() final;
		virtual void Update() final;

	private:

		virtual void OnEnable() {}
		virtual void OnDisable() {}
		virtual void OnMute(bool mute) { (void)mute; }

		virtual void OnUpdate(std::span<resource::Sound const*> requests) = 0;

		bool m_Muted{};

		// TODO Fix!
		mutable bool m_SetMuted{};

		using RequestQueue = std::vector<resource::Sound const*>;

		std::atomic<std::shared_ptr<RequestQueue>> m_pRequests{};

		struct ServiceRegisterer
		{
			void operator () (AudioHandler* old, AudioHandler* young) const;
		};

	public:

		using Service = tei::internal::utility::Service<AudioHandler const, ServiceRegisterer>;

	};

	extern AudioHandler::Service Audio;

}

namespace tei::external
{
	inline constexpr auto const& Audio{ tei::internal::audio::Audio };
}

#pragma region _impl

namespace tei::internal::audio
{

	inline void AudioHandler::Mute(bool mute = true) const
	{
		m_SetMuted = mute;
	}
	inline bool AudioHandler::IsMuted() const noexcept
	{
		return m_Muted;
	}

}

#pragma endregion