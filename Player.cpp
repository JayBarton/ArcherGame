#include "Player.h"
#include<gtx/rotate_vector.hpp>
#include "Pickup.h"
#include "TileManager.h"
#include "Walker.h"
#include "SpriteRenderer.h"

#include "ResourceManager.h"

Player::Player()
{
}

void Player::init(InputManager *inputManager, std::vector<Arrow> *arrows, std::vector<glm::vec4> &bodyUVs, std::vector<glm::vec4>& bowUVs, const std::vector<Mix_Chunk*>& sounds)
{
    stand();

    sprite.setSize(glm::vec2(32, 32));

    this->inputManager = inputManager;
    this->arrows = arrows;

    physics = new Physics();

    physics->state = Physics::ON_GROUND;

    health = Health(MAX_HEALTH);

    sprite.uv = &bodyUVs;

    aimer2.uv = &bowUVs;

    soundEffects = sounds;
    sprite.startingFrame = IDLE_RIGHT;
}

void Player::setSprite(glm::vec2 p)
{
    sprite.active = true;
    sprite.SetPosition(p);
    sprite.facing = 1;

    aimer.active = true;
    aimer.SetPosition(p);

    aimer2.active = true;
    aimer2.SetPosition(sprite.getCenter() - 8.0f);
    aimer2.setParent(&sprite);

    aimDirection = glm::vec2(sprite.facing, 0.0f);
    sprite.currentFrame = IDLE_RIGHT;
}

void Player::setUp()
{
    stand();
    resetAimTimer = 0.0f;
    chargeTimer = 0.0f;
    fireTimer = 0.0f;
    physics->moveSpeed = glm::vec2(5.0f, 0.0f);

    physics->currentSpeed = 0;

    physics->state = Physics::ON_GROUND;

    moveButton = false;

    chargeLevel = 0;

    sprite.alpha = 1.0f;
    rotationDirection = 0.0f;

    dash = false;

    recentFire = false;
    charging = false;
    canFire = true;

    ducking = false;

    gameOver = false;
    sprite.setParent(nullptr);
    parentObject = nullptr;

    aimDirection = glm::vec2(0);
}

void Player::setStatus()
{
    health = Health(MAX_HEALTH);
    currentStars = 0;
    currentLives = STARTING_LIVES;
}


void Player::update(float delta)
{
    startPosition = sprite.getPosition();

    if (!health.hit)
    {
        checkInput();
    }

    health.updateDamaged(delta);

    //Don't really care for this but it will do for now
    if(!health.damaged)
    {
        sprite.alpha = 1.0f;
    }

    checkParent();

    physics->update(sprite, delta);

    //I want the player to stop dashing if they hit a wall or edge
    //However, if they are jumping over something, they should maintain the dash if they go over the ledge.
    if(physics->state == physics->ON_GROUND && dash && physics->leftRightSensor)
    {
        dash = false;
    }

    checkCharge(delta);

    if(!canFire)
    {
        fireTimer += delta;
        if (fireTimer >= fireTime)
        {
            canFire = true;
            fireTimer = 0.0f;
        }
    }

    animate(delta);

    //As this is largely just visual, I have it here for that
    //Aim direction will actually be calculated when the player releases the fire button
    aimDirection = glm::vec2(sprite.facing, 0.0f);
    aimer.SetPosition(sprite.getCenter() - 8.0f + glm::rotate(aimDirection, glm::radians(rotationDirection * sprite.facing)) * 16.0f);
}

void Player::checkParent()
{
    if (sprite.parent != nullptr)
    {
        glm::vec4 hb = sprite.getHitBox();
        glm::vec4 ehb = sprite.parent->getHitBox();
        //  if (!sprite.parent->active || !sprite.collide(sprite.getHitBox(), sprite.parent->getHitBox()))
        //   if(!enemyContact || sprite.active == false)
        if(!sprite.parent->active || (!(hb.x + 5 > ehb.x &&
                                        hb.x + 5 < ehb.x + ehb.z) &&
                                      !(hb.x + (hb.z - 5) > ehb.x &&
                                        hb.x + (hb.z - 5) < ehb.x + ehb.z)))
        {
            sprite.setParent(nullptr);
        }
    }
}


