#include "Pickup.h"


Pickup::Pickup(glm::vec2 position, int theType, Mix_Chunk* sound)
{
	sprite.SetPosition(position);
	sprite.setHitBox(glm::vec4(0, 0, 32, 32));
	sprite.active = true;
	type = theType;
	soundEffect = sound;
}

void Pickup::apply(Player& player)
{
    Mix_PlayChannel(-1, soundEffect, 0);
}


void Pickup::animate(float delta)
{
    sprite.playAnimation(delta, 4, true);
}


Pickup::~Pickup()
{
}
