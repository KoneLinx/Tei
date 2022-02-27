#pragma once

#include "ResourceManager.h"

namespace tei::internal::resource
{

	struct Sound
	{
		void* pData;
		std::string name;
		float volume{ .1f };
		bool loop{};

		constexpr operator bool() const noexcept
		{
			return this != nullptr && pData != nullptr;
		}
	};

	namespace prefab
	{

		class AudioLoader final : public ResourceManager::template Loader<Sound>
		{
		public:

			AudioLoader();
			~AudioLoader();

		private:

			Sound* OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<Sound> init) const override;

			void OnFree(ResourceManager const& resources, Sound* font) const override;

		};

	}

}

namespace tei::external::resource
{
	using tei::internal::resource::Sound;
}

