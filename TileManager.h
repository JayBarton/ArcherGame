#ifndef TILEMANAGER_H
#define TILEMANAGER_H

#include "Tile.h"

#include "Shader.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "Camera.h"
#include <GL/glew.h>


#include <vector>

class TileManager
{
private :

    //Tile constants
    static const int TILE_SPRITES = 13;

    //The different tile sprites
    static const int TILE_BLOCK = 0;
    static const int TILE_WATER = 1;
    static const int TILE_BLANK = 2;

    static const int SLOW_ATTRIBUITE = 1;

    static const int NUMBER_OF_TILES = 6;
    static const int NUMBER_OF_TILE_ATTRIBUITES = 3;

    static const int LEVEL_START_X = 0;
    static const int LEVEL_START_Y = 0;

    //Sprite from the tile sheet
   // SDL_Rect clips[ TILE_SPRITES ];

     //SDL_Rect attribuiteClips[1] ;

public :
	static const int TILE_WIDTH = 32;
	static const int TILE_HEIGHT = 32;

    static TileManager tileManager;
    //the currently selected tile. maybe temporary.
    int currentType;

	int rowTiles;
	int columnTiles;
	int totalTiles;
	int levelWidth;
	int levelHeight;

    //the tiles
    Tile *tiles;

    // the drawn tiles
    std::vector<Tile> foregroundTiles;
	std::vector<Tile> backgroundTiles;

	std::vector<glm::vec4> uvs;

    //initilize variables
    TileManager();

    //load and set the tiles
	bool setTiles(std::ifstream &tileMap, int width, int height);


    //check if a position is out of bounds
    bool outOfBounds(int x, int y);
    bool tileIsPassable(int x, int y);
    bool airPassable(int x, int y);

    int xPositionToIndex(int x);
    int yPositionToIndex(int y);

    int tileHeight(int x, int y);

	//probably not keeping this as I don't actually need it probably
	int getType(int x, int y);

	Tile * getTile(int x, int y);

    SimpleShape getTileBox(int xPosition, int yPosition);

    void PositionToTile(int &x, int &y);

    void setUp(int width, int height);

    //get the tiles from a tile sheet
  //  void clipTiles();

  //  void clipAttribuites();

    //show the tiles on screen
    void showForeground(SpriteRenderer *Renderer, Camera& camera);
	void showBackground(SpriteRenderer *Renderer, Camera& camera);

    //called when the program stops running, clears the tiles
    void clearTiles();
    void cleanUp();

};

#endif // TILEMANAGER_H
