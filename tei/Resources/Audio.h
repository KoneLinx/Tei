#pragma once

#include "ResourceManager.h"

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

tei::external::Resource<tei::external::resource::Sound>
Load(
	tei::external::resource::ToLoad<tei::external::resource::Sound>,
	std::filesystem::path const&,
	bool loop = false,
	float volume = .1f,
	std::string_view name = {}
);

tei::external::Resource<tei::external::resource::Sound>
Load(tei::external::resource::ToLoad<tei::external::resource::Sound>);