#pragma once

#include "ResourceManager.h"
#include <tei/time.h>

namespace tei::internal::resource
{

	struct Texture
	{
		void* pData;
		int w, h;

		operator bool() const noexcept
		{ return this != nullptr && pData != nullptr; }
	};

	struct Sprite
	{
		Sprite() noexcept = default;

		Sprite(Texture& texture, time::Clock::duration frameduration, int cols, int rows, bool loop = true, time::Clock::time_point origin = time::Time->frame.now) noexcept
			: texture{ texture }, cols{ cols }, rows{ rows }, loop{ loop }, origintime{ origin }, frameduration{ frameduration }
		{}

		std::reference_wrapper<Texture> texture;
		int cols, rows;
		time::Clock::time_point origintime;
		time::Clock::duration frameduration;
		bool loop;
	};

	namespace prefab
	{

		class TextureLoader final : public ResourceManager::template Loader<Texture>
		{
		public:

			TextureLoader();

		private:

			Texture* OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<Texture> init) const override;

			void OnFree(ResourceManager const& resources, Texture* init) const override;

		};

	}

}

namespace tei::external::resource
{
	using tei::internal::resource::Texture;
	using tei::internal::resource::Sprite;
}