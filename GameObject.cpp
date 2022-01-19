#include "GameObject.h"


GameObject::GameObject()
{
}

int GameObject::addCollider(glm::vec4 dims, int type)
{
	Collider c(dims, type);
	colliders.push_back(c);
	return colliders.size() - 1;
}


GameObject::~GameObject()
{
}
