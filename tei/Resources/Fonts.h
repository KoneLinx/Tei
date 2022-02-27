#pragma once

#include "ResourceManager.h"

namespace tei::internal::resource
{

	struct Font
	{
		void* pData;
		int ptSize;

		constexpr operator bool() const noexcept
		{ return this != nullptr && pData != nullptr; }
	};

	namespace prefab
	{

		class FontsLoader final : public ResourceManager::template Loader<Font>
		{
		public:

			FontsLoader();

		private:

			Font* OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<Font> init) const override;

			void OnFree(ResourceManager const& resources, Font* font) const override;

		};

	}

}

namespace tei::external::resource
{
	using tei::internal::resource::Font;
}

