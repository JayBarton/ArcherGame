#include "TileManager.h"
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>

#include <gtc/matrix_transform.hpp>

TileManager TileManager::tileManager;

TileManager::TileManager()
{

}

void TileManager::setUp(int width, int height)
{
    rowTiles = width;
    columnTiles = height;
    totalTiles = rowTiles * columnTiles;

    levelWidth = rowTiles * TILE_WIDTH;
    levelHeight = columnTiles * TILE_HEIGHT;

    tiles = new Tile[totalTiles];
}

bool TileManager::setTiles(std::ifstream& tileMap, int width, int height)
{
    setUp(width, height);

    //The tile offsets
    int startX = LEVEL_START_X;
    int startY = LEVEL_START_Y;
    int x = startX;
    int y = startY;

    tileMap.ignore(1);

    for (int i = 0; i < totalTiles; i++)
    {
        //Determines what kind of tile will be made
        int tileType = 0;
        int tileAttribuite = 0;
        int tileLayer = 0;

        //Read tile from map file
        //ignore the first "
        tileMap.ignore(1);
        tileMap >> tileType >> tileAttribuite >> tileLayer;

        //ignore the second "
        tileMap.ignore(2);

        //If the number is a valid tile number
        //attribuite needs work
        if ((tileType >= -1) && (tileType < TILE_SPRITES) && tileAttribuite >= 0)
        {
            //tiles[i] = new Tile( x, y, TILE_WIDTH, TILE_HEIGHT, tileType, tileAttribuite );
            tiles[i] = Tile(x, y, TILE_WIDTH, TILE_HEIGHT, tileType, tileAttribuite, tileLayer);

            if (tileType >= 0)
            {
                if (tileLayer == 0)
                {
                    foregroundTiles.push_back(tiles[i]);
                }
                else
                {
                    backgroundTiles.push_back(tiles[i]);
                }
            }
        }
        //If we don't recognize the tile type
        else
        {
            //Stop loading map
            return false;
        }

        //Move to next tile spot
        x += TILE_WIDTH;

        //If we've gone too far
        if (x >= levelWidth + startX)
        {
            //Move back
            x = startX;

            //Move to the next row
            y += TILE_HEIGHT;
        }
    }
    return true;
}

void TileManager::showForeground(SpriteRenderer *Renderer, Camera& camera)
{
    for (int i = 0; i < foregroundTiles.size(); i++)
    {
        glm::vec4 box = foregroundTiles[i].getArea();
        if(camera.onScreen(glm::vec2(box.x, box.y), glm::vec2(box.z, box.w)))
        {
            Renderer->setUVs(uvs[foregroundTiles[i].getType()]);
            Texture2D displayTexture = ResourceManager::GetTexture("tiles");
            Renderer->DrawSprite(displayTexture, glm::vec2(box.x, box.y), 0.0f, glm::vec2(32, 32));
        }
    }
}

void TileManager::showBackground(SpriteRenderer *Renderer, Camera& camera)
{
    for (int i = 0; i < backgroundTiles.size(); i++)
    {
        glm::vec4 box = backgroundTiles[i].getArea();
        if(camera.onScreen(glm::vec2(box.x, box.y), glm::vec2(box.z, box.w)))
        {
            Renderer->setUVs(uvs[backgroundTiles[i].getType()]);
            Texture2D displayTexture = ResourceManager::GetTexture("tiles");
            Renderer->DrawSprite(displayTexture, glm::vec2(box.x, box.y), 0.0f, glm::vec2(32, 32));
        }
    }
}

bool TileManager::outOfBounds(int x, int y)
{
    bool out = false;
    if(x < LEVEL_START_X  || x > levelWidth + TILE_WIDTH || y < LEVEL_START_Y || y > levelHeight + TILE_HEIGHT)
    {
        out = true;
    }

    return out;
}

bool TileManager::tileIsPassable(int xPos, int yPos)
{
    if(xPos >= 0 && xPos < levelWidth && yPos >= 0 && yPos < levelHeight)
    {
        return getTile(xPos, yPos)->isPassable();
    }
    else
    {
        return true;
    }
}

bool TileManager::airPassable(int x, int y)
{
    if(x >= 0 && x < levelWidth && y >= 0 && y < levelHeight)
    {
        return getTile(x, y)->getAttribuite() == 0;
    }
    else
    {
        return true;
    }
}


//returns the height of a tile at an x point
//subtracting the passed in x value from the tile's x position will equal a number between 0 and 31,which is the tile width - 1
//this number is used as an index to return the height of a tile on that point
int TileManager::tileHeight(int x, int y)
{
    int height = 0;
    Tile theTile = *getTile(x, y);

    height = theTile.getHeight(x - theTile.getArea().x );
    return height;
}

int TileManager::getType(int x, int y)
{
    return getTile(x, y)->getType();
}

Tile * TileManager::getTile(int x, int y)
{
    return &tiles[xPositionToIndex(x) + yPositionToIndex(y) * rowTiles];
}

int TileManager::xPositionToIndex(int x)
{
    return x/32;
}

int TileManager::yPositionToIndex(int y)
{
    return y/32;
}

SimpleShape TileManager::getTileBox(int xPosition, int yPosition)
{
    return getTile(xPosition, yPosition)->getBox();
}

void TileManager::PositionToTile(int &xass, int &yass)
{
    float xTemp = xass;
    float yTemp = yass;
    xTemp /=32;
    yTemp /= 32;
    xTemp = floor(xTemp + 0.5f);
    yTemp =floor (yTemp + 0.5f);
    xTemp *= 32;
    yTemp *= 32;
    xass = xTemp;
    yass = yTemp;
}

void TileManager::clearTiles()
{
    backgroundTiles.clear();
    foregroundTiles.clear();
    delete[] tiles;
    tiles = nullptr;
}

void TileManager::cleanUp()
{
    clearTiles();
}
