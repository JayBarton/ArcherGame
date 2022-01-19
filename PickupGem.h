#pragma once
#include "Pickup.h"


class PickupGem : public Pickup
{
public:
    PickupGem(glm::vec2 position, int type, Mix_Chunk* soundEffect);

    void apply(Player &player);

    ~PickupGem();
};
