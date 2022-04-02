#pragma once

#include <memory>
#include <filesystem>

namespace tei::internal::resource
{

	struct Font
	{
		void* pData;
		int ptSize;

		constexpr operator bool() const noexcept
		{ return this != nullptr && pData != nullptr; }
	};

}

namespace tei::external::resource
{
	using tei::internal::resource::Font;
}

void Load(
	std::shared_ptr<tei::external::resource::Font>&,
	std::filesystem::path const&,
	int ptSize = 11
);
