#pragma once
#include "Enemy.h"
#include "ArmorComponent.h"
class ArmorCharger : public Enemy //inhertance temporary, want to do composition
{
public:
	ArmorCharger();

	glm::vec4 armor; //this is purely visual

	void update(float delta, Sprite& player);

	~ArmorCharger();
};

