#include "teipch.h"
#include "Audio.h"

#include <tei/audio.h>

#include <SDL_mixer.h>

using namespace tei::internal::resource;
using namespace tei::internal;
using namespace std::literals;

void Load(std::shared_ptr<Sound>& out, std::filesystem::path const& path, bool loop, float volume, [[maybe_unused]] std::string_view name)
{

    //struct Audio
    //{
    //    Audio()
    //    {
    //        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    //            throw utility::TeiRuntimeError{ "Could not open audio", SDL_GetError() };
    //    }
    //    ~Audio()
    //    {
    //        Mix_CloseAudio();
    //    }
    //};
    //static std::weak_ptr<Audio> s_Audio{};

    //auto audio = s_Audio.lock();
    //if (!audio)
    //    s_Audio = audio = std::make_shared<Audio>();


    //auto const DELETER = [] (Sound* pSound)
    //{
    //    audio::Audio->Free(pSound->pData);
    //    delete pSound;
    //};

    //audio::Chunk* pChunk{};
    //if (!path.empty())
    //    pChunk = audio::Audio->Load(path);
        //if (pChunk = Mix_LoadWAV(path.string().c_str()));
        //else
        //    throw utility::TeiRuntimeError{ "Sound chunk could not be loaded", SDL_GetError() };

//#if defined(DEBUG) || defined(_DEBUG)
//    std::string sname{
//        name.empty()
//        ? path.filename().string()
//        : name
//    };
//#endif
//
//    out = { 
//        std::shared_ptr<Sound>{ 
//            new Sound{
//                pChunk, 
//                volume,
//                loop
//#if defined(DEBUG) || defined(_DEBUG)
//                , std::move(sname) 
//#endif
//            },
//            DELETER
//        }
//    };

    out = audio::Audio->Load(path);

    out->loop = loop;
    out->volume = volume;
#if defined(DEBUG) || defined(_DEBUG)
    out->_name = name;
#endif

}

void Load(std::shared_ptr<Sound>& out)
{
    Load(out, {});
}