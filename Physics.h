#pragma once
#include "Sprite.h"

#include <glm.hpp>

class Physics
{
public:
	Physics();

	Physics(float fallAcceleration, float maxFallSpeed);

	enum State { ON_GROUND, IN_AIR };
	State state = IN_AIR;

	float currentSpeed = 0.0f;

	glm::vec2 moveSpeed;

	~Physics();

	void update(Sprite &sprite, float delta);

	void push(glm::vec2 velocity, float xSpeed);

	void checkTileBounds(Sprite &sprite);
	void leftRightTileCheck(Sprite &sprite, glm::vec2 tilePoint, const glm::vec4 &hb);
	void checkLevelBounds(Sprite &sprite);

	void reset();

	bool leftRightSensor;
	bool topSensor;
	bool bottomLeftSensor;
	bool bottomRightSensor;

private:

	float fallAcceleration = 0.5f;
	float maxFallSpeed = 9.0f;

	void land();
	void applyGravity(float delta);

	void checkHorizontalCollisions(Sprite & sprite, glm::vec4 &hb);
	void checkVerticalCollisions(Sprite& sprite, glm::vec4& hb);
	void checkBottom(Sprite& sprite, glm::vec4& hb);
	void checkTop(Sprite& sprite, glm::vec4& hb);

};

