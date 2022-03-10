#pragma once

#include "ResourceManager.h"

namespace tei::internal::resource
{

	struct Sound
	{
		void* pData;
		float volume;
		bool loop;
		std::string name;

		constexpr operator bool() const noexcept
		{
			return this != nullptr && pData != nullptr;
		}
	};

	//namespace prefab
	//{

	//	class AudioLoader final : public ResourceManager::template Loader<Sound>
	//	{
	//	public:

	//		AudioLoader();
	//		~AudioLoader();

	//	private:

	//		Sound* OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<Sound> init) const override;

	//		void OnFree(ResourceManager const& resources, Sound* font) const override;

	//	};

	//}

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
