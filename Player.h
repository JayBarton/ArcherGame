#pragma once

#include <glm.hpp>
#include <SDL.h>

#include "Sprite.h"
#include "Physics.h"
#include "InputManager.h"
#include "GameObject.h"
#include "SATCollision.h"
#include "Arrow.h"
#include "Collider.h"
#include "Block.h"

#include <SDL_mixer.h>

class Pickup;
class SpriteRenderer;
class Player
{
public:

    const static int IDLE_RIGHT = 8;
    const static int TAKE_HIT_SOUND = 4;

    Player();
    ~Player();

    Sprite sprite; //may replace this with a transform/position type of thing
    Health health;
    Physics *physics = nullptr;

    void init(InputManager *inputManager, std::vector<Arrow> *arrows, std::vector<glm::vec4> &uvs, std::vector<glm::vec4>& bowUVs, const std::vector<Mix_Chunk*>& sounds);
    void setSprite(glm::vec2 p);

    void update(float delta);
    void lateUpdate();

    void animate(float delta);

    void drawBody(SpriteRenderer* Renderer);
    void drawBow(SpriteRenderer* Renderer);

    //Just using aiming to help visualize where things will go for now
    Sprite aimer;
    //The idea with aimer2 is that I will be using a composite sprite for at least aiming
    //That way I can just rotate that part of the sprite as the player is aiming
    Sprite aimer2;

    //These are both temporarily public until I make an actual draw function
    float rotationDirection = 0.0f;

    bool dash = false;

    bool charging = false;

    bool gameOver = false;

    int currentLives = STARTING_LIVES;
    int currentStars = 0;
    int collectedGems = 0;

    GameObject *parentObject = nullptr;
    glm::vec2 parentVelocity;

    void checkEnemies(std::vector<GameObject*> &enemies);
    void checkBlocks(std::vector<GameObject*>& enemies);
    bool checkBlock(glm::vec4 hb, glm::vec2 objectPosition, GameObject *block, glm::vec2 &push);
    void checkProjectiles(std::vector<Projectile> &projectiles);

    void checkPickups(std::vector<Pickup*> &pickups);
    void addHealth(int health);
    void addLives(int lives);
    void addStars(int stars);

    //Resets player's state to default
    void setUp();
    //Resets player health/lives/stars
    void setStatus();

    void die();

    bool deathAnimation(float delta);

    //Costs 1 life to return to hub, can't return if none remaining
    bool canReturnToHub();

    //Just for testing, will change player color to see if this works
    int Charge()
    {
        return chargeLevel;
    }


private:

    const static int UP_ROTATION = -90;
    const static int DOWN_ROTATION = 45;
    const static int STRAIGHT_DOWN = 90;

    const static int MAX_CHARGE = 2;

    const static int MAX_HEALTH = 3;
    const static int STARTING_LIVES = 3;
    const static int MAX_LIVES = 99;
    const static int MAX_STARS = 100;

    const static int JUMP_SOUND = 0;
    const static int CHARGE_LEVEL_1_SOUND = 1;
    const static int CHARGE_LEVEL_2_SOUND = 2;
    const static int FIRE_ARROW_SOUND = 3;
    const static int EXTRA_LIFE = 5;

    const static int WALK_RIGHT = 0;
    const static int WALK_LEFT = 4;
    const static int IDLE_LEFT = 9;
    const static int CHARGE_WALK_RIGHT = 10;
    const static int CHARGE_WALK_LEFT = 14;
    const static int IDLE_CHARGE_RIGHT = 18;
    const static int IDLE_CHARGE_LEFT = 19;
    const static int DUCK_WALK_RIGHT = 20;
    const static int DUCK_WALK_CHARGE_RIGHT = 30;
    const static int DUCK_RIGHT_IDLE = 28;
    const static int DUCK_CHARGE_RIGHT_IDLE = 38;

    const static int RUN_RIGHT_START = 40;
    const static int RUN_LEFT_START = 44;
    const static int HIT_FRAME_RIGHT = 48;
    const static int HIT_FRAME_LEFT = 49;
    const static int JUMP_RIGHT_FRAME = 50;
    const static int JUMP_LEFT_FRAME = 51;
    const static int FALL_RIGHT_FRAME = 52;
    const static int FALL_LEFT_FRAME = 53;
    const static int JUMP_RIGHT_AIM_FRAME = 54;
    const static int JUMP_LEFT_AIM_FRAME = 55;
    const static int FALL_RIGHT_AIM_FRAME = 56;
    const static int FALL_LEFT_AIM_FRAME = 57;
    const static int DEATH_FRAME = 58;


    bool recentFire = false;

    bool ducking = false;

    bool moveButton = false; //true if left or right is down

    bool canFire = true;

    float jumpSpeed = -12.0f; // 4*
    float jumpCutoffSpeed = -4.0f;

    float dashSpeed = 9.0f;
    float walkSpeed = 5.0f;
    float chargeWalkSpeed = 2.5f;
    float duckWalkSpeed = 2.5f;

    float fireTimer = 0.0f;
    float fireTime = 0.3f;

    // float fireTime = 1.0f;

    float resetAimTimer = 0.0f;
    float resetAimTime = 0.4f;

    float chargeTimer = 0.0f;
    float chargeTime = 0.5f;

    float deathTimer = 0.0f;
    float deathTime =  1.0f;
    float fallSpeed = -9;

    int chargeLevel;

    glm::vec2 aimDirection;

    //The player position at the start of a frame
    glm::vec2 startPosition;

    InputManager *inputManager = nullptr;

    std::vector<Arrow> *arrows = nullptr;

    std::vector<Mix_Chunk*> soundEffects;

    void checkInput();
    void checkParent();
    void groundMovement(); //7
    void checkCharge(float delta);
    void chargeShot();
    //Not sure if this function is sticking around in it's current state
    void jump();
    void fire();
    void duck();
    void resetAim();
    void takeHit(int damage);

    void stand();
};

