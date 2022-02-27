#pragma once

#include <optional>

#include <tei/resource.h>
#include <tei/utility.h>
#include <tei/unit.h>

namespace tei::internal::application
{
	struct Window;
}

namespace tei::internal::render
{

	class RendererClass
	{
	public:

		RendererClass(application::Window const&);
		~RendererClass();

		void Clear();
		void Update();
		void Present();

		auto const& RenderTraget()
		{ return m_RenderTarget; }

		void DrawTexture(resource::Texture const& texture, unit::Transform const& transform, std::optional<unit::Rectangle> source = std::nullopt) const;
		void DrawSprite(resource::Sprite const& texture, unit::Transform const& transform) const;

	private:

		time::Clock::time_point m_Time{};

		struct RenderTarget
		{
			void* pData;
			int w, h;
		}
		m_RenderTarget{};

		unit::Scale m_TargetSize{};
		unit::Position m_TargetCenter{};
		unit::Scale m_TargetRatio{};
		unit::Scale m_TargetScale{};

	public:

		using Service = utility::Service<RendererClass const>;

	};

	extern RendererClass::Service Renderer;

}

namespace tei::external
{

	constexpr static auto const& Renderer{ tei::internal::render::Renderer };

}