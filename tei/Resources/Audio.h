#pragma once

#include <memory>
#include <filesystem>

namespace tei::internal::resource
{

	struct Sound
	{
		void* pData;
		float volume;
		bool loop;
#if defined(DEBUG) || defined(_DEBUG)
		std::string _name;
#endif

		constexpr operator bool() const noexcept
		{
			return this != nullptr && pData != nullptr;
		}
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
	float volume = .1f,
	std::string_view name = {}
);

void Load(
	std::shared_ptr<tei::external::resource::Sound>&
);
