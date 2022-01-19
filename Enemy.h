#pragma once

#include "Sprite.h"
#include "Physics.h"
#include "WalkBehaviour.h"

#include "GameObject.h"

#include <glm.hpp>

class Enemy : public GameObject
{
public:
    SpawnArea spawnArea; //Not sure how to handle this, here for now

    std::vector<Collider> colliders;

    int addCollider(glm::vec4, int type);


};

