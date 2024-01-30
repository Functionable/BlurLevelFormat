#include "test.hpp"

#include "blf/localobjecttable.hpp"

#include <cstdint>
#include <cassert>

struct Vector
{
    float X;
    float Y;
    float Z;

    bool operator==(const Vector& other) const
    {
        return X == other.X && Y == other.Y && Z == other.Z;
    }
};

struct GravityStatus
{
    bool enabled = true;
    Vector direction = {0, -1, 0};
    float magnitude = 9.81f;

    bool operator==(const GravityStatus& other) const
    {
        return enabled == other.enabled && direction == other.direction && magnitude == other.magnitude;
    }
};

struct ScoreStatus
{
    uint16_t team1Score = 46;
    std::string team1Name = "Foo Team";
    uint16_t team2Score = 15;
    std::string team2Name = "Bar Team";

    bool operator==(const ScoreStatus& other) const
    {
        return team1Score == other.team1Score && team2Score == other.team2Score &&
               team1Name == other.team1Name && team2Name == other.team2Name;
    }
};

struct UIState
{
    Vector location = {800, 600};
    Vector size = {200, 200};
    bool shown = false;

    bool operator==(const UIState& other) const
    {
        return location == other.location && size == other.size && shown == other.shown;
    }
};

struct GameUIStatus
{
    UIState ui1State = { {200, 200}, {500, 500}, true };
    UIState ui2State = { {290, 900}, {900, 590}, true };
    UIState ui3State = { {800, 800}, {500, 500}, false };

    bool operator==(const GameUIStatus& other) const
    {
        return ui1State == other.ui1State && ui2State == other.ui2State && ui3State == other.ui3State;
    }
};

struct GameState
{
    GravityStatus gravStatus;
    ScoreStatus scoreStatus;
    GameUIStatus gameUIStatus;

    bool operator==(const GameState& other) const
    {
        return gravStatus == other.gravStatus && scoreStatus == other.scoreStatus && gameUIStatus == other.gameUIStatus;
    }

    bool operator!=(const GameState& other) const
    {
        return !(*this == other);
    }
};

GameState makeblank()
{
    return {
        {false, {0,0,0}, 0},
        {0, "", 0, ""},
        {
            { 0,  0, false},
            { 0,  0, false},
            { 0,  0, false}
        }
    };
}

bool test()
{
    bool success = true;
    char databuf[512];

    blf::LocalObjectTable table;
    auto& vecDef = table.define<Vector>("Vector",
        blf::arg("x", &Vector::X),
        blf::arg("y", &Vector::Y),
        blf::arg("z", &Vector::Z)
    );

    auto& gravDef = table.define<GravityStatus>("GravityStatus",
        blf::arg("enabled", &GravityStatus::enabled),
        blf::arg("direction", &GravityStatus::direction, vecDef),
        blf::arg("magnitude", &GravityStatus::magnitude)
    );

    auto& scoreDef = table.define<ScoreStatus>("ScoreStatus",
        blf::arg("score1", &ScoreStatus::team1Score),
        blf::arg("name1", &ScoreStatus::team1Name),
        blf::arg("score2", &ScoreStatus::team2Score),
        blf::arg("name2", &ScoreStatus::team2Name)
    );

    auto& uiDef = table.define<UIState>("UIState",
        blf::arg("location", &UIState::location, vecDef),
        blf::arg("size", &UIState::size, vecDef),
        blf::arg("shown", &UIState::shown)  
    );

    auto& gameUIDef = table.define<GameUIStatus>("GameUIStatus",
        blf::arg("ui1State", &GameUIStatus::ui1State, uiDef),
        blf::arg("ui2State", &GameUIStatus::ui2State, uiDef),
        blf::arg("ui3State", &GameUIStatus::ui3State, uiDef)
    );

    auto& definition = table.define<GameState>("GameState",
          blf::arg("GravStatus", &GameState::gravStatus, gravDef),
          blf::arg("ScoreStatus", &GameState::scoreStatus, scoreDef),
          blf::arg("GameUIDef", &GameState::gameUIStatus, gameUIDef)
    );

    GameState aState;
    GameState blankState = makeblank();
    blf::SerializationContext ctx = {};
    assert(aState != blankState);

    {
        PROFILE_SCOPE("Compound definition serialization");
        definition.serialize(ctx, &aState, databuf);
    }

    {
        PROFILE_SCOPE("Compound definition deserialization");
        definition.deserialize(ctx, &blankState, databuf);
    }

    if( aState != blankState )
    {
        success = false;
        std::cout << "Compound object mismatch." << std::endl;
    }

    return success;
}