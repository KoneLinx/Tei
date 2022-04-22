#include "Level.h"

#include "Player.h"
#include "Achievements.h"

#include "Json.h"

//tei::ecs::Object& CreateLevel(tei::ecs::Object& parent)
//{
//    auto& levelObject = parent.AddChild();
//
//    levelObject.AddComponent<Level>();
//    auto& sub = levelObject.AddComponent<tei::components::Subject>();
//
//    CreatePlayerTracker(levelObject);
//
//    levelObject.AddChild().AddComponent<Player>(0);
//    auto& player = levelObject.AddChild().AddComponent<Player>(1);
//
//    //CreateScore(levelObject);
//
//    return levelObject;
//}

void Level::OnEnable(tei::ecs::Object const& object)
{
    auto& subject = object.GetComponent<tei::components::Subject>();
    subject.AddObserver(
        [&](Level::Event event)
        {
            if (event == Level::Event::COMPLETED && !std::exchange(completed, true))
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

tei::ecs::Object& Level::Create(tei::ecs::Object& parent, nlohmann::json const& object, bool versus)
{
    auto& levelObject = parent.AddChild();

    auto& level = levelObject.AddComponent(Level{});
    auto& data = level.m_Leveldata;
    data.versus = versus;
    LoadToOptional(data.stage      , object, "stage");
    LoadToOptional(data.hieght     , object, "floors");
    LoadToOptional(data.width      , object, "burgers");
    LoadToOptional(data.enemies    , object, "enemies");
    LoadToOptional(data.ingredients, object, "ingredients");
    LoadToOptional(data.difficulty , object, "difficulty");
    LoadToOptional(data.ladders    , object, "ladders");
    LoadToOptional(data.seed       , object, "seed");

    levelObject.AddComponent<tei::components::Subject>();

    return levelObject;
}

void Level::OnEnable(tei::ecs::Object const& object)
{

    auto& playerObject = Player::Create(object);

}

Level::Level()
{}
