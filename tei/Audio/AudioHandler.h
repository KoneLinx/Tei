
#pragma once

#include <tei/utility.h>
#include <tei/resource.h>

#include <span>

namespace tei::internal::audio
{

	class AudioHandler
	{
	public:

		AudioHandler() = default;
		virtual ~AudioHandler() = default;

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

		virtual void OnUpdate(std::span<std::reference_wrapper<resource::Sound const>> requests) = 0;

		bool m_Muted{};

		// TODO Fix!
		mutable bool m_SetMuted{};
		mutable std::vector<std::reference_wrapper<resource::Sound const>> m_Requests{};

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
	constexpr static auto const& Audio{ tei::internal::audio::Audio };
}

#pragma region _impl

namespace tei::internal::audio
{

	inline void AudioHandler::Play(resource::Sound const& sound) const
	{
		if (!m_Muted)
			m_Requests.push_back(sound);
	}

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