#pragma once

#include "Sprite.h"
#include "Physics.h"
#include "WalkBehaviour.h"
#include "ChargeBehaviour.h"

#include "Enemy.h"

#include <glm.hpp>
#include <SDL_mixer.h>

class ChargeEnemy : public GameObject
{
public:

    ChargeEnemy(glm::vec2 p, int facing, bool armor, Sprite &player, Mix_Chunk* sound);

    void update(float delta);

   // WalkBehaviour walk;
    Sprite &player;

    void init(glm::vec2 p, int facing);
    void reset();

    glm::vec4 sight; //This doesn't need to be a variable, just using it so I can visualize it

    bool charging = false; //only public to visualize

    bool armor;

    void takeHit(int damage);

    glm::vec2 startPosition;
    int startFacing;

    ~ChargeEnemy();

private:

    const static int FRAMES_PER_ANIMATION = 4;
    const static int START_FRAME_WALK_RIGHT = 0;
    const static int START_FRAME_WALK_LEFT = 4;
    const static int HURT_FRAME = 8;
    const static int START_FRAME_RUN_RIGHT = 9;
    const static int START_FRAME_RUN_LEFT = 13;

    bool knockedBack = false; //Using this for the hurt frame

    float walkSpeed = 1.5f;
    float chargeSpeed = 3.0f;

    float searchTimer = 0.0f;
    float searchTime = 2.0f;

    Mix_Chunk* noticePlayerSound;

    //Note that this is not a complete function, it's largely being used for testing and will be reworked
    //to have actual parameters later
    //void checkLevelBounds();
    void handleAnimation(float delta);

    bool checkPlayer();
    void lineOfSight();

    void input();
    void checkTurn();

    void turnAround();

};

