#pragma once
#include "Sprite.h"
#include "GameObject.h"
class GameObject;
class Collider;
class Projectile
{
public:
	Projectile();

	void fire(glm::vec2 start,  glm::vec2 velocity, int type);

	void update(float delta);

	void checkTiles();
	void nearPhase(glm::vec2 point);

	void checkMovables(std::vector<GameObject*>& movables);

	Sprite sprite;

	glm::vec2 moveSpeed;
	glm::vec2 direction;

	bool moving = false;

	~Projectile();

private:
    void checkMovable(GameObject *movable, glm::vec4 hb, glm::vec4 phb);
    //Returns the push of the arrow and the object
    glm::vec2 checkObject(GameObject* object, Collider collider, glm::vec2 objectPosition, glm::vec4 hb, glm::vec4 phb);
};

