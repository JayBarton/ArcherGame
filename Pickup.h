#pragma once
#include "Player.h"
#include "Sprite.h"

#include <glm.hpp>
#include <SDL_mixer.h>

class Player;
class Pickup
{
public:
	Pickup(glm::vec2 position, int type, Mix_Chunk* soundEffect);

	void animate(float delta);

	virtual void apply(Player &player);

	Mix_Chunk* soundEffect = nullptr;

	virtual ~Pickup();

	Sprite sprite;
    int type;
};

