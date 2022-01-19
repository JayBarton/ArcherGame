#pragma once
#include "Sprite.h"
#include "GameObject.h"
#include "Block.h"
#include <glm.hpp>
#include <SDL_mixer.h>

class Arrow
{
public:
    Arrow();

    void setSounds(const std::vector<Mix_Chunk*>& sounds);

    void update(float delta);

    void fire(glm::vec2 start, int level, glm::vec2 velocity, float fallSpeed);

    void checkObjects(std::vector<GameObject*>& enemies, std::vector<GameObject*>& movables);

    bool checkIfHit(GameObject * enemy);

    Sprite sprite;

    std::vector<GameObject*> previouslyHit;

    //Used to make sure arrow collisions with enemies only occur in the direction the arrow is moving
    //Will be a local variable later, currently public so I can visualize it
    glm::vec4 phb;

   // float rotation; //public for now, need a draw function

    ~Arrow();

private:

    //For the tile collisions
    //used to give a slight offset from the top and bottom of the arrow collisions, looks nicer
    //this number should be slightly smaller than the hitbox height
    const static int COLLIDE_OFFSET = 4;

    const static int NORMAL_COLLIDE_SOUND = 0;
    const static int ARMOR_COLLIDE_SOUND = 1;

    glm::vec2 moveSpeed;

    float fallAcceleration = 0.1f;
    float maxFallSpeed = 9.0f;

    float stuckTimer = 0.0f;
    float stuckTime = 1.0f;

    int level;

    bool stuck;

    std::vector<Mix_Chunk*> soundEffects;


    void checkTiles();
    void nearPhase(glm::vec2 point);
    void hitEnemy(GameObject *enemy, int hitCollider);
    void sweep(int& deepestSweep, glm::vec2& push, glm::vec4& hb, glm::vec4& phb,
                GameObject* enemy, Collider& collider, glm::vec2& enemyPositon,
                 glm::vec2& greatestPush, int& hitCollider, int c);

    bool checkEnemy(GameObject *enemy, glm::vec4 hb, glm::vec4 phb);
    bool checkMovable(GameObject *movable, glm::vec4 hb, glm::vec4 phb);
    //Returns the push of the arrow and the object
    glm::vec2 checkObject(GameObject* object, Collider collider, glm::vec2 objectPosition, glm::vec4 hb, glm::vec4 phb);

    bool lineIntersection(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4, glm::vec2& intersection);
};

