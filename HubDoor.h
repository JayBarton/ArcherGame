#ifndef HUBDOOR_H
#define HUBDOOR_H

#include "Door.h"


class HubDoor : public Door
{
public:
    HubDoor(int ID, glm::vec2 position, std::string nextMap, int levelNumber);
    int levelNumber;
    ~HubDoor();

};

#endif // HUBDOOR_H
