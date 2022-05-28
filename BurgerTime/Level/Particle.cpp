#include "pch.h"
#include "Particle.h"

using namespace tei;
using namespace resource;
using namespace unit;
using namespace components;

tei::ecs::Object& Particle::Create(ecs::Object& parent, Transform start, Transform increment, std::variant<Resource<Texture>, Resource<Sprite>> texture, Clock::duration dur)
{
    auto& rootObject = parent.AddChild();
    auto& visualObject = rootObject.AddChild();

    rootObject.AddComponent<Particle>({
        &rootObject.AddComponent<ObjectTransform>(start),
        std::move(increment),
        std::move(dur)
    });
    
    if (std::holds_alternative<Resource<Texture>>(texture))
        visualObject.AddComponents(
            std::get<Resource<Texture>>(texture),
            TextureRenderComponent{}
        );
    else if (std::holds_alternative<Resource<Sprite>>(texture))
        visualObject.AddComponents(
            std::get<Resource<Sprite>>(texture),
            SpriteRenderComponent{}
        );

    return rootObject;
}


void Particle::OnUpdate(tei::ecs::Object& object)
{
    if (!m_Timer)
    {
        auto delta = Time->frame.delta.count();

        auto inc = m_Increment;
        inc.position *= delta;
        inc.rotation *= delta;
        inc.scale = pow(inc.scale, Scale{ 1 + delta });

        *m_pVisualTransform = **m_pVisualTransform * inc;
    }
    else
    {
        object.SetState(false);
    }
}

void Particle::OnDisable(tei::ecs::Object& object)
{
    object.Clear();
}

Particle::Particle(tei::components::ObjectTransform* pTransform, tei::unit::Transform increment, tei::time::TimeOnce timer)
    : m_pVisualTransform{ pTransform }
    , m_Increment{ std::move(increment) }
    , m_Timer{ std::move(timer) }
{}
