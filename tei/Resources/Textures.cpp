#include "Textures.h"

#include <tei/render.h>
#include <SDL_image.h>
#include "Textures.h"

using namespace tei::internal::resource;

prefab::TextureLoader::TextureLoader()
{
	METRICS_TIMEBLOCK;

	int init{
		bool(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) &
		bool(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) 
	};
	if (!init)
		throw utility::TeiRuntimeError{ "Could not properly initialize SDL_image", SDL_GetError() };
}

Texture* prefab::TextureLoader::OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<Texture> init) const
{
	METRICS_TIMEBLOCK;

	auto const file = resources.GetExisting(path);

	auto const pathStr{ file.string() };

	if (!init) init = Texture{};

	SDL_Texture* pTexture = IMG_LoadTexture(
		static_cast<SDL_Renderer*>(render::Renderer->GetRenderTraget().pData),
		pathStr.c_str()
	);
	init->pData = pTexture;

	if (pTexture == nullptr ||
		SDL_QueryTexture(
			pTexture, 
			nullptr,
			nullptr, 
			&init->w, 
			&init->h
		) != 0
	)
		throw utility::TeiRuntimeError{ "Could not load texture: " + pathStr, SDL_GetError() };

	return new Texture{ *std::move(init) };
}

void prefab::TextureLoader::OnFree(ResourceManager const&, Texture* texture) const
{
	METRICS_TIMEBLOCK;

	SDL_DestroyTexture(static_cast<SDL_Texture*>(texture->pData));
	delete texture;
}
