#include "teipch.h"
#include "TextComponent.h"

#include <tei/render.h>
#include <tei/resource.h>

#include <SDL.h>
#include <SDL_ttf.h>

using namespace tei::internal::render;
using namespace tei::internal::components;
using namespace tei::internal::resource;
using namespace tei::internal::utility;

void OnUpdate(std::nullptr_t, TextRenderComponent const& comp)
{
	if (auto& [text, font, texture, transform] = comp.refs; text.checkout())
	{
		if (auto data = std::exchange(texture->pData, {}))
			SDL_DestroyTexture(static_cast<SDL_Texture*>(data));

		if (!text->empty())
		{
			const auto surf = TTF_RenderText_Blended(static_cast<TTF_Font*>(font->pData), text->c_str(), { 255, 255, 255 });
			if (surf == nullptr)
				throw TeiRuntimeError{ "Rendering text failed", SDL_GetError() };

			SDL_Texture* pTexture = SDL_CreateTextureFromSurface(static_cast<SDL_Renderer*>(Renderer->GetRenderTraget().pData), surf);
			SDL_FreeSurface(surf);

			int w{}, h{};
			if (pTexture == nullptr || SDL_QueryTexture(pTexture, nullptr, nullptr, &w, &h) != 0)
				throw TeiRuntimeError{ "Creating texture from surface failed", SDL_GetError() };

			texture->pData = pTexture;
			texture->size = { w, h };
		}
	}
}

void OnRender(std::nullptr_t, TextRenderComponent const& comp)
{
	auto& [text, font, texture, transform] = comp.refs;
	Renderer->DrawTexture(*texture, transform.world);
}
