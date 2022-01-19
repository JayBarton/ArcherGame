#ifndef TILE_H
#define TILE_H
#include <glm.hpp>
#include "SATCollision.h"

class Tile
{
private:

    const static int AIR_TILE = 5;
    //The attributes of the tile
  //  SDL_Rect box;
	//glm::vec4 box;
	SimpleShape box;
    //The tile type
    int theType;
    //the tile attribuite
    int theAttribuite;

    int theHeights[32];

    void setHeights();

public:
    bool isPassable();

    Tile();
    //Initializes the variables
    Tile( int x, int y, int tileWidth, int tileHeight, int tileType, int tileAttribuite = 0, int tileLayer = 0 );

	int layer;
    glm::vec4 area;

    //Shows the tile
   // void Show(SDL_Rect *tile);

    //Get the tile type
    int getType();
    //get the tile attribuite
    int getAttribuite();

    //Get the collision box
	SimpleShape getBox();
	glm::vec4 getArea();


    int getHeight(int index);

    //set a new tile attibuite overwriting the previous one
    void setAttribuite(int newAttribuite);
    void setType(int newType);
    bool isOccupied;

};

#endif // TILE_H
