#include "Level.h"

#include "Player.h"
#include "Achievements.h"

tei::ecs::Object& CreateLevel(tei::ecs::Object& parent)
{
    auto& levelObject = parent.AddChild();

    levelObject.AddComponent<Level>();
    auto& sub = levelObject.AddComponent<tei::components::Subject>();

    CreatePlayerTracker(levelObject);

    levelObject.AddChild().AddComponent<Player>(0);
    auto& player = levelObject.AddChild().AddComponent<Player>(1);

    //CreateScore(levelObject);

    return levelObject;
}

void OnEnable(Level& level, tei::ecs::Object const& object)
{
    auto& subject = object.GetComponent<tei::components::Subject>();
    subject.AddObserver(
        [&](Level::Event event)
        {
            if (event == Level::Event::COMPLETED && !std::exchange(level.completed, true))
            {
                puts("Level completed!");
                tei::Event->Notify(Achievenments::FIRST_LEVEL_COMPETED);
            }
            else
            if (event == Level::Event::FAILED)
            {
                puts("Level failed!");
            }
        }
    );

}
