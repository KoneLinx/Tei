#pragma once

#include <optional>

#include <tei/resource.h>
#include <tei/unit.h>
#include <tei/application.h>

namespace tei::internal::render
{

	class RendererClass
	{
	public:

		RendererClass(application::Application::Window const&);
		~RendererClass();

		void Clear();
		void Update();
		void Present();

		auto const& GetRenderTraget() const
		{ return m_RenderTarget; }

		void SetVSync(bool synced) const;

		void DrawTexture(resource::Texture const& texture, unit::Transform const& transform, std::optional<unit::Rectangle> source = std::nullopt) const;
		void DrawSprite(resource::Sprite const& texture, unit::Transform const& transform) const;

	private:

		time::Clock::time_point m_Time{};

		struct RenderTarget
		{
			void* pData;
			unit::Scale size;
		}
		m_RenderTarget{};

		//unit::Scale m_TargetSize{};
		unit::Position m_TargetCenter{};
		//unit::Scale m_TargetRatio{};
		//unit::Scale m_TargetScale{};

	public:

		using Service = utility::Service<RendererClass const>;

	};

	extern RendererClass::Service Renderer;

}

namespace tei::external
{

	constexpr static auto const& Renderer{ tei::internal::render::Renderer };

}