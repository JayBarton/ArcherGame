#pragma once
#include <glm.hpp>
#include <vector>
#include "GameObject.h"

class Collider
{
public:
	Collider(){}
	Collider(glm::vec4 dims, int type);

	const static int NORMAL = 0;
	const static int ARMOR = 1;
	const static int WALKABLE = 2;
	const static int DAMAGE = 3;
	const static int BLOCK = 4; //Used exclusively for block types, don't use it for anything else.

	int type;
	//x,y are offsets, z,x are width and height
	glm::vec4 dimensions;

	~Collider();
};

