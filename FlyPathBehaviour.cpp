#include "FlyPathBehaviour.h"

FlyPathBehaviour::FlyPathBehaviour(float speed, std::vector<glm::vec2>& path) : speed(speed), path(path)
{
    end = path.size() - 1;
    current = 0;

    getNewDirection();
}

FlyPathBehaviour::~FlyPathBehaviour()
{
}

void FlyPathBehaviour::update(float delta, Sprite &sprite)
{
    //No point updating if this one isn't moving
    if(path.size() > 1)
    {
        movement(sprite);
    }
}

void FlyPathBehaviour::movement(Sprite &sprite)
{
    glm::vec2 newPosition = sprite.getPosition() + direction * speed;
    glm::vec2 currentPostion = sprite.getPosition();

    glm::vec2 toNextNode = newPosition - nextNode;
    if(toNextNode == glm::vec2(0))
    {
        sprite.SetPosition(newPosition);
        getNewDirection();
    }
    else
    {
        glm::vec2 directionDifference = toNextNode * direction;
        //If the direction to the next node and the current direction are the same signs, object has overshot the next node
        if(directionDifference.x > 0 || directionDifference.y > 0)
        {
            //Snap object to next node's position
            sprite.SetPosition(nextNode);
            getNewDirection();

            //Move the object along towards the new next node by the distance that it overshot
            //This is to insure that the object moves the same amount each frame
            float remainingDistance =  glm::abs(toNextNode.x) + glm::abs(toNextNode.y);
            sprite.Move(direction * remainingDistance);
        }
        else
        {
            sprite.SetPosition(newPosition);
        }
    }
}

void FlyPathBehaviour::findNextNode()
{

    if (current == end)
    {
        current = 0;
    }
    else
    {
        current++;
    }

    // loop behavior not in use
    /*if (forward)
    {
        if (current == end)
        {
            if (loop)
            {
                forward = false;
                current--;
            }
            else
            {
                current = 0;
            }
        }
        else
        {
            current++;
        }
    }
    else
    {
        if (current == 0)
        {
            if (loop)
            {
                forward = true;
                current++;
            }
            else
            {
                current = end;
            }
        }
        else
        {
            current--;
        }
    }*/
}

void FlyPathBehaviour::getNewDirection()
{
    glm::vec2 previousNode = path[current];
    findNextNode();

    nextNode = path[current];

    glm::vec2 toNext = nextNode - previousNode;
    //need to check for 0, normalizing 0 returns NAN
    if(toNext.x == 0 && toNext.y == 0)
    {
        direction = glm::vec2(0);
    }
    else
    {
        direction = glm::normalize(nextNode - previousNode);
        //Only ever want directions of 1, 0, or -1
        //This is mainly for cases when the path is diagonal, though I have seen glm::normalize return weird 0.99995 types of values before, so
        //this is also to be safe.
        //A consequence of this is that diagonal paths must be the same size, i.e. 6 down, 6 right
        direction.x = glm::round(direction.x);
        direction.y = glm::round(direction.y);
    }
}


void FlyPathBehaviour::reset()
{
    current = 0;
    getNewDirection();
}
