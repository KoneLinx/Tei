#pragma once

#include <memory>
#include <filesystem>
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

}

namespace tei::external::resource
{
	using tei::internal::resource::Texture;
	using tei::internal::resource::Sprite;
}

void Load(
	std::shared_ptr<tei::external::resource::Texture>&,
	std::filesystem::path const&
);

void Load(
	std::shared_ptr<tei::external::resource::Texture>&
);

void Load(
	std::shared_ptr<tei::external::resource::Sprite>&, 
	std::filesystem::path const&,
	tei::external::Clock::duration frameduration,
	int cols,
	int rows,
	bool loop = true,
	tei::external::Clock::time_point origin = tei::external::Time->frame.now
);

void Load(
	std::shared_ptr<tei::external::resource::Sprite>&
);
