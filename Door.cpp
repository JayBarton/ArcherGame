#include "Door.h"

Door::Door(int ID, glm::vec2 position, std::string nextMap, int level /*= 0*/, bool locked /*= false*/) : ID(ID), level(level), locked(locked)
{
	sprite.SetPosition(position);
	sprite.setHitBox(glm::vec4(0, 0, 32, 32));
	sprite.color = glm::vec3(1.0f, 0.0f, 1.0f);
	this->nextMap = nextMap + ".map";
}

Door::~Door()
{
}