void Player::lateUpdate()
{
    if (physics->state == Physics::IN_AIR)
    {
        //Not sure where to put this, player shouldn't be able to duck while in the air
        //definitely don't want to do it every frame though
        ducking = false;
        stand();
    }
    //verify that the player did not get a new parent after checkParent
    //If they didn't, then they are back in the air
    //If the player is moving in the same direction as their former parent, add that parent's x velocity to the player's velocity
    //This is to maintain the inertia of the former parent
    //(also prevents an issue that makes walking off of the former parent difficult!)
    if(sprite.parent == nullptr && parentObject != nullptr)
    {
        if (Block *block = dynamic_cast<Block*>(parentObject))
        {
            if(block->flyer.direction.x * physics->moveSpeed.x > 0)
            {
                physics->currentSpeed += block->flyer.speed;
            }
        }
        else
        {
            Walker *walker = static_cast<Walker*>(parentObject);
            if(walker->physics->moveSpeed.x * physics->moveSpeed.x > 0)
            {
                physics->currentSpeed += walker->walk.walkSpeed;
            }
        }
        parentObject = nullptr;
    }
}


//TODO fix checking parent, shouldn't be possible to collide with them
void Player::checkEnemies(std::vector<GameObject*>& enemies)
{
    glm::vec4 hb = sprite.getHitBox();

    for (int i = 0; i < enemies.size(); i++)
    {
        //nearest collision. not crazy about this idea,
        //as it kind of means you need to check every non walkable collider rather than the first you hit.
        //Also currently written to just take the last collider you hit
        //keep working on it.
        Collider *nc = nullptr;
        GameObject *enemy = enemies[i];
        if (sprite.parent != &enemy->sprite)
        {
            glm::vec2 enemyPositon = enemy->sprite.getPosition();
            for (int c = 0; c < enemy->colliders.size(); c++)
            {
                Collider collider = enemy->colliders[c];
                glm::vec4 ehb = collider.dimensions + glm::vec4(enemyPositon.x, enemyPositon.y, 0, 0);

                if(collider.type == Collider::DAMAGE)
                {
                    if (sprite.collide(hb, ehb))
                    {
                        nc = &collider;
                    }
                }
                else if(enemy->health.alive())
                {
                    if(collider.type == Collider::WALKABLE)
                    {
                        //not sure what I'm doing with the - 16
                        if (physics->moveSpeed.y > 0 && hb.y + hb.w - 16 <= ehb.y && sprite.collide(hb, ehb))
                        {
                            SimpleShape pBox;
                            pBox = newSquare(sprite.getHitBoxCenter(), hb.z, hb.w);

                            SimpleShape tBox;
                            tBox = newSquare(enemy->sprite.getHitBoxCenter(), 32, 32); //figure this out

                            glm::vec2 push(0);
                            satCollision(pBox, tBox, push.x, push.y);
                            if (push.y < 0)
                            {
                                physics->moveSpeed.y = 0.0f;

                                sprite.setParent(&enemy->sprite);
                                parentObject = enemy;

                                sprite.Move(push);

                                physics->state = Physics::ON_GROUND;
                                nc = nullptr;
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (sprite.collide(hb, ehb))
                        {
                            nc = &collider;
                        }
                    }
                }
            }
            //need to check for not colliding with player's parent, and hasn't been recently damaged
            if (!health.damaged && nc != nullptr)
            {
                takeHit(1);
            }
        }
    }
}

void Player::checkBlocks(std::vector<GameObject*>& objects)
{
    glm::vec4 hb = sprite.getHitBox();

    //Check horizontal collisions
    for (int i = 0; i < objects.size(); i++)
    {
        GameObject *object = objects[i];

        glm::vec2 push;
        if(checkBlock(hb, object->sprite.getPosition(), object, push))
        {
            if(push.x != 0)
            {
                physics->moveSpeed.x = 0;
                sprite.Move(push);
                hb = sprite.getHitBox();
            }
        }
    }

    //Check vertical collisions
    //Vertical collisions are thinner than horizontal
    glm::vec4 hb2 = hb;
    hb2.x += 2;
    hb2.z -= 4;
    for (int i = 0; i < objects.size(); i++)
    {
        GameObject *object = objects[i];
        glm::vec2 objectPosition = object->sprite.getPosition();
        glm::vec2 push;
        if(checkBlock(hb2, objectPosition, object, push))
        {
            Block *block = static_cast<Block*>(object);

            //Need a Secondary x check for some odd edge cases
            //Keep a close eye on this, it might cause issues elsewhere.
            if(push.x != 0)
            {
                physics->moveSpeed.x = 0;
                sprite.Move(push);
                hb = sprite.getHitBox();
            }
            else
            {
                if(push.y > 0)
                {
                    if(physics->moveSpeed.y < 0)
                    {
                        physics->moveSpeed.y = 0.0f;
                    }
                    //Crush the player
                    else if(block->flyer.direction.y > 0 && physics->moveSpeed.y == 0 && physics->state == Physics::ON_GROUND)
                    {
                        push.y = 0;
                        sprite.currentFrame = DEATH_FRAME;
                        sprite.active = false;
                    }
                    else
                    {
                        //   push.y = 0;
                    }
                }
                else if (push.y <= 0)
                {
                    if (hb.y + (hb.w * 0.5f) <= objectPosition.y)
                    {
                        //needs to be tested.
                        //Try without the first condidtion?
                        if(parentObject != block && (!physics->bottomLeftSensor && !physics->bottomRightSensor))
                        {
                            physics->moveSpeed.y = 0.0f;

                            sprite.setParent(&object->sprite);
                            parentObject = block;

                            physics->state = Physics::ON_GROUND;
                        }
                    }
                    else
                    {
                        push.y = 0;
                    }
                }
                sprite.Move(push);
                hb = sprite.getHitBox();
            }
        }
    }
}

bool Player::checkBlock(glm::vec4 hb, glm::vec2 objectPosition, GameObject* object, glm::vec2 &push)
{
    push = glm::vec2(0);

    if (sprite.parent != &object->sprite)
    {
        if(sprite.collide(hb, object->sprite.getHitBox()))
        {

            //Only ever call this function on blocks, that only ever has the one collider
            Collider collider = object->colliders[0];
            glm::vec4 ehb = collider.dimensions + glm::vec4(objectPosition.x, objectPosition.y, 0, 0);
            glm::vec4 ehb2 = object->sprite.getHitBox();
            SimpleShape pBox;
            pBox = newSquare(sprite.getHitBoxCenter(), hb.z, hb.w);

            SimpleShape tBox;
            tBox = newSquare(object->sprite.getCenter(), object->sprite.getSize().x, object->sprite.getSize().y); //figure this out
            return satCollision(pBox, tBox, push.x, push.y);
        }
    }
    return false;
}



void Player::checkProjectiles(std::vector<Projectile>& projectiles)
{
    if(!health.damaged)
    {
        for(int i = 0; i < projectiles.size(); i ++)
        {
            if(projectiles[i].sprite.active && sprite.collide(sprite.getHitBox(), projectiles[i].sprite.getHitBox()))
            {
                takeHit(1);
                break;
            }
        }
    }
}


void Player::checkPickups(std::vector<Pickup*>& pickups)
{
    glm::vec4 hb = sprite.getHitBox();

    for (int i = 0; i < pickups.size(); i++)
    {
        Pickup *pickup = pickups[i];
        if (sprite.collide(hb, pickup->sprite.getHitBox()))
        {
            pickup->apply(*this);
        }
    }
}

void Player::addHealth(int health)
{
    this->health.currentHealth += health;
    if ( this->health.currentHealth > MAX_HEALTH)
    {
        this->health.currentHealth = MAX_HEALTH;
    }
}

void Player::addLives(int lives)
{
    currentLives += lives;
    Mix_PlayChannel(-1, soundEffects[EXTRA_LIFE], 0);
    if (currentLives > MAX_LIVES)
    {
        currentLives = MAX_LIVES;
    }
}

void Player::addStars(int stars)
{
    currentStars += stars;
    if (currentStars >= MAX_STARS)
    {
        currentStars -= MAX_STARS;
        addLives(1);
    }
}

void Player::animate(float delta)
{
    //using this to handle left facing frames
    int leftAnimate = 0;
    int leftFrame = 0;
    int startingFrame = 0;

    if(sprite.facing > 0)
    {
        if(charging && canFire)
        {
            aimer2.currentFrame = 0;
        }
        else if(recentFire || (charging && !canFire))
        {
            aimer2.currentFrame = 1;
        }
    }
    else
    {
        if(charging && canFire)
        {
            aimer2.currentFrame = 2;
        }
        else if(recentFire || (charging && !canFire))
        {
            aimer2.currentFrame = 3;
        }
        leftAnimate = 4;
        leftFrame = 1;
    }
    if(health.hit)
    {
        sprite.currentFrame = HIT_FRAME_RIGHT + leftFrame;
    }
    else if(physics->state == physics->State::IN_AIR)
    {
        if(physics->moveSpeed.y < 0)
        {
            if(charging || recentFire)
            {
                sprite.currentFrame = JUMP_RIGHT_AIM_FRAME + leftFrame;
            }
            else
            {
                sprite.currentFrame = JUMP_RIGHT_FRAME + leftFrame;
            }
        }
        //checking like this to prevent the player from entering the falling state while walking over small gaps
        else if(physics->moveSpeed.y > 0.5f)
        {
            if(charging || recentFire)
            {
                sprite.currentFrame = FALL_RIGHT_AIM_FRAME + leftFrame;
            }
            else
            {
                sprite.currentFrame = FALL_RIGHT_FRAME + leftFrame;
            }
        }
    }
    else if(!ducking)
    {
        if(physics->moveSpeed.x != 0)
        {
            if(dash)
            {
                startingFrame = RUN_RIGHT_START + leftAnimate;
                sprite.startingFrame = startingFrame;
                if(sprite.currentFrame < sprite.startingFrame || sprite.currentFrame >= sprite.startingFrame + 4)
                {
                    sprite.currentFrame = startingFrame;
                }
                sprite.playAnimation(delta, 4, false);
            }
            else
            {
                if(charging || recentFire)
                {
                    startingFrame = CHARGE_WALK_RIGHT + leftAnimate;
                    sprite.startingFrame = startingFrame;
                    if(sprite.currentFrame < sprite.startingFrame || sprite.currentFrame >= sprite.startingFrame + 4)
                    {
                        sprite.currentFrame = startingFrame;
                    }
                }
                else
                {
                    startingFrame = WALK_RIGHT + leftAnimate;
                    sprite.startingFrame = startingFrame;
                    if(sprite.currentFrame < sprite.startingFrame || sprite.currentFrame >= sprite.startingFrame + 4)
                    {
                        sprite.currentFrame = startingFrame;
                    }
                }
                sprite.playAnimation(delta, 4, true);
            }

        }
        else
        {
            //just for testing right now. The idea is that there are going to be a set of frames used for aiming and regular ones for walking
            if(charging || recentFire)
            {
                sprite.currentFrame = IDLE_CHARGE_RIGHT + leftFrame;
            }
            else
            {
                sprite.currentFrame = IDLE_RIGHT + leftFrame;
            }
        }
    }
    else
    {
        if(physics->moveSpeed.x != 0)
        {
            if(charging || recentFire)
            {
                startingFrame = DUCK_WALK_CHARGE_RIGHT + leftAnimate;
                sprite.startingFrame = startingFrame;
                if(sprite.currentFrame < sprite.startingFrame || sprite.currentFrame >= sprite.startingFrame + 4)
                {
                    sprite.currentFrame = startingFrame;
                }
            }
            else
            {
                startingFrame = DUCK_WALK_RIGHT + leftAnimate;
                sprite.startingFrame = startingFrame;
                if(sprite.currentFrame < sprite.startingFrame || sprite.currentFrame >= sprite.startingFrame + 4)
                {
                    sprite.currentFrame = startingFrame;
                }
            }
            sprite.playAnimation(delta, 4, true);

        }
        else
        {
            //just for testing right now. The idea is that there are going to be a set of frames used for aiming and regular ones for walking
            if(charging || recentFire)
            {
                sprite.currentFrame = DUCK_CHARGE_RIGHT_IDLE + leftFrame;
            }
            else
            {
                sprite.currentFrame = DUCK_RIGHT_IDLE + leftFrame;
            }
        }
    }
}


void Player::drawBody(SpriteRenderer* Renderer)
{
    ResourceManager::GetShader("sprite").Use();
    glm::vec3 color = sprite.color;
    glm::vec4 colorAndAlpha = glm::vec4(color.x, color.y, color.z, sprite.alpha);

    glm::vec2 position = sprite.getPosition();
    Renderer->setUVs(sprite.getUV());
    Texture2D texture = ResourceManager::GetTexture("sprites");

    Renderer->DrawSprite(texture, position, 0.0f, sprite.getSize(), colorAndAlpha);

}

void Player::drawBow(SpriteRenderer* Renderer)
{
    if(charging || recentFire)
    {
        glm::vec2 position = sprite.getPosition();
        Renderer->setUVs(aimer2.getUV());
        Texture2D texture = ResourceManager::GetTexture("sprites");
        float rotation = glm::radians(rotationDirection * sprite.facing);
        if(ducking)
        {
            position.y += 7;
        }
        Renderer->DrawSprite(texture, position, rotation, glm::vec2(32.0f, 32.0f));
    }
}

Player::~Player()
{
    delete physics;
}

void Player::checkInput()
{
    moveButton = false;
    switch (physics->state)
    {
    case Physics::ON_GROUND:

        groundMovement();

        if (!ducking)
        {
            if (inputManager->isKeyPressed(SDLK_q))
            {
                if (inputManager->isKeyDown(SDLK_UP))
                {
                    rotationDirection = UP_ROTATION;
                }
            }

            if (inputManager->isKeyDown(SDLK_q))
            {
                chargeShot();

                physics->currentSpeed = chargeWalkSpeed;
                dash = false;
                if (inputManager->isKeyDown(SDLK_UP))
                {
                    rotationDirection -= 5.0f;
                    if (rotationDirection < UP_ROTATION)
                    {
                        rotationDirection = UP_ROTATION;
                    }
                }
                else if (inputManager->isKeyDown(SDLK_DOWN))
                {
                    rotationDirection += 5.0f;
                    if (rotationDirection > DOWN_ROTATION)
                    {
                        rotationDirection = DOWN_ROTATION;
                    }
                }
            }

            //TODO shouldn't check this at all if charging
            //Will always be false if charging, due to how keyDoublePress works, but still
            if (inputManager->keyDoublePress(SDLK_RIGHT) || inputManager->keyDoublePress(SDLK_LEFT))
            {
                dash = true;
                physics->currentSpeed = walkSpeed;
            }

            if (dash)
            {
                physics->currentSpeed += 1.0f;
                //accelerate until max speed
                if (physics->currentSpeed >= dashSpeed)
                {
                    physics->currentSpeed = dashSpeed;
                }
            }
            else
            {
                //No acceleration when charging
                if (!charging) //hopefully come up with a better solution than this
                {
                    if(moveButton)
                    {
                        //accelerate until max speed
                        physics->currentSpeed += 1.0f;
                        if(physics->currentSpeed >= walkSpeed)
                        {
                            physics->currentSpeed = walkSpeed;
                        }
                    }
                }
            }

            //Maybe make this a bit cleaner
            if(!moveButton)
            {
                physics->currentSpeed = 0.0f;
                physics->moveSpeed.x = 0;
                dash = false;
            }

            //not fond of this here
            if (rotationDirection > DOWN_ROTATION)
            {
                if (charging)
                {
                    rotationDirection = DOWN_ROTATION;
                }
                else
                {
                    resetAim();
                }
            }

            //fire
            if (inputManager->isKeyReleased(SDLK_q))
            {
                physics->currentSpeed = walkSpeed;

                fire();

            }

            if (inputManager->isKeyPressed(SDLK_w))
            {
                jump();
            }

            else if (!charging && inputManager->isKeyDown(SDLK_DOWN))
            {
                duck();
            }
        }
        else
        {
            if (!moveButton)
            {
                physics->moveSpeed.x = 0;
            }

            if (inputManager->isKeyDown(SDLK_q))
            {
                //charge shot
                chargeShot();
            }

            if (inputManager->isKeyReleased(SDLK_q))
            {
                fire();
            }


            glm::vec4 hb = sprite.getHitBox();
            //the 6 comes from the physics stuff, not sure how I'll handle that going forward
            bool under = !TileManager::tileManager.tileIsPassable(hb.x + 6, hb.y) ||
                         !TileManager::tileManager.tileIsPassable(hb.x + hb.z - 6, hb.y);
            if(!under)
            {
                if (inputManager->isKeyUp(SDLK_DOWN))
                {
                    ducking = false;
                    stand();
                    physics->currentSpeed = walkSpeed;
                }
                else if (inputManager->isKeyDown(SDLK_w))
                {
                    jump();
                    ducking = false;
                    stand();
                }
            }
        }
        break;

    case Physics::IN_AIR:
        if (inputManager->isKeyPressed(SDLK_q))
        {
            if (inputManager->isKeyDown(SDLK_UP))
            {
                rotationDirection = UP_ROTATION;
            }
            else if (inputManager->isKeyDown(SDLK_DOWN))
            {
                rotationDirection = STRAIGHT_DOWN;
            }
        }

        if (inputManager->isKeyDown(SDLK_q))
        {
            chargeShot();
            dash = false;
            if (inputManager->isKeyDown(SDLK_UP))
            {
                rotationDirection -= 5.0f;
                if (rotationDirection < UP_ROTATION)
                {
                    rotationDirection = UP_ROTATION;
                }
            }
            else if (inputManager->isKeyDown(SDLK_DOWN))
            {
                rotationDirection += 5.0f;
                if (rotationDirection > STRAIGHT_DOWN)
                {
                    rotationDirection = STRAIGHT_DOWN;
                }
            }
        }

        if (inputManager->isKeyReleased(SDLK_q))
        {
            fire();
        }

        //short jumps
        if (inputManager->isKeyReleased(SDLK_w))
        {
            if (physics->moveSpeed.y < jumpCutoffSpeed)
            {
                physics->moveSpeed.y = jumpCutoffSpeed;
            }
        }

        if (inputManager->isKeyDown(SDLK_LEFT))
        {
            moveButton = true;
            physics->moveSpeed.x = -1;
        }
        else if (inputManager->isKeyDown(SDLK_RIGHT))
        {
            moveButton = true;
            physics->moveSpeed.x = 1;
        }

        //Attempting to change directions in mid air should slow the player down
        if (moveButton && physics->moveSpeed.x * sprite.facing  < 0 )
        {
            sprite.facing *= -1;
            physics->currentSpeed = 0;
            dash = 0;
        }

        if(physics->leftRightSensor)
        {
            physics->currentSpeed = 5; //Not sure what to do here, want to slow down when hitting something but I'm not sure if I want to stop entirely
        }

        //in air acceleration is slow
        if (physics->currentSpeed < 5.0f && moveButton)
        {
            physics->currentSpeed += 0.2f;
        }
        else
        {
            //if the player is not holding down a button, they will decellerate
            if (physics->currentSpeed > 0 && !moveButton)
            {
                physics->currentSpeed -= 0.4f;
            }
            if (physics->currentSpeed < 0)
            {
                physics->currentSpeed = 0;
                physics->moveSpeed.x = 0;
            }
        }

        break;
    }
}

void Player::groundMovement()
{
    if (inputManager->isKeyDown(SDLK_LEFT))
    {
        physics->moveSpeed.x = -1;
        sprite.facing = -1;
        moveButton = true;
    }
    else if (inputManager->isKeyDown(SDLK_RIGHT))
    {
        physics->moveSpeed.x = 1;
        sprite.facing = 1;
        moveButton = true;
    }
}

void Player::chargeShot()
{
    charging = true;
    //if can fire timer
    //can fire
}

void Player::checkCharge(float delta)
{
    if (charging)
    {
        if (chargeLevel < MAX_CHARGE)
        {
            chargeTimer += delta;
            if (chargeTimer >= chargeTime)
            {
                chargeTimer = 0.0f;
                chargeLevel++;
                if(chargeLevel < MAX_CHARGE)
                {
                    Mix_PlayChannel(-1, soundEffects[CHARGE_LEVEL_1_SOUND], 0);
                }
                else
                {
                    Mix_PlayChannel(-1, soundEffects[CHARGE_LEVEL_2_SOUND], 0);
                }
            }
        }
    }
    else
    {
        if (recentFire)
        {
            if (physics->moveSpeed.x != 0) //need to account for being in the air, reset if direction changes
            {
                resetAim();
            }
            else
            {
                resetAimTimer += delta;
                if (resetAimTimer >= resetAimTime)
                {
                    resetAim();
                }
            }
        }
    }
}

void Player::jump()
{
    physics->moveSpeed.y = jumpSpeed;
    //state = Player::IN_AIR;
    if (!charging)
    {
        resetAim();
    }

    if (physics->moveSpeed.x == 0)
    {
        physics->currentSpeed = 0;
    }
    sprite.setParent(nullptr);
    parentObject = nullptr;

    // ResourceManager::PlaySound("PlayerJump");
    Mix_PlayChannel(-1, soundEffects[JUMP_SOUND], 0);
}

void Player::fire()
{
    charging = false;
    recentFire = true;

    if(canFire)
    {
        canFire = false;
        resetAimTimer = 0.0f;

        float launchSpeed = 0.0f;
        float fallSpeed = 0.0f;

        glm::vec2 velocity;
        if (chargeLevel == 0)
        {
            launchSpeed = 7.5f;
            fallSpeed = 0.02f;
        }
        else if(chargeLevel == 1)
        {
            launchSpeed = 10.0f;
            fallSpeed = 0.01f;
        }
        else
        {
            launchSpeed = 15.0f;
            fallSpeed = 0.0f;
        }
        glm::vec2 fireDirection = glm::rotate(aimDirection, glm::radians(rotationDirection * sprite.facing));

        //When firing down, want to make sure that if the player is falling they won't fall faster than the arrow
        //so add the player's fall speed to the arrow.
        if(fireDirection.y > 0 && physics->moveSpeed.y > 0)
        {
            launchSpeed += physics->moveSpeed.y;
        }

        velocity = launchSpeed * fireDirection;

        glm::vec2 firePosition = sprite.getHitBoxCenter();

        Mix_PlayChannel(-1, soundEffects[FIRE_ARROW_SOUND], 0);

        //find and fire first active arrow
        for (int i = 0; i < arrows->size(); i++)
        {
            if (!(*arrows)[i].sprite.active)
            {
                (*arrows)[i].fire(firePosition,
                                  chargeLevel,
                                  velocity,
                                  fallSpeed);
                chargeLevel = 0;
                chargeTimer = 0.0f;

                break;
            }
            /* if (!(*arrows)[i].sprite.active)
             {
                 (*arrows)[i].fire(firePosition,
                                   chargeLevel,
                                   fireDirection * launchSpeed,
                                   fallSpeed);
                 chargeLevel = 0;
                 chargeTimer = 0.0f;

                 break;
             }*/
        }
    }
}

void Player::duck()
{
    ducking = true;
    physics->currentSpeed = duckWalkSpeed;
    dash = false;
    sprite.setHitBox(glm::vec4(0, 16, 30, 16));
    resetAim();
}

void Player::resetAim()
{
    resetAimTimer = 0.0f;
    recentFire = false;
    rotationDirection = 0;
}

void Player::takeHit(int damage)
{
    physics->push(glm::vec2(-sprite.facing, 0.0f), 3.0f);
    charging = false;
    chargeLevel = 0;
    chargeTimer = 0.0f;
    resetAim();

    ducking = false;
    dash = false;
    stand();

    if(health.takeDamage(damage))
    {
        sprite.currentFrame = DEATH_FRAME;
        sprite.active = false;
    }
    else
    {
        Mix_PlayChannel(-1, soundEffects[TAKE_HIT_SOUND], 0);
        sprite.alpha = 0.5f;
    }
}

void Player::die()
{
    currentLives --;
    if(currentLives < 0)
    {
        //game over
        gameOver = true;
        sprite.active = false;
    }
    else
    {
        health = Health(MAX_HEALTH);
        sprite.active = false;
    }
}

bool Player::deathAnimation(float delta)
{
    //Setting this here for when the player falls off the stage.
    sprite.currentFrame = DEATH_FRAME;

    bool end = false;
    deathTimer += delta/deathTime;

    fallSpeed+= 0.5f;
    if(fallSpeed >= 9)
    {
        fallSpeed = 9;
    }

    sprite.Move(glm::vec2(0.0f, fallSpeed));

    if(deathTimer >= 1)
    {
        end = true;
        deathTimer = 0.0f;
        fallSpeed = -9;
    }
    return end;
}

bool Player::canReturnToHub()
{
    bool canReturn = currentLives > 0;
    if(canReturn)
    {
        currentLives--;
    }
    return canReturn;
}


void Player::stand()
{
    sprite.setHitBox(glm::vec4(0, 0, 30, 32));
}
