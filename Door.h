#pragma once
#include "Sprite.h"
#include <string>
class Door
{
public:
	Door() {}
	//if level > 0 then this is a hub door
	Door(int ID, glm::vec2 position, std::string nextMap, int level = 0, bool locked = false);

	int ID;
	int level;
	bool locked;
	Sprite sprite;
	std::string nextMap;

	~Door();
};

