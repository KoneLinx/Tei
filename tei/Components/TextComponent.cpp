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
	if (auto& [text, font, texture, colour, transform] = comp.Refs(); text.checkout() | colour.checkout()) // Non short-circuiting!
	{
		unit::Scale scale{ 1 };

		if (auto data = std::exchange(texture->pData, {}))
		{
			scale = texture->size;
			SDL_DestroyTexture(static_cast<SDL_Texture*>(data));
		}

		if (!text->empty())
		{
			const auto surf = TTF_RenderText_Blended(
				static_cast<TTF_Font*>(font->pData),
				text->c_str(), 
				SDL_Colour{ 
					.r = Uint8(colour->r * 0xFF),
					.g = Uint8(colour->g * 0xFF),
					.b = Uint8(colour->b * 0xFF), 
					.a = Uint8(colour->a * 0xFF)
				}
			);
			if (surf == nullptr)
				throw TeiRuntimeError{ "Rendering text failed", SDL_GetError() };

			SDL_Texture* pTexture = SDL_CreateTextureFromSurface(static_cast<SDL_Renderer*>(Renderer->GetRenderTraget().pData), surf);
			SDL_FreeSurface(surf);

			int w{}, h{};
			if (pTexture == nullptr || SDL_QueryTexture(pTexture, nullptr, nullptr, &w, &h) != 0)
				throw TeiRuntimeError{ "Creating texture from surface failed", SDL_GetError() };

			texture->pData = pTexture;
			texture->size = { w, h };

			scale = texture->size / scale;

			transform.get().scale *= scale.x / scale.y;
		}
	}
}

void OnRender(std::nullptr_t, TextRenderComponent const& comp)
{
	auto& [text, font, texture, colour, transform] = comp.Refs();
	Renderer->DrawTexture(*texture, transform.world);
}

void OnInitialize(std::nullptr_t, tei::internal::components::TextRenderComponent const&, tei::internal::ecs::Object& object)
{
	if (object.HasComponent<Observed<unit::Colour>>() == nullptr) 
		object.AddComponent(Observed{ unit::Colour{ 1, 1, 1, 1 } });
}
