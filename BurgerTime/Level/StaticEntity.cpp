#include "pch.h"
#include "StaticEntity.h"
#include "Hitbox.h"

using namespace tei;
using namespace resource;
using namespace components;

tei::ecs::Object& StaticEntity::Create(tei::ecs::Object& selfObject, StaticEntityData const& data)
{
    auto& self = selfObject.AddComponent<StaticEntity>();

    self.m_pData = &data;

    selfObject.AddComponents(
        data.box,
        Hitbox{}
    );

    auto& visuals = selfObject.AddChild();
    visuals.AddComponents(
        ObjectTransform{ unit::Scale{ data.box.x, data.box.y } },
        data.texture,
        TextureRenderComponent{}
    );

    return selfObject;
}
