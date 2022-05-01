#include "teipch.h"
#include "SDLAudio.h"

#include <SDL_mixer.h>

#include "../Resources/Audio.h"


namespace tei::internal::audio
{

    struct Chunk : Mix_Chunk
    {};

    void SDLAudio::OnEnable()
    {
        // Mix_Init() ?
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
            throw utility::TeiRuntimeError{ "Could not open audio", SDL_GetError() };
    }

    void SDLAudio::OnDisable()
    {
        // Mix_Quit();
        Mix_CloseAudio();
    }

    void SDLAudio::OnMute(bool mute)
    {
        if (mute)
            Mix_HaltChannel(-1);
    }

    void SDLAudio::OnPlay(resource::Sound const& sound)
    {
        auto chunk = static_cast<Mix_Chunk*>(sound.pData);
        Mix_VolumeChunk(chunk, int(sound.volume * MIX_MAX_VOLUME));
        if (Mix_PlayChannel(-1, chunk, sound.loop) == -1)
            puts("[WARNING] playing too many audio chunks, skipping current call"); //throw utility::TeiRuntimeError{ "Sound could not be played on any channel", SDL_GetError() };
    }

    Chunk* SDLAudio::Load(std::filesystem::path const& path)
    {
        if (Chunk* chunk = static_cast<Chunk*>(Mix_LoadWAV(path.string().c_str())))
            return chunk;
        else
            throw utility::TeiRuntimeError{ "Sound chunk could not be loaded", SDL_GetError() };
    }

    void SDLAudio::Free(Chunk* chunk)
    {
        Mix_FreeChunk(chunk);
    }

}