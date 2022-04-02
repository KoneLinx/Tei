#include "Textures.h"

#include <mutex>

#include <tei/internal/Utility/Error.h>
#include <tei/render.h>
#include <SDL_image.h>
#include "Textures.h"

using namespace tei::internal::resource;
using namespace tei::internal;

Texture LoadTexture(std::filesystem::path const& path)
{
	//static std::once_flag once{};
	//std::call_once(
	//	once,
	//	[]
	//	{
	//		bool(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) &&
	//		bool(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) ||
	//		(throw utility::TeiRuntimeError{ "Could not properly initialize SDL_image", SDL_GetError() }, false);
	//	}
	//);

	Texture texture{};

	if (!path.empty())
	{
		SDL_Texture* pTexture = IMG_LoadTexture(
			static_cast<SDL_Renderer*>(render::Renderer->GetRenderTraget().pData),
			path.string().c_str()
		);

		texture.pData = pTexture;

		if (pTexture == nullptr)
			throw utility::TeiRuntimeError{ "Could not load texture: " + path.string(), SDL_GetError() };

		int w{}, h{};
		if (SDL_QueryTexture(pTexture, nullptr, nullptr, &w, &h) != 0)
			throw utility::TeiRuntimeError{ "Could not querry texture: " + path.string(), SDL_GetError() };
		
		texture.size = { w, h };
	}

	return texture;
}

static constexpr auto  DELETER = [] (auto* pTexture) // Texture or Sprite
{
	if (pTexture->pData)
	{
		if (pTexture->pData)
			SDL_DestroyTexture(static_cast<SDL_Texture*>(pTexture->pData));
	}
	delete pTexture;
};

void Load(std::shared_ptr<Texture>& ref, std::filesystem::path const& path)
{
	ref = { std::shared_ptr<Texture>{ new Texture{ LoadTexture(path) }, DELETER } };
}
void Load(std::shared_ptr<Texture>& ref)
{
	ref = { std::shared_ptr<Texture>{ new Texture{ LoadTexture({}) }, DELETER } };
}

void Load(std::shared_ptr<Sprite>& ref, std::filesystem::path const& path, time::Clock::duration frameduration, int cols, int rows, bool loop, time::Clock::time_point origin)
{
	ref = { std::shared_ptr<Sprite>{ new Sprite{ LoadTexture(path), frameduration, cols, rows, loop, origin }, DELETER } };
}

