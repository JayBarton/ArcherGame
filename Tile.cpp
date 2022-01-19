#include "Tile.h"

Tile::Tile()
{
}

Tile::Tile( int x, int y, int tileWidth, int tileHeight, int tileType, int tileAttribuite, int tileLayer )
{
    //Get the offsets
    if(tileAttribuite <= 1)
    {
        box = newSquare(glm::vec2(x + (tileWidth * 0.5f), y + (tileHeight * 0.5f)), tileWidth, tileHeight);
    }
    else
    {
        //half tiles
        box = newSquare(glm::vec2(x + (tileWidth * 0.5f), y + (tileHeight * 0.25f)), tileWidth, tileHeight * 0.5f);
      //  box = testTri(glm::vec2(x + (tileWidth * 0.5f), y + (tileHeight * 0.5f)), tileWidth, tileHeight);
    }

    area = glm::vec4(x, y, tileWidth, tileHeight);

    //Get the tile type and attribuite
    theType = tileType;
    theAttribuite = tileAttribuite;
    layer = tileLayer;
    isOccupied = false;
    setHeights();
}

/*
void Tile::Show(SDL_Rect* tile)
{
    //Show the tile
    if(CollidedWithCamera(camera, box))
    {
        apply_surface( box.x - camera.x , box.y - camera.y, tileSheet, screen, tile );
    }
}
*/
void Tile::setType(int newType)
{
    theType = newType;
}

void Tile::setAttribuite(int newAttribuite)
{
    theAttribuite = newAttribuite;
}

int Tile::getType()
{
    return theType;
}

int Tile::getAttribuite()
{
    return theAttribuite;
}

SimpleShape Tile::getBox()
{
    return box;
}

glm::vec4 Tile::getArea()
{
    return area;
}

bool Tile::isPassable()
{
    return theAttribuite < 1 || theAttribuite > 2;
}

void Tile::setHeights()
{
    if(theType < 3 )
    {
        for(int i = 0; i <32; i ++)
        {
            theHeights[i] =32;
        }
    }
    else if( theType == AIR_TILE)
    {
        for(int i = 0; i <32; i ++)
        {
            theHeights[i] =0;
        }
    }
    else if(theType == 3)
    {
        for(int i = 0; i <32; i ++)
        {
            theHeights[i] = i+1   ;
        }
    }
    else if(theType == 4)
    {
        for(int i = 31; i >=0; i --)
        {
            theHeights[i] = 32 -i  ;
        }
    }
}

int Tile::getHeight(int index)
{
    return theHeights[index];
}


