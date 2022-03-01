#include "SDLAudio.h"

#include <SDL_mixer.h>

#include <ranges>

namespace tei::internal::audio
{

    void SDLAudio::OnEnable()
    {}

    void SDLAudio::OnDisable()
    {}

    void SDLAudio::OnMute(bool mute)
    {
        if (mute)
            Mix_HaltChannel(-1);
    }

    void SDLAudio::OnUpdate(std::span<std::reference_wrapper<resource::Sound const>> requests)
    {
        for (resource::Sound const& sound : requests)
        {
            auto chunk = static_cast<Mix_Chunk*>(sound.pData);
            Mix_VolumeChunk(chunk, int(sound.volume * MIX_MAX_VOLUME));
            if (int c = Mix_PlayChannel(-1, chunk, sound.loop); c == -1)
                puts("[WARNING] playing too many audio chunks, skipping current call"); //throw utility::TeiRuntimeError{ "Sound could not be played on any channel", SDL_GetError() };
        }
    }

}