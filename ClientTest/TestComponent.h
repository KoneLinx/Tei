#pragma once

#include <tei.h>

template<>
void On<tei::ecs::Message::INIT>(std::string_view);

template<>
void On<tei::ecs::Message::ENABLE>(std::string_view);

template<>
void On<tei::ecs::Message::UPDATE>(std::string_view);

template<>
void On<tei::ecs::Message::RENDER>(std::string_view);

template<>
void On<tei::ecs::Message::DISABLE>(std::string_view);

template<>
void On<tei::ecs::Message::CLEANUP>(std::string_view);
