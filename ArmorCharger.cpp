#include "ArmorCharger.h"
#include <iostream>

ArmorCharger::ArmorCharger()
{
	armor.z = 16;
	armor.w = 32;

	ArmorComponent *ac = new ArmorComponent(armor);
	addComponent(ac);
}


void ArmorCharger::update(float delta, Sprite & player)
{
	Enemy::update(delta, player);

	glm::vec2 position = sprite.getPosition();
	if (sprite.facing > 0)
	{
		armor.x = position.x + armor.z;

	}
	else
	{
		armor.x = position.x;

	}
	armor.y = position.y;

}

ArmorCharger::~ArmorCharger()
{
}
