#include "Collider.h"

Collider::Collider(glm::vec4 dims, int type)
{
	dimensions = dims;
	this->type = type;
}

Collider::~Collider()
{
}

