#include "HubDoor.h"

HubDoor::HubDoor(int ID, glm::vec2 position, std::string nextMap, int levelNumber) : Door::Door(ID, position, nextMap ), levelNumber(levelNumber)
{

}

HubDoor::~HubDoor()
{
}
