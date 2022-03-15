#pragma once

#include "ResourceManager.h"
#include <tei/time.h>
#include <tei/unit.h>

namespace tei::internal::resource
{

	struct Texture
	{
		void* pData;
		unit::Scale size;

		operator bool() const noexcept
		{ return this != nullptr && pData != nullptr; }
	};

	struct Sprite : Texture
	{
		Sprite(Texture texture, time::Clock::duration frameduration, int cols, int rows, bool loop = true, time::Clock::time_point origin = time::Time->frame.now) noexcept
			: Texture{ texture }, cols{ cols }, rows{ rows }, loop{ loop }, origintime{ origin }, frameduration{ frameduration }
		{}

		int cols, rows;
		time::Clock::time_point origintime;
		time::Clock::duration frameduration;
		bool loop;
	};

	//namespace prefab
	//{

	//	class TextureLoader final : public ResourceManager::template Loader<Texture>
	//	{
	//	public:

	//		TextureLoader();

	//	private:

	//		Texture* OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<Texture> init) const override;

	//		void OnFree(ResourceManager const& resources, Texture* init) const override;

	//	};

	//}

}

namespace tei::external::resource
{
	using tei::internal::resource::Texture;
	using tei::internal::resource::Sprite;
}

tei::external::Resource<tei::external::resource::Texture>
Load(
	tei::external::resource::ToLoad<tei::external::resource::Texture>,
	std::filesystem::path const&,
	bool nonempty = true
);

tei::external::Resource<tei::external::resource::Sprite> 
Load(
	tei::external::resource::ToLoad<tei::external::resource::Sprite>, 
	std::filesystem::path const&,
	tei::external::Clock::duration frameduration,
	int cols,
	int rows,
	bool loop = true,
	tei::external::Clock::time_point origin = tei::external::Time->frame.now
);
