#include "Fonts.h"

#include <SDL_ttf.h>

using namespace tei::internal::resource;
using namespace tei::internal;
using namespace std::literals;

//prefab::FontsLoader::FontsLoader()
//{
//   if(TTF_Init() == -1)
//       throw std::runtime_error{ "Failed to load SDL_ttf: "s + SDL_GetError() };
//}
//
//Font* prefab::FontsLoader::OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<Font> init) const
//{
//    auto const file = resources.GetExisting(path).string();
//
//    if (!init) init = Font{ .ptSize = 27 };
//
//    if ((init->pData = TTF_OpenFont(file.c_str(), init->ptSize)) == nullptr)
//        throw std::runtime_error{ "Failed to load font: " + file + ": " + SDL_GetError() };
//
//    return new Font{ *move(init) };
//}
//
//void prefab::FontsLoader::OnFree(ResourceManager const&, Font* font) const
//{
//    TTF_CloseFont(static_cast<TTF_Font*>(font->pData));
//    delete font;
//}

Resource<Font> Load(ToLoad<Font>, std::filesystem::path const& path, int ptSize)
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

    return { std::shared_ptr<Font>{ new Font{ pFont, ptSize }, DELETER } };
}