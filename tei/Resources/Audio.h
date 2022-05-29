#pragma once

#include <memory>
#include <filesystem>

namespace tei::internal::audio
{
	struct Chunk;
}

namespace tei::internal::resource
{

	struct Sound
	{
		audio::Chunk* pData;
		float volume{};
		bool loop{};
	};

}

namespace tei::external::resource
{
	using tei::internal::resource::Sound;
}

void Load(
	std::shared_ptr<tei::external::resource::Sound>&,
	std::filesystem::path const&,
	bool loop = false,
	float volume = .1f
);

void Load(
	std::shared_ptr<tei::external::resource::Sound>&
);
