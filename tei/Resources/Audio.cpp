#include "teipch.h"
#include "Audio.h"

#include <tei/audio.h>

#include <SDL_mixer.h>

using namespace tei::internal::resource;
using namespace tei::internal;
using namespace std::literals;

void Load(std::shared_ptr<Sound>& out, std::filesystem::path const& path, bool loop, float volume)
{
    METRICS_TIMEBLOCK;

    out = audio::Audio->Load(path);
    out->loop = loop;
    out->volume = volume;
}

void Load(std::shared_ptr<Sound>& out)
{
    Load(out, {});
}