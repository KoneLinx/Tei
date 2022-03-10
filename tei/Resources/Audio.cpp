#include "Audio.h"

#include <SDL_mixer.h>

using namespace tei::internal::resource;
using namespace tei::internal;
using namespace std::literals;

//prefab::AudioLoader::AudioLoader()
//{
//    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
//        throw utility::TeiRuntimeError{ "Could not open audio", SDL_GetError() };
//}
//prefab::AudioLoader::~AudioLoader()
//{
//    Mix_CloseAudio();
//}
//
//Sound* prefab::AudioLoader::OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<Sound> init) const
//{
//    auto const file = resources.GetExisting(path).string();
//
//    if (!init) init = Sound{
//        .name = path.string()
//    };
//
//    init->pData = Mix_LoadWAV(file.c_str());
//
//    if (init->pData == nullptr)
//        throw utility::TeiRuntimeError{ "Sound chunk could not be loaded", SDL_GetError() };
//
//    return new Sound{ *move(init) };
//}
//
//void prefab::AudioLoader::OnFree(ResourceManager const&, Sound* sound) const
//{
//    Mix_FreeChunk(static_cast<Mix_Chunk*>(sound->pData));
//    delete sound;
//}

Resource<Sound> Load(ToLoad<Sound>, std::filesystem::path const& path, bool loop, float volume, std::string_view name)
{
    struct Audio
    {
        Audio()
        {
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
                throw utility::TeiRuntimeError{ "Could not open audio", SDL_GetError() };
        }
        ~Audio()
        {
            Mix_CloseAudio();
        }
    };
    static Audio audio{};

    Mix_Chunk* pChunk = Mix_LoadWAV(path.string().c_str());
    if (pChunk == nullptr)
        throw utility::TeiRuntimeError{ "Sound chunk could not be loaded", SDL_GetError() };

    constexpr static auto DELETER = [](Sound* pSound)
    {
        Mix_FreeChunk(static_cast<Mix_Chunk*>(pSound->pData));
        delete pSound;
    };

    std::string sname{
        name.empty()
        ? path.filename().string()
        : name
    };

    return { std::shared_ptr<Sound>{ new Sound{ pChunk, volume, loop, std::move(sname) }, DELETER } };
}