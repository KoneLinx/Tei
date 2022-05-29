#pragma once
#include "AudioHandler.h"

namespace tei::internal::audio
{

	class SDLAudioService final : public AudioService
	{
	public:

		SDLAudioService();
		~SDLAudioService();

		virtual void Play(resource::Resource<resource::Sound> const& sound) const override;
		virtual void Mute(bool state) const override;
		virtual bool IsMuted() const override;

		virtual void Update();
		virtual std::shared_ptr<resource::Sound> Load(std::filesystem::path const& path) override;

	private:

		std::unique_ptr<struct Impl> const m_pImpl;

	};

	//class SDLAudio : public AudioHandler
	//{

	//	virtual void OnEnable() override;
	//	virtual void OnDisable() override;
	//	virtual void OnMute(bool) override;
	//	virtual void OnPlay(resource::Sound const&) override;
	//	virtual struct Chunk* Load(std::filesystem::path const&) override;
	//	virtual void Free(struct Chunk*) override;

	//};

}

