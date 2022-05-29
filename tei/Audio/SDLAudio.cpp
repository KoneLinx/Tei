#include "teipch.h"
#include "SDLAudio.h"

#include <SDL_mixer.h>

#include "../Resources/Audio.h"


namespace tei::internal::audio
{

    struct Chunk : Mix_Chunk
    {};

    struct Impl
    {
        using Item = resource::Resource<resource::Sound>::Weak;

        constexpr static size_t QUEUE_SIZE = 8;

        bool Enqueue(Item chunk)
        {
            std::scoped_lock scope{ m_Lock };

            size_t pos = head;
            if (pos + 1 == tail)
                return false; // Queue full

            queue[pos] = std::move(chunk);
            head = (pos + 1) % QUEUE_SIZE;
            return true;
        }

        std::optional<Item> Dequeue()
        {
            std::scoped_lock scope{ m_Lock };

            size_t pos = tail;
            if (pos == head)
                return std::nullopt; // No queued item

            std::optional chunk{ std::move(queue[pos]) };
            tail = (pos + 1) % QUEUE_SIZE;
            return chunk;
        }

        std::array<Item, QUEUE_SIZE> queue{};

        std::mutex m_Lock{};
        size_t head{}, tail{};

        bool muted{ false };
        int volume;
    };
    
    SDLAudioService::SDLAudioService()
        : m_pImpl{ std::make_unique<Impl>() }
    {
        METRICS_TIMEBLOCK;

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
            throw utility::TeiRuntimeError{ "Could not open audio", SDL_GetError() };
    }

    SDLAudioService::~SDLAudioService()
    {
	    METRICS_TIMEBLOCK;

        Mix_CloseAudio();
    }

    void SDLAudioService::Play(resource::Resource<resource::Sound> const& sound) const
    {
	    METRICS_TIMEBLOCK;

        if (m_pImpl->muted)
            return;

        if (m_pImpl->Enqueue(sound) == false)
            std::cerr << "[WARNING] Playing too many audio chunks, skipping current call\n";
    }

    void SDLAudioService::Mute(bool state) const
    {
	    METRICS_TIMEBLOCK;

        if (m_pImpl->muted != state)
        {
            m_pImpl->muted = state;
            if (state)
                m_pImpl->volume = Mix_Volume(-1, 0);
            else
                Mix_Volume(-1, m_pImpl->volume);
        }
    }

    bool SDLAudioService::IsMuted() const
    {
        return m_pImpl->muted;
    }

    void SDLAudioService::Update()
    {
	    METRICS_TIMEBLOCK;

        if (m_pImpl->muted)
            return;

        while (auto item{ m_pImpl->Dequeue() })
        if (auto sound{ item->Lock() })
        {
            Mix_VolumeChunk(sound->pData, int(sound->volume * MIX_MAX_VOLUME));
            if (Mix_PlayChannel(-1, sound->pData, sound->loop) == -1)
                std::cerr << "[WARNING] Playing too many audio chunks, skipping current call\n";
        }
    }

    std::shared_ptr<resource::Sound> SDLAudioService::Load(std::filesystem::path const& path)
    {
	    METRICS_TIMEBLOCK;

        auto chunk = Mix_LoadWAV(path.string().c_str());

        if (chunk == nullptr)
            throw utility::TeiRuntimeError{ "Sound chunk could not be loaded", SDL_GetError() };
        
        auto deleter = [](resource::Sound* sound)
        {
            Mix_FreeChunk(sound->pData);
            delete sound;
        };
        
        return std::shared_ptr<resource::Sound>{
            new resource::Sound{ static_cast<audio::Chunk*>(chunk) },
            deleter
        };
    }


}