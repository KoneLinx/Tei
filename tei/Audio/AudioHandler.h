#pragma once

#include <tei/utility.h>

#include <array>
#include <filesystem>

#include "../Resources/ResourceManager.h"

namespace tei::internal::resource
{
	struct Sound;
}

namespace tei::internal::audio
{

	class AudioService
	{
	protected:

		AudioService() = default;
	
	public:

		virtual ~AudioService() = default;

		virtual void Play(resource::Resource<resource::Sound> const& sound) const = 0;
		virtual void Mute(bool state) const = 0;
		virtual bool IsMuted() const = 0;

		virtual void Update() = 0;
		virtual std::shared_ptr<resource::Sound> Load(std::filesystem::path const& path) = 0;

		using Service = utility::Service<AudioService const>;

	};

	extern AudioService::Service Audio;

}

namespace tei::external
{
	inline constexpr auto const& Audio{ tei::internal::audio::Audio };
}