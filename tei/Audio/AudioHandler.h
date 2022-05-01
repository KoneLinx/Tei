#pragma once

#include <tei/utility.h>

#include <array>
#include <filesystem>

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

		virtual void Play(resource::Sound const& sound) const final; //const

		virtual void Mute(bool mute) const final;
		virtual bool IsMuted() const noexcept final;

		virtual void Enable() final;
		virtual void Disable() final;
		virtual void Update() final;

		virtual struct Chunk* Load(std::filesystem::path const& path) = 0;
		virtual void Free(struct Chunk*) = 0;

	private:

		virtual void OnEnable() {}
		virtual void OnDisable() {}
		virtual void OnMute(bool mute) { (void)mute; }

		virtual void OnPlay(resource::Sound const& sound) = 0;

		std::unique_ptr<struct Queue> m_Queue{};

		struct ServiceRegisterer
		{
			void operator () (AudioHandler* old, AudioHandler* young) const;
		};

	public:

		using Service = tei::internal::utility::Service<AudioHandler, ServiceRegisterer>;

	};

	extern AudioHandler::Service Audio;

}

namespace tei::external
{
	inline constexpr auto const& Audio{ tei::internal::audio::Audio };
}