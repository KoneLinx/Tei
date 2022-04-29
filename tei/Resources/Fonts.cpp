#include "teipch.h"
#include "Fonts.h"

#include <SDL_ttf.h>

using namespace tei::internal::resource;
using namespace tei::internal;
using namespace std::literals;

void Load(std::shared_ptr<Font>& out, std::filesystem::path const& path, int ptSize)
{
    static bool init{
        TTF_Init() == 0 ||
        (throw std::runtime_error{ "Failed to load SDL_ttf: "s + SDL_GetError() }, false)
    };
    
    TTF_Font* pFont{ TTF_OpenFont(path.string().c_str(), ptSize) };
    if (pFont == nullptr)
        throw utility::TeiRuntimeError{ "Failed to load font: " + path.string(), SDL_GetError() };

    constexpr static auto DELETER = [](Font* pFont)
    {
        TTF_CloseFont(static_cast<TTF_Font*>(pFont->pData));
        delete pFont;
    };

    out = { std::shared_ptr<Font>{ new Font{ pFont, ptSize }, DELETER } };
}