#include "Achievements.h"

//tmp
#include "../tei/Steam.h"

tei::ecs::Object& CreateAchievements(tei::ecs::Object& parent)
{
    auto& ach = parent.AddChild();

    ach.AddComponent<Achievenments>();
    ach.AddComponent<tei::components::ObserverComponent>();

    return ach;
}

void SetAchievement(Achievenments::Event ach)
{
    if (ach == Achievenments::FIRST_LEVEL_COMPETED)
        SteamSetAchievement("ACH_WIN_ONE_GAME");
}

void OnEnable(Achievenments const&, tei::ecs::Object& object)
{
    auto& observers = object.GetComponent<tei::components::ObserverComponent>();
    observers.AddObserver(
        tei::Event,
        [](Achievenments::Event event)
        {
            if (event == Achievenments::FIRST_LEVEL_COMPETED)
            {
                puts("First level completed!");
                SetAchievement(Achievenments::FIRST_LEVEL_COMPETED);
            }
        }
    );
}
