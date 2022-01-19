#include <string>
#include <cstdio>
#include <iostream>

#include <GL/glew.h>

#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "Timing.h"
#include "TextRenderer.h"
#include "Player.h"
#include "InputManager.h"

#include "Arrow.h"
#include "Projectile.h"

#include "TileManager.h"

#include "ChargeEnemy.h"
#include "ProjectileEnemy.h"
#include "Walker.h"
#include "FlyChaser.h"
#include "FlyShooter.h"
#include "Block.h"
#include "Pickup.h"
#include "PickupStars.h"
#include "PickupHealth.h"
#include "PickupLives.h"
#include "PickupGem.h"

#include "Collider.h"
#include "Door.h"

#include "Target.h"

#include "Explosion.h"

#include <vector>
#include <map>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_mixer.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

#include <sstream>
#include <fstream>

#include <algorithm>
#include <random>
#include <ctime>

void init();
void loadMap(std::string nextMap, int idOfPrevious); //-1 means can't go back
void loadHub(std::string hub, int idOfPrevious, int currentLevel);
void returnToHub();
void setPickups(int numberOfPickups, std::ifstream& tileMap, std::string mapName);
void setEnemies(int numberOfEnemies, std::ifstream& map);
void setMovables(int numberOfMovables, std::ifstream & map);
void setTargets(int numberOfTargets, std::ifstream& map);
void newExplosion(glm::vec2 position, glm::vec2 size, glm::vec2 originalSize);
void exitMap();
void newMap(Door &door);
void Update(float deltaTime);
void playerUpdate(float deltaTime);
void updateActiveEnemies(float deltaTime);
void updateArrows(float deltaTime);
void clearDeadEnemies();
void spawnAllOnScreenEnemies();
void checkEnemySpawn();
void drawBackground();
void Draw();
void DrawGameCompleteMenu();
void DrawGemMenu();
void DrawGameOverMenu();
void DrawPauseMenuInHub();
void DrawPauseMenu();
void DrawTitleScreen();
void DrawUI();
void HandlePostProcessing();
void DrawTargets();
void DrawExplosions();
void DrawEnemies();
void DrawMovingObjects();
void DrawProjectiles();
void DrawPickups();
void DrawDoors();
void DrawArrows();
void drawButton(Texture2D& border, int width, int height, glm::vec2 position, glm::vec3 color);
void restartLevel();
void completeLevel();
void clear();
void clearEnemies();
void clearProjectiles();
void clearPickups();
void clearGems();
void clearMovingObjects();
void removeEnemy(int i);
void close();

void moveMenu(int i);

std::string intToString(int i);
Mix_Chunk* loadSound(const std::string& fileName);
Mix_Music* loadMusic(const std::string& fileName);

enum GameState
{
    TITLE_SCREEN, PLAYING, PAUSED, PAUSED_FROM_HUB, MAP_TRANSITION, DEAD,
    DEATH_TRANSITION, GAME_OVER, LEVEL_COMPLETE, RETURN_TO_HUB, GEM_MESSAGE,
    GAME_COMPLETE
};

GameState state = TITLE_SCREEN;

// The Width of the screen
const static GLuint SCREEN_WIDTH = 800;
// The height of the screen
const static GLuint SCREEN_HEIGHT = 600;

const static int LIVES_INDEX = 0;
const static int HEALTH_INDEX = 1;
const static int STARS_START_INDEX = 2;
const static int STARS_MEDIUM_INDEX = 3;
const static int STARS_END_INDEX = 4;
const static int GEM_INDEX = 5;

const static int WALKER = 0;
const static int CHARGER = 1;
const static int ARMOR_CHARGER = 2;
const static int SHOOTER = 3;
const static int ARMOR_SHOOTER = 4;
const static int FLY_CHASER = 5;
const static int FLY_SHOOTER = 6;

const static int NUMBER_OF_LEVELS = 4;
const static int LAST_LEVEL = 5;

const static int UNLOCKED_DOOR_INDEX = 0;
const static int LOCKED_DOOR_INDEX = 1;

const static int PAUSE_MENU_UNPAUSE = 0;
const static int PAUSE_MENU_HUB = 1;
const static int PAUSE_MENU_QUIT = 2;

const static int PAUSE_MENU_FROM_HUB_UNPAUSE = 0;
const static int PAUSE_MENU_FROM_HUB_QUIT = 1;

const static int QUIT_NO = 0;
const static int QUIT_YES = 1;

const static int NUMBER_OF_GEMS = 4;

const static int SOUND_INDICES = 8;

const static int PLAYER_SOUNDS_INDEX = 0;
const static int PICKUP_SOUNDS_INDEX = 1;
const static int ARROW_SOUNDS_INDEX = 2;
const static int CHARGER_SOUNDS_INDEX = 3;
const static int PROJECTILE_SOUNDS_INDEX = 4;
const static int BOMB_SOUNDS_INDEX = 5;
const static int ENEMY_DEATH_SOUNDS_INDEX = 6;
const static int MENU_SOUNDS_INDEX = 7;

//For the game complete screen
float gemRotation = 0.0f;
Sprite gemSprite;

int levelWidth;
int levelHeight;

int currentLevel = -1;
int nextLevel = 1;

int menuIndex = 0;

SDL_Window *window;
SpriteRenderer *Renderer;
SpriteRenderer *outlineRenderer;

TextRenderer *Text;

GLuint shapeVAO;

Camera camera;

Player player;

std::vector<GameObject*> movingObjects;
std::vector<GameObject*> activeEnemies;
std::vector<GameObject*> inactiveEnemies;

//drawn and collidable
std::vector<Pickup*> activePickups;
//to remove at the end of the level
//benefit of this set up is it allows pickups to easily respawn if a level
//restarts on player death, and then be deleted and set up when they move to another level
std::vector<Pickup*> collectedPickups;

//Using this to prevent pickups from respawning on map load
//All pickups will go in here when a map loads
//cleared out when the player dies or when they move through a one way door(id < 0)
std::unordered_map<std::string, std::vector<Pickup*>> spawnedPickups;

std::unordered_map<std::string, bool> collectedGems;

std::vector<Target> targets;

InputManager inputManager;

std::vector<Door> doors;
std::vector<Arrow> arrows;
std::vector<Projectile> projectiles;

std::vector<Explosion> explosions;

std::string mapName;
std::string levelDirectory = "Levels/";
std::string backgroundDirectory = "Textures/";
std::string hub = "hub.map";

std::string activeEnemiesString;

glm::vec2 playerStart;

std::vector<glm::vec4> doorUVs;
//std::vector <glm::vec4> enemyUVs[7];
std::vector <std::vector<glm::vec4>> enemyUVs;
std::vector<glm::vec4> pickupUVs[6];
std::vector<glm::vec4> projectileUVs[2];
std::vector<glm::vec4> targetUVs;
std::vector<glm::vec4> arrowUVs;

std::vector<glm::vec4> movingObjectUVs;
glm::vec4 buttonUV;

std::vector<glm::vec4> smallExplosionUV;
std::vector<glm::vec4> largeExplosionUV;

std::vector<std::vector<Mix_Chunk*>> soundEffects;
Mix_Music* bgm;
Mix_Music* gameOverTheme;
Mix_Music* levelCompleteTheme;

bool levelExit = false;
bool levelComplete = false;

float transitionTimer = 0.0f;
float mapTransitionTime = 0.3f;

float deathTimer = 0.0f;
float deathTransitionTime = 0.3f;

float transitionAlpha = 0.0f;

Door *exitDoor = nullptr;

//all post process stuff
bool shake = false;
glm::vec2 viewportCentre = glm::vec2(400,300); //Lets say screen size is 800 x 600
float radius = 30.0;
float randomAngle = rand()%360;
float offset = ( sin(randomAngle) * radius, cos(randomAngle) * radius);  //create offset 2d vector

GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
GLuint RBO; // RBO is used for multisampled color buffer
GLuint VAO;

Texture2D Texture;

void postprocess();

int main(int argc, char** argv)
{
    init();

    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    //Main loop flag
    bool isRunning = true;

    //Event handler
    SDL_Event event;

    int fps = 0;
    fps = 0;
    FPSLimiter fpsLimiter;
    fpsLimiter.setMaxFPS(69990.0f);

    const float MS_PER_SECOND = 1000;
    const float DESIRED_FPS = 60;
    const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
    const float MAXIMUM_DELTA_TIME = 1.0f;

    const int MAXIMUM_STEPS = 6;

    float previousTicks = SDL_GetTicks();

    GLfloat verticies[] =
    {
        0.0f, 1.0f, // Left
        1.0f, 0.0f, // Right
        0.0f, 0.0f,  // Top
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    GLuint tVBO;
    glGenVertexArrays(1, &shapeVAO);
    glGenBuffers(1, &tVBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(shapeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, tVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    camera.setPosition(glm::vec2(0.0f, 0.0f));
    camera.update();

    ResourceManager::LoadShader("Shaders/spriteVertexShader.txt", "Shaders/spriteFragmentShader.txt", nullptr, "sprite");
    ResourceManager::LoadShader("Shaders/shapeVertexShader.txt", "Shaders/shapeFragmentShader.txt", nullptr, "shape");

    ResourceManager::LoadShader("Shaders/spriteVertexShader.txt", "Shaders/sliceFragmentShader.txt", nullptr, "slice");

    ResourceManager::LoadShader("Shaders/postVertexShader.txt", "Shaders/postFragmentShader.txt", nullptr, "postprocessing");

    ResourceManager::LoadTexture("Textures/tilesheet.png", "tiles");

    ResourceManager::LoadTexture("Textures/sprites.png", "sprites");

    ResourceManager::LoadTexture("Menu Screens/PauseScreen.png", "PauseScreen");

    postprocess();

    ResourceManager::GetShader("shape").Use().SetMatrix4("projection", camera.getCameraMatrix());
    ResourceManager::GetShader("shape").SetFloat("alpha", 1.0f);

    ResourceManager::GetShader("slice").Use().SetInteger("image", 0);
    ResourceManager::GetShader("slice").SetMatrix4("projection", camera.getCameraMatrix());

    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", camera.getCameraMatrix());

    doorUVs = ResourceManager::GetTexture("sprites").GetUVs(0, 0, 32, 32, 2, 1);

    enemyUVs.resize(7);

    enemyUVs[WALKER] = ResourceManager::GetTexture("sprites").GetUVs(0, 448, 32, 32, 9, 1);
    enemyUVs[CHARGER] = ResourceManager::GetTexture("sprites").GetUVs(0, 320, 32, 32, 9, 2, 17);
    enemyUVs[ARMOR_CHARGER] = ResourceManager::GetTexture("sprites").GetUVs(0, 384, 32, 32, 9, 2, 17);
    enemyUVs[SHOOTER] = ResourceManager::GetTexture("sprites").GetUVs(0, 480, 32, 64, 3, 1);
    enemyUVs[ARMOR_SHOOTER] = ResourceManager::GetTexture("sprites").GetUVs(0, 544, 32, 64, 3, 1);
    enemyUVs[FLY_CHASER] = ResourceManager::GetTexture("sprites").GetUVs(0, 256, 32, 32, 8, 2);
    enemyUVs[FLY_SHOOTER] = ResourceManager::GetTexture("sprites").GetUVs(0, 608, 64, 64, 5, 1);

    movingObjectUVs = ResourceManager::GetTexture("sprites").GetUVs(64, 0, 32, 32, 1, 1);

    buttonUV = ResourceManager::GetTexture("sprites").GetUVs(192, 32, 32, 32, 1, 1)[0];

    pickupUVs[LIVES_INDEX] = ResourceManager::GetTexture("sprites").GetUVs(0, 32, 32, 32, 4, 1);
    pickupUVs[HEALTH_INDEX] = ResourceManager::GetTexture("sprites").GetUVs(0, 64, 32, 32, 4, 1);
    pickupUVs[STARS_START_INDEX] = ResourceManager::GetTexture("sprites").GetUVs(0, 96, 32, 32, 4, 1);
    pickupUVs[STARS_MEDIUM_INDEX] = ResourceManager::GetTexture("sprites").GetUVs(0, 128, 32, 32, 4, 1);
    pickupUVs[STARS_END_INDEX] = ResourceManager::GetTexture("sprites").GetUVs(0, 160, 32, 32, 4, 1);
    pickupUVs[GEM_INDEX] = ResourceManager::GetTexture("sprites").GetUVs(128, 64, 32, 32, 5, 1);

    projectileUVs[0] = ResourceManager::GetTexture("sprites").GetUVs(96, 0, 32, 16, 1, 2);

    arrowUVs = ResourceManager::GetTexture("sprites").GetUVs(128, 224, 32, 8, 2, 1);

    targetUVs = ResourceManager::GetTexture("sprites").GetUVs(128, 32, 32, 32, 2, 1);

    for(int i = 0; i < 4; i++)
    {
        targetUVs.push_back(pickupUVs[i+1][0]);
    }

    smallExplosionUV = ResourceManager::GetTexture("sprites").GetUVs(128, 0, 32, 32, 4, 1);
    largeExplosionUV = ResourceManager::GetTexture("sprites").GetUVs(96, 480, 64, 64, 2, 2);

    auto leadUp = ResourceManager::GetTexture("sprites").GetUVs(0, 192, 32, 32, 6, 1);
    //lead up to chaser explosion
    for(int i = 0; i < 6; i ++)
    {
        enemyUVs[FLY_CHASER].push_back(leadUp[i]);
    }
    //chaser explosion animation
    for(int i = 0; i < 4; i ++)
    {
        enemyUVs[FLY_CHASER].push_back(largeExplosionUV[i]);
    }

    std::vector<glm::vec4> playerUVs = ResourceManager::GetTexture("sprites").GetUVs(0, 672, 32, 32, 10, 6, 59);
    std::vector<glm::vec4> bowUVs = ResourceManager::GetTexture("sprites").GetUVs(0, 224, 32, 32, 4, 1);

    //used for the game complete screen
    gemSprite.uv = &pickupUVs[GEM_INDEX];
    gemSprite.currentFrame = 0;

    soundEffects.resize(SOUND_INDICES);
    soundEffects[PLAYER_SOUNDS_INDEX].push_back(loadSound("Sounds/Jump.wav")); //jump
    soundEffects[PLAYER_SOUNDS_INDEX].push_back(loadSound("Sounds/charge1.wav")); //charge level 1
    soundEffects[PLAYER_SOUNDS_INDEX].push_back(loadSound("Sounds/charge2.wav")); //charge level 2
    soundEffects[PLAYER_SOUNDS_INDEX].push_back(loadSound("Sounds/arrow.wav")); //fire arrow
    soundEffects[PLAYER_SOUNDS_INDEX].push_back(loadSound("Sounds/playerhit.wav")); //take hit
    soundEffects[PLAYER_SOUNDS_INDEX].push_back(loadSound("Sounds/LifePickup.wav")); //including in the player so the sound can be played on collecting 100 stars

    soundEffects[PICKUP_SOUNDS_INDEX].push_back(loadSound("Sounds/StarPickup.wav")); //stars
    soundEffects[PICKUP_SOUNDS_INDEX].push_back(loadSound("Sounds/HealthPickup.wav")); //hearts
    soundEffects[PICKUP_SOUNDS_INDEX].push_back(loadSound("Sounds/LifePickup.wav")); //lives
    soundEffects[PICKUP_SOUNDS_INDEX].push_back(loadSound("Sounds/GemPickup.wav")); //gems

    soundEffects[CHARGER_SOUNDS_INDEX].push_back(loadSound("Sounds/ChargerNotice.wav"));

    soundEffects[PROJECTILE_SOUNDS_INDEX].push_back(loadSound("Sounds/projectile1.wav")); //projectile enemy
    soundEffects[PROJECTILE_SOUNDS_INDEX].push_back(loadSound("Sounds/projectile2.wav")); //fly shooter

    soundEffects[ARROW_SOUNDS_INDEX].push_back(loadSound("Sounds/enemyHit.wav")); //normal hit
    soundEffects[ARROW_SOUNDS_INDEX].push_back(loadSound("Sounds/armorHit.wav")); //armor hit

    soundEffects[BOMB_SOUNDS_INDEX].push_back(loadSound("Sounds/Explosion.wav")); //explode

    soundEffects[ENEMY_DEATH_SOUNDS_INDEX].push_back(loadSound("Sounds/EnemyDeath.wav"));

    soundEffects[MENU_SOUNDS_INDEX].push_back(loadSound("Sounds/menuSelect.wav"));

    bgm = loadMusic("Sounds/Music/level_1sttime.wav");
    gameOverTheme = loadMusic("Sounds/Music/gameover.wav");
    levelCompleteTheme = loadMusic("Sounds/Music/youwin.wav");

    Mix_Volume(-1, 8);
    Mix_VolumeMusic(8);

    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);
    Shader oShader = ResourceManager::GetShader("slice");
    outlineRenderer = new SpriteRenderer(oShader);
    outlineRenderer->setUVs();


    TileManager::tileManager.uvs = ResourceManager::GetTexture("tiles").GetUVs(32, 32);

    Text = new TextRenderer(800, 600);
    Text->Load("fonts/Teko-Light.TTF", 30);

    player.init(&inputManager, &arrows, playerUVs, bowUVs, soundEffects[PLAYER_SOUNDS_INDEX]);
    loadHub(hub, currentLevel, nextLevel);

    arrows.resize(20);
    for(int i = 0; i < 20; i ++)
    {
        arrows[i].setSounds(soundEffects[ARROW_SOUNDS_INDEX]);
    }
    projectiles.resize(20);

    while (isRunning)
    {
        GLfloat timeValue = SDL_GetTicks() / 1000.0f;
        //Calculate deltatime of current frame
        GLfloat currentFrame = timeValue;
        deltaTime = currentFrame - lastFrame;
        deltaTime = glm::clamp(deltaTime, 0.0f, 0.02f); //Clamped in order to prevent odd updates if there is a pause
        lastFrame = currentFrame;

        fpsLimiter.beginFrame();

        float newTicks = SDL_GetTicks();
        float frameTime = newTicks - previousTicks;
        previousTicks = newTicks;

        float totalDeltaTime = frameTime / DESIRED_FRAMETIME; //Consider deleting all of this.

        inputManager.update(deltaTime);

        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                inputManager.pressKey(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                inputManager.releaseKey(event.key.keysym.sym);
                break;
            }
        }

        if (inputManager.isKeyPressed(SDLK_ESCAPE))
        {
            if(state == PAUSED || state == PAUSED_FROM_HUB)
            {
                state = PLAYING;
            }
            else if (state == PLAYING)
            {
                if(mapName == hub)
                {
                    state = PAUSED_FROM_HUB;
                    menuIndex = PAUSE_MENU_FROM_HUB_UNPAUSE;
                }
                else
                {
                    state = PAUSED;
                    menuIndex = PAUSE_MENU_UNPAUSE;

                }
            }
        }

        if (inputManager.isKeyPressed(SDLK_BACKSPACE))
        {
            isRunning = false;
        }

        if(state == TITLE_SCREEN)
        {
            if(inputManager.isKeyPressed(SDLK_LEFT))
            {
                moveMenu(-1);
                if(menuIndex < 0)
                {
                    menuIndex = 1;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RIGHT))
            {
                moveMenu(1);
                if(menuIndex > 1)
                {
                    menuIndex = 0;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RETURN))
            {
                switch(menuIndex)
                {
                case 0:
                    state = PLAYING;
                    Mix_PlayMusic(bgm, -1);
                    break;
                case 1:
                    isRunning = false;
                    break;
                }
            }
        }
        else if(state == PAUSED)
        {
            if(inputManager.isKeyPressed(SDLK_LEFT))
            {
                moveMenu(-1);
                if(menuIndex < 0)
                {
                    menuIndex = PAUSE_MENU_QUIT;
                }
                else if(menuIndex == PAUSE_MENU_HUB && player.currentLives == 0)
                {
                    menuIndex--;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RIGHT))
            {
                moveMenu(1);
                if(menuIndex > PAUSE_MENU_QUIT)
                {
                    menuIndex = PAUSE_MENU_UNPAUSE;
                }
                else if(menuIndex == PAUSE_MENU_HUB && player.currentLives == 0)
                {
                    menuIndex++;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RETURN))
            {
                switch(menuIndex)
                {
                case PAUSE_MENU_UNPAUSE:
                    state = PLAYING;
                    break;
                case PAUSE_MENU_HUB:
                    if(player.canReturnToHub())
                    {
                        returnToHub();
                        state = PLAYING;
                    }
                    break;
                case PAUSE_MENU_QUIT:
                    isRunning = false;
                    break;
                }
            }
        }
        else if(state == PAUSED_FROM_HUB)
        {
            if(inputManager.isKeyPressed(SDLK_LEFT))
            {
                moveMenu(-1);
                if(menuIndex < 0)
                {
                    menuIndex = PAUSE_MENU_FROM_HUB_QUIT;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RIGHT))
            {
                moveMenu(1);
                if(menuIndex > PAUSE_MENU_FROM_HUB_QUIT)
                {
                    menuIndex = PAUSE_MENU_FROM_HUB_UNPAUSE;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RETURN))
            {
                switch(menuIndex)
                {
                case PAUSE_MENU_FROM_HUB_UNPAUSE:
                    state = PLAYING;
                    break;
                case PAUSE_MENU_FROM_HUB_QUIT:
                    isRunning = false;
                    break;
                }
            }
        }
        else if(state == GAME_COMPLETE)
        {
            if(inputManager.isKeyPressed(SDLK_LEFT))
            {
                moveMenu(-1);
                if(menuIndex < 0)
                {
                    menuIndex = 1;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RIGHT))
            {
                moveMenu(1);
                if(menuIndex > 1)
                {
                    menuIndex = 0;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RETURN))
            {
                switch(menuIndex)
                {
                case 0:
                    currentLevel = -1;
                    loadHub(hub, currentLevel, nextLevel);
                    player.setUp();
                    state = RETURN_TO_HUB;
                    break;
                case 1:
                    isRunning = false;
                    break;
                }
            }
            gemRotation += 350.0f * deltaTime;
            gemSprite.playAnimation(deltaTime, 4, true);

        }
        else if(state == GAME_OVER)
        {
            if(inputManager.isKeyPressed(SDLK_LEFT))
            {
                moveMenu(-1);
                if(menuIndex < 0)
                {
                    menuIndex = 1;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RIGHT))
            {
                moveMenu(1);
                if(menuIndex > 1)
                {
                    menuIndex = 0;
                }
            }
            else if(inputManager.isKeyPressed(SDLK_RETURN))
            {
                switch(menuIndex)
                {
                case 0:
                    if(nextLevel == LAST_LEVEL - 1)
                    {
                        if(player.collectedGems == NUMBER_OF_GEMS)
                        {
                            //open last door
                            nextLevel ++;
                        }
                    }
                    loadHub(hub, currentLevel, nextLevel);
                    player.setUp();
                    player.setStatus();
                    state = PLAYING;
                    Mix_PlayMusic(bgm, -1);
                    break;
                case 1:
                    isRunning = false;
                    break;
                }
            }
        }
        else
        {
            Update(deltaTime);
        }

        Draw();
        fps = fpsLimiter.end();
    }

    close();

    return 0;
}

void postprocess()
{
    Texture = Texture2D();
    // Initialize renderbuffer/framebuffer object
    glGenFramebuffers(1, &MSFBO);
    glGenFramebuffers(1, &FBO);
    glGenRenderbuffers(1, &RBO);

    // Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT); // Allocate storage for render buffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO); // Attach MS render buffer object to framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

    // Also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    Texture.Generate(SCREEN_WIDTH, SCREEN_HEIGHT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.ID, 0); // Attach texture to framebuffer as its color attachment
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Initialize render data and uniforms
    // Configure VAO/VBO
    GLuint VBO;
    GLfloat vertices[] =
    {
        // Pos        // Tex
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    ResourceManager::GetShader("postprocessing").Use().SetInteger("scene", 0, GL_TRUE);
    GLfloat offset = 1.0f / 300.0f;
    GLfloat offsets[9][2] =
    {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right
    };
    glUniform2fv(glGetUniformLocation(ResourceManager::GetShader("postprocessing").ID, "offsets"), 9, (GLfloat*)offsets);
    GLint edge_kernel[9] =
    {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(ResourceManager::GetShader("postprocessing").ID, "edge_kernel"), 9, edge_kernel);
    GLfloat blur_kernel[9] =
    {
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    };
    glUniform1fv(glGetUniformLocation(ResourceManager::GetShader("postprocessing").ID, "blur_kernel"), 9, blur_kernel);
}


void init()
{
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    SDL_Init(SDL_INIT_EVERYTHING);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError());
    }

    //Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    SDL_GLContext context; //check if succesfully created later

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //For multisampling
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("Archer Game", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, flags);

    context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return;
    }

    SDL_GL_SetSwapInterval(1);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void loadMap(std::string nextMap, int idOfPrevious)
{
    std::ifstream map(levelDirectory + nextMap);

    mapName = nextMap;

    int xTiles;
    int yTiles;
    std::string bg = "";

    while (map.good())
    {
        std::string thing;
        map >> thing;

        if (thing == "Level")
        {
            map >> xTiles >> yTiles;
            TileManager::tileManager.setTiles(map, xTiles, yTiles);
        }
        else if (thing == "Pickups")
        {
            int numberOfPickups;
            map >> numberOfPickups;
            setPickups(numberOfPickups, map, mapName);
        }
        else if (thing == "Enemies")
        {
            int numberOfEnemies;
            map >> numberOfEnemies;
            setEnemies(numberOfEnemies, map);
        }
        else if(thing == "Movables")
        {
            int numberOfMovables;
            map >> numberOfMovables;
            setMovables(numberOfMovables, map);
        }
        else if (thing == "Start")
        {
            glm::vec2 s;
            //figure out a better way to skip lines later
            map >> s.x >> s.y;
            if(idOfPrevious < 0)
            {
                playerStart = s;
            }
        }
        else if (thing == "Doors")
        {
            int numberOfDoors;
            map >> numberOfDoors;

            doors.resize(numberOfDoors);

            for (int i = 0; i < numberOfDoors; i++)
            {
                int id;
                glm::vec2 position;
                std::string next;
                map >> id >> position.x >> position.y >> next;
                doors[i] = Door(id, position, next);
                if (idOfPrevious >= 0 && idOfPrevious == id)
                {
                    playerStart = position;
                }
            }
        }
        else if(thing == "Targets")
        {
            int numberOfTargets;
            map >> numberOfTargets;
            setTargets(numberOfTargets, map);
        }
        else if(thing == "Background")
        {
            map >> bg;
        }
    }
    if(bg == "")
    {
        std::string directory = backgroundDirectory + "noback.png";
        ResourceManager::LoadTexture(directory.c_str(), "bg");
    }
    else
    {
        bg += ".png";
        std::string directory = backgroundDirectory + bg;
        ResourceManager::LoadTexture(directory.c_str(), "bg");

    }
    map.close();

    player.setSprite(playerStart);

    levelWidth = xTiles * TileManager::TILE_WIDTH;
    levelHeight = yTiles * TileManager::TILE_HEIGHT;
    camera = Camera(800, 600, levelWidth, levelHeight);

    camera.setPosition(player.sprite.getPosition());
    camera.update();

    spawnAllOnScreenEnemies();

    levelExit = false;
}


void loadHub(std::string hub, int idOfPrevious, int currentLevel)
{
    std::ifstream map(levelDirectory + hub);

    mapName = hub;

    int xTiles;
    int yTiles;

    while (map.good())
    {
        std::string thing;
        map >> thing;

        if (thing == "Level")
        {
            map >> xTiles >> yTiles;
            TileManager::tileManager.setTiles(map, xTiles, yTiles);
        }
        else if (thing == "Start")
        {
            glm::vec2 s;
            //figure out a better way to skip lines later
            map >> s.x >> s.y;
            if(idOfPrevious < 0)
            {
                playerStart = s;
            }
        }
        else if (thing == "Doors")
        {
            int numberOfDoors;
            map >> numberOfDoors;

            doors.resize(numberOfDoors);

            for (int i = 0; i < numberOfDoors; i++)
            {
                int id;
                glm::vec2 position;
                std::string next;
                int levelNumber;
                map >> id >> position.x >> position.y >> next >> levelNumber;
                //compare levelNumber to current level before creating door
                if(levelNumber <= currentLevel)
                {
                    //regular door
                    doors[i] = Door(id, position, next, levelNumber);
                }
                else
                {
                    //locked door
                    doors[i] = Door(id, position, next, levelNumber, true);

                }
                if (idOfPrevious >= 0 && idOfPrevious == levelNumber)
                {
                    playerStart = position;
                }
            }
        }
    }
    map.close();

    std::string hubDirectory = backgroundDirectory + "hub.png";
    ResourceManager::LoadTexture(hubDirectory.c_str(), "bg");

    player.setSprite(playerStart);

    levelWidth = xTiles * TileManager::TILE_WIDTH;
    levelHeight = yTiles * TileManager::TILE_HEIGHT;
    camera = Camera(800, 600, levelWidth, levelHeight);

    camera.setPosition(player.sprite.getPosition());
    camera.update();

    levelExit = false;
}

void returnToHub()
{
    if(nextLevel == LAST_LEVEL - 1)
    {
        if(player.collectedGems == NUMBER_OF_GEMS)
        {
            //open last door
            nextLevel ++;
        }
    }
    clear();
    clearPickups();
    player.setUp();
    loadHub(hub, currentLevel, nextLevel);
}

void setPickups(int numberOfPickups, std::ifstream& map, std::string mapName)
{
    if(spawnedPickups.count(mapName) == 0)
    {
        activePickups.resize(numberOfPickups);
        for (int i = 0; i < activePickups.size(); i++)
        {
            glm::vec2 position;
            int type;

            map >> type >> position.x >> position.y;

            if (type == LIVES_INDEX)
            {
                activePickups[i] = new PickupLives(position, type, soundEffects[PICKUP_SOUNDS_INDEX][2]);
            }
            else if (type == HEALTH_INDEX)
            {
                activePickups[i] = new PickupHealth(1, position, type, soundEffects[PICKUP_SOUNDS_INDEX][1]);
            }
            else if(type >= STARS_START_INDEX && type <= STARS_END_INDEX)
            {
                int amount = 0;
                if (type == STARS_START_INDEX)
                {
                    amount = 1;
                }
                else if(type < STARS_END_INDEX)
                {
                    amount = 5;
                }
                else
                {
                    amount = 10;
                }
                activePickups[i] = new PickupStars(amount, position, type, soundEffects[PICKUP_SOUNDS_INDEX][0]);
            }
            else
            {
                //check if gem has already been collected
                //if it has not, spawn gem
                if(collectedGems.count(mapName) == 0)
                {
                    activePickups[i] = new PickupGem(position, type, soundEffects[PICKUP_SOUNDS_INDEX][3]);
                }
                //if it it has, spawn a large star
                else
                {
                    int amount = 10;
                    type = STARS_END_INDEX;
                    activePickups[i] = new PickupStars(amount, position, type, soundEffects[PICKUP_SOUNDS_INDEX][0]);

                }
            }
            activePickups[i]->sprite.uv = &pickupUVs[type];
        }
        spawnedPickups[mapName] = activePickups;
        collectedPickups.reserve(activePickups.size());
    }
    else
    {
        std::vector<Pickup*>& thePickups = spawnedPickups[mapName];
        for(int i = 0; i < thePickups.size(); i ++)
        {
            activePickups.reserve(thePickups.size());
            collectedPickups.reserve(thePickups.size());

            if(thePickups[i]->sprite.active)
            {
                activePickups.push_back(thePickups[i]);
            }
            else
            {
                collectedPickups.push_back(thePickups[i]);
            }
        }
    }
}

void setTargets(int numberOfTargets, std::ifstream& map)
{
    targets.reserve(numberOfTargets);
    for (int i = 0; i < numberOfTargets; i++)
    {
        glm::vec2 position;
        int type;

        map >> type >> position.x >> position.y;

        Pickup *pickup = nullptr;

        if (type == LIVES_INDEX)
        {
            pickup = new PickupLives(position, type, soundEffects[PICKUP_SOUNDS_INDEX][2]);
        }
        else if (type == HEALTH_INDEX )
        {
            pickup = new PickupHealth(1, position, type, soundEffects[PICKUP_SOUNDS_INDEX][1]);
        }
        else
        {
            int amount = 0;
            if (type == STARS_START_INDEX)
            {
                amount = 1;
            }
            else if(type < STARS_END_INDEX)
            {
                amount = 5;
            }
            else
            {
                amount = 10;
            }
            pickup = new PickupStars(amount, position, type, soundEffects[PICKUP_SOUNDS_INDEX][0]);
        }

        int pathLength;
        map >> pathLength;
        std::vector<glm::vec2> path(pathLength);
        path[0] = position;
        for (int i = 1; i < pathLength; i++)
        {
            map >> path[i].x >> path[i].y;
        }
        targets.emplace_back(position, pickup, path);
        targets[i].sprite.uv = &targetUVs;
    }
}

void newExplosion(glm::vec2 position, glm::vec2 size, glm::vec2 originalSize)
{
    glm::vec2 p = position;

    if(size.x != originalSize.x)
    {
        p.x = (position.x + originalSize.x * 0.5f) - size.x *0.5f;
    }

    if(size.y != originalSize.y)
    {
        p.y = (position.y + originalSize.y * 0.5f) - size.y *0.5f;
    }
    //Explosion explosion(p, size, glm::vec4(0,0,32,32), pickupUVs[4]);
    Explosion explosion(p, size, glm::vec4(0,0,32,32), smallExplosionUV);

    explosions.push_back(explosion);
}


void setEnemies(int numberOfEnemies, std::ifstream & map)
{
    inactiveEnemies.resize(numberOfEnemies);

    std::stringstream levelEnemies;
    levelEnemies << numberOfEnemies << " ";

    for (int i = 0; i < inactiveEnemies.size(); i++)
    {
        glm::vec2 position;
        int type;
        int facing;

        map >> type >> position.x >> position.y >> facing;

        levelEnemies << type << " " << position.x << " " << position.y << " " << facing << " ";

        if (type == WALKER)
        {
            inactiveEnemies[i] = new Walker(position, facing);
        }
        else if (type == CHARGER )
        {
            inactiveEnemies[i] = new ChargeEnemy(position, facing, false, player.sprite, soundEffects[CHARGER_SOUNDS_INDEX][0]);
        }
        else if(type == ARMOR_CHARGER)
        {
            inactiveEnemies[i] = new ChargeEnemy(position, facing, true, player.sprite, soundEffects[CHARGER_SOUNDS_INDEX][0]);
        }
        else if (type == SHOOTER)
        {
            inactiveEnemies[i] = new ProjectileEnemy(position, facing, false, &projectiles, camera, player.sprite, soundEffects[PROJECTILE_SOUNDS_INDEX][0]);
        }
        else if( type == ARMOR_SHOOTER)
        {
            inactiveEnemies[i] = new ProjectileEnemy(position, facing, true, &projectiles, camera, player.sprite, soundEffects[PROJECTILE_SOUNDS_INDEX][0]);
        }
        else if (type == FLY_CHASER)
        {
            inactiveEnemies[i] = new FlyChaser(position, facing, player.sprite, soundEffects[BOMB_SOUNDS_INDEX][0]);
        }
        else
        {
            int pathLength;
            map >> pathLength;
            std::vector<glm::vec2> path(pathLength);
            path[0] = position;
            for (int i = 1; i < pathLength; i++)
            {
                map >> path[i].x >> path[i].y;
            }
            if(type == FLY_SHOOTER)
            {
                inactiveEnemies[i] = new FlyShooter(position, facing, &projectiles, path, player.sprite, soundEffects[PROJECTILE_SOUNDS_INDEX][1]);
            }

            levelEnemies << pathLength<< " ";

            for(int i = 1; i < pathLength; i ++)
            {
                levelEnemies << path[i].x << " " << path[i].y << " ";
            }
        }
        //Obviously temp, will be done up earlier
        //will use the type as the index
        if (type == FLY_CHASER)
        {
            inactiveEnemies[i]->sprite.uv = &enemyUVs[FLY_CHASER];
        }
        else if(type == SHOOTER)
        {
            inactiveEnemies[i]->sprite.uv = &enemyUVs[SHOOTER];
        }
        else if(type == ARMOR_SHOOTER)
        {
            inactiveEnemies[i]->sprite.uv = &enemyUVs[ARMOR_SHOOTER];
        }
        else if(type == FLY_SHOOTER)
        {
            inactiveEnemies[i]->sprite.uv = &enemyUVs[FLY_SHOOTER];
        }
        else if(type == WALKER)
        {
            inactiveEnemies[i]->sprite.uv = &enemyUVs[WALKER];
        }
        else if(type == CHARGER)
        {
            inactiveEnemies[i]->sprite.uv = &enemyUVs[CHARGER];
        }
        else if(type == ARMOR_CHARGER)
        {
            inactiveEnemies[i]->sprite.uv = &enemyUVs[ARMOR_CHARGER];
        }
    }
    activeEnemiesString = levelEnemies.str();

    activeEnemies.reserve(inactiveEnemies.size());
}

void setMovables(int numberOfMovables, std::ifstream& map)
{
    movingObjects.resize(numberOfMovables);

    for (int i = 0; i < movingObjects.size(); i++)
    {
        glm::vec2 position;
        int type;
        int width;
        float speed;

        map >> type >> position.x >> position.y >> speed >> width;

        int pathLength;
        map >> pathLength;
        std::vector<glm::vec2> path(pathLength);
        path[0] = position;
        for (int i = 1; i < pathLength; i++)
        {
            map >> path[i].x >> path[i].y;
        }
        movingObjects[i] = new Block(type, position, speed, width, path);
        movingObjects[i]->sprite.uv = &movingObjectUVs;
    }
}


void exitMap()
{
    levelExit = true;

    state = GameState::MAP_TRANSITION;

    transitionTimer = 0.0f;
}

void newMap(Door & door)
{
    std::string mapName = door.nextMap;
    int id = door.ID;

    clear();

    //Since there is no way to go back, don't need to worry about pickups respawning,
    //so we can clear them here.
    if(id < 0)
    {
        clearPickups();
    }

    player.setUp();

    loadMap(mapName, id);
}

void Update(float deltaTime)
{
    switch(state)
    {
    case PLAYING:

        updateActiveEnemies(deltaTime);

        for(int i = 0; i < targets.size(); i ++)
        {
            targets[i].update(deltaTime);
        }

        for(int i = 0; i < movingObjects.size(); i ++)
        {
            movingObjects[i]->update(deltaTime);
        }

        checkEnemySpawn();

        playerUpdate(deltaTime);

        player.checkEnemies(activeEnemies);

        player.checkBlocks(movingObjects);

        player.checkPickups(activePickups);

        updateArrows(deltaTime);

        for (int i = 0; i < projectiles.size(); i++)
        {
            if (projectiles[i].sprite.active)
            {
                projectiles[i].update(deltaTime);
                if(camera.onScreen(projectiles[i].sprite.getPosition(), projectiles[i].sprite.getSize(), glm::vec2(0, 0)))
                {
                    projectiles[i].checkMovables(movingObjects);
                }
                else
                {
                    projectiles[i].sprite.active = false;
                }
            }
        }

        player.checkProjectiles(projectiles);

        clearDeadEnemies();

        for(int i = 0; i < explosions.size(); i ++)
        {
            //update
            explosions[i].update(deltaTime);
            if(!explosions[i].sprite.active)
            {
                explosions[i] = explosions.back();
                explosions.pop_back();
                i--;
            }
        }

        if(!player.sprite.active)
        {
            shake = true;
            state = GameState::DEAD;
            Mix_HaltMusic();
            Mix_PlayChannel(-1, soundEffects[PLAYER_SOUNDS_INDEX][player.TAKE_HIT_SOUND], 0);
        }
        else
        {
            //no reason to go through this if dead
            //handle inactive objects
            for (int i = 0; i < activePickups.size(); i++)
            {
                if (!activePickups[i]->sprite.active)
                {
                    //If we picked up a gem, replace it with a large star
                    //This is important if the player dies and the pickups need to be reset
                    if(activePickups[i]->type == GEM_INDEX)
                    {
                        collectedGems[mapName] = true;
                        int amount = 10;
                        int type = STARS_END_INDEX;
                        glm::vec2 position = activePickups[i]->sprite.getPosition();
                        Pickup* replacementPickup = new PickupStars(amount, position, type, soundEffects[PICKUP_SOUNDS_INDEX][0]);
                        replacementPickup->sprite.uv = &pickupUVs[type];
                        collectedPickups.push_back(replacementPickup);
                    }
                    else
                    {
                        collectedPickups.push_back(activePickups[i]);
                    }
                    activePickups[i] = activePickups.back();
                    activePickups.pop_back();
                    i--;
                }
            }
        }
        //Separate loop to animate pickups, so the i-- at the end of the previous loop can't mess anything up
        //Want to update even off screen pickups, so the animations stay in synch
        for (int i = 0; i < activePickups.size(); i++)
        {
            //animate pickups
            activePickups[i]->animate(deltaTime);
        }

        player.lateUpdate();

        camera.setPosition(player.sprite.getPosition());
        camera.update();
        break;

    case MAP_TRANSITION:
        if(levelExit)
        {
            transitionAlpha = glm::mix(0.0f, 1.0f, transitionTimer);

            if(transitionTimer >= 1)
            {
                newMap(*exitDoor);
                transitionTimer = 0.0f;
                levelExit = false;
            }
        }
        else
        {
            transitionAlpha = glm::mix(1.0f, 0.0f, transitionTimer);

            if(transitionTimer >= 1)
            {
                transitionTimer = 0.0f;

                state = PLAYING;
            }
        }
        transitionTimer += deltaTime / mapTransitionTime;
        break;

    case LEVEL_COMPLETE:
        if(!Mix_PlayingMusic())
        {

            //hold here while playing level complete music
            //begin transition after
            //might even make the transition its own state
            transitionAlpha = glm::mix(0.0f, 1.0f, transitionTimer);

            if(transitionTimer >= 1)
            {
                completeLevel();
                transitionTimer = 0.0f;
                levelExit = false;
            }
            else
            {
                transitionTimer += deltaTime / mapTransitionTime;
            }
            std::cout << "level: " <<  transitionAlpha << "\n";
        }
        break;

    case RETURN_TO_HUB:
        transitionAlpha = glm::mix(1.0f, 0.0f, transitionTimer);

        if(transitionTimer >= 1)
        {
            state = PLAYING;
            Mix_PlayMusic(bgm, -1);
            transitionTimer = 0.0f;
        }
        else
        {
            transitionTimer += deltaTime / mapTransitionTime;
        }
        std::cout << "hub: " <<  transitionAlpha << "\n";

        break;

    case DEAD:
        //Hopefully set this up better, but the idea is I want a shake or some sort of visual effect, a short pause, and then the animation
        deathTimer += deltaTime;
        if(deathTimer >= deathTransitionTime)
        {
            shake = false;
            if(deathTimer >= deathTransitionTime * 2)
            {
                if(player.deathAnimation(deltaTime))
                {
                    player.die();
                    if(!player.gameOver)
                    {
                        restartLevel();
                        state = PLAYING;
                        shake = false;
                        Mix_PlayMusic(bgm, -1);
                    }
                    else
                    {
                        Mix_PlayMusic(gameOverTheme, 1);
                        clear();
                        clearPickups();
                        shake = false;

                        state = GAME_OVER;
                    }
                    deathTimer = 0.0f;
                }
            }
        }

        break;

    case GEM_MESSAGE:
        std::cout << "complete: " <<  transitionAlpha << "\n";
        if(inputManager.isKeyDown(SDLK_RETURN))
        {
            currentLevel = -1;
            loadHub(hub, currentLevel, nextLevel);
            player.setUp();
            state = RETURN_TO_HUB;
        }
        break;
    }
}

void playerUpdate(float deltaTime)
{
    player.update(deltaTime);

    if (inputManager.isKeyPressed(SDLK_UP))
    {
        if(!player.charging && player.physics->state != Physics::IN_AIR)
        {
            Sprite playerSprite = player.sprite;
            glm::vec4 hb = playerSprite.getHitBox();
            for (int i = 0; i < doors.size(); i++)
            {
                glm::vec4 doorHB = doors[i].sprite.getHitBox();
                if (!doors[i].locked && playerSprite.collide(hb, doorHB))
                {
                    exitMap();
                    exitDoor = &doors[i];
                    //if this is the hub
                    //set current level to exit door's level
                    if(exitDoor->level > 0)
                    {
                        currentLevel = exitDoor->level;
                    }
                    break;
                }
            }
        }
    }
}

void updateActiveEnemies(float deltaTime)
{
    for (int i = 0; i < activeEnemies.size(); i++)
    {
        GameObject *currentEnemy = activeEnemies[i];
        if(camera.onScreen(currentEnemy->sprite.getPosition(), currentEnemy->sprite.getSize(), currentEnemy->despawnArea))
        {
            if(!currentEnemy->health.dying)
            {
                currentEnemy->update(deltaTime);
            }
            else
            {
                currentEnemy->handleDeath(deltaTime);
                if(currentEnemy->physics && currentEnemy->physics->state == Physics::IN_AIR )
                {
                    currentEnemy->physics->update(currentEnemy->sprite, deltaTime);
                }
            }
        }
        else
        {
            removeEnemy(i);
            i--;
        }
    }
}

void updateArrows(float deltaTime)
{
    for (int i = 0; i < arrows.size(); i++)
    {
        if (arrows[i].sprite.active)
        {
            arrows[i].update(deltaTime);
            if(camera.onScreen(arrows[i].sprite.getPosition(), arrows[i].sprite.getSize(), glm::vec2(0, 0)))
            {
                arrows[i].checkObjects(activeEnemies, movingObjects);

                for(int c = 0; c < targets.size(); c ++)
                {
                    if(targets[c].sprite.active && arrows[i].sprite.collide(arrows[i].sprite.getHitBox(), targets[c].sprite.getHitBox()))
                    {
                        targets[c].pickup->apply(player);
                        targets[c].sprite.active = false;
                        state = LEVEL_COMPLETE;
                        Mix_PlayMusic(levelCompleteTheme, 1);
                        break;
                    }
                }
            }
            else
            {
                arrows[i].sprite.active = false;
                arrows[i].sprite.setParent(nullptr);
            }
        }
    }
}

void clearDeadEnemies()
{
    for (int i = 0; i < activeEnemies.size(); i++)
    {
        //clear out
        if(activeEnemies[i]->health.dead)
        {
            //Still not ideal, but better
            //This is for the flychasers, as they have a unique explosion
            if(!activeEnemies[i]->health.special)
            {
                //play enemy death sound
                Mix_PlayChannel(-1, soundEffects[ENEMY_DEATH_SOUNDS_INDEX][0], 0);
                newExplosion(activeEnemies[i]->sprite.getPosition(), glm::vec2(32, 32), activeEnemies[i]->sprite.getSize());
            }
            removeEnemy(i);
            i--;
        }
    }
}


void spawnAllOnScreenEnemies()
{
    for(int i = 0; i < inactiveEnemies.size(); i ++)
    {
        if(camera.onScreen(inactiveEnemies[i]->sprite.getPosition(), inactiveEnemies[i]->sprite.getSize(), glm::vec2(128, 128)))
        {
            inactiveEnemies[i]->spawnArea.entered = true;
            inactiveEnemies[i]->reset();
            activeEnemies.push_back(inactiveEnemies[i]);
            inactiveEnemies[i] = inactiveEnemies.back();
            inactiveEnemies.pop_back();
            i --;
        }
    }
}

void checkEnemySpawn()
{
    for (int i = 0; i < inactiveEnemies.size(); i++)
    {
        SpawnArea* sp = &inactiveEnemies[i]->spawnArea;

        if(!sp->entered && camera.onScreen(glm::vec2(sp->p.x, sp->p.y), inactiveEnemies[i]->sprite.getSize(), glm::vec2(96, 96)))
        {
            sp->entered = true;
            inactiveEnemies[i]->reset();
            inactiveEnemies[i]->sprite.active = true;
            activeEnemies.push_back(inactiveEnemies[i]);
            inactiveEnemies[i] = inactiveEnemies.back();
            inactiveEnemies.pop_back();
            i --;
        }
        else if(!camera.onScreen(glm::vec2(sp->p.x, sp->p.y), glm::vec2(32, 32), glm::vec2(96, 96)))
        {
            sp->entered = false;
        }
    }
}

void drawBackground()
{
    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", camera.getCameraMatrix());
    Texture2D texture = ResourceManager::GetTexture("bg");
    Renderer->setUVs();

    float halfWidth = SCREEN_WIDTH * 0.5f;
    float halfHeight = SCREEN_HEIGHT * 0.5f;
    float progress = camera.getPosition().x - halfWidth;

    int endPosition = levelWidth - SCREEN_WIDTH;
    if(endPosition > 0)
    {
        progress /= (endPosition);
    }
    else
    {
        progress = 1.0f;
    }

    int textureOffset = SCREEN_WIDTH - texture.Width;
    Renderer->DrawSprite(texture, glm::vec2((camera.getPosition().x - halfWidth) + (textureOffset * progress), camera.getPosition().y - halfHeight), 0, glm::vec2(texture.Width, texture.Height));
}

void Draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    switch (state)
    {
    case PLAYING:
    case MAP_TRANSITION:
    case DEAD:
    case DEATH_TRANSITION:
    case LEVEL_COMPLETE:
    case RETURN_TO_HUB:
    {
        glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawBackground();

        ResourceManager::GetShader("shape").Use();

        ResourceManager::GetShader("shape").SetMatrix4("projection", camera.getCameraMatrix());

        ResourceManager::GetShader("shape").SetFloat("alpha", 1.0);

        TileManager::tileManager.showBackground(Renderer, camera);

        DrawDoors();

        DrawPickups();

        if(player.sprite.active)
        {
            //player body
            player.drawBody(Renderer);
        }

        DrawArrows();

        DrawProjectiles();

        TileManager::tileManager.showForeground(Renderer, camera);

        //Want the death animation to be drawn in front of the foreground
        if(!player.sprite.active)
        {
            //player body
            player.drawBody(Renderer);
        }
        player.drawBow(Renderer);

        ResourceManager::GetShader("slice").Use().SetMatrix4("projection", camera.getCameraMatrix());

        DrawMovingObjects();

        ResourceManager::GetShader("sprite").Use();

        DrawEnemies();

        DrawExplosions();

        DrawTargets();

        DrawUI();

        HandlePostProcessing();
    }
    break;

    case TITLE_SCREEN:
    {
        DrawTitleScreen();

        break;
    }
    case PAUSED:
    {
        DrawPauseMenu();

        break;
    }
    case PAUSED_FROM_HUB:
    {
        DrawPauseMenuInHub();

        break;
    }
    case GAME_OVER:
        DrawGameOverMenu();
        break;

    case GEM_MESSAGE:
    {
        glClearColor(1.0f, 0.76f, 0.69f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        DrawGemMenu();
        break;
    }

    case GAME_COMPLETE:
        glClearColor(1.0f, 0.76f, 0.69f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        DrawGameCompleteMenu();
        break;
    }
    SDL_GL_SwapWindow(window);
}

void DrawGameCompleteMenu()
{
    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", camera.getOrthoMatrix());

    Texture2D texture = ResourceManager::GetTexture("sprites");

    glm::vec2 position;
    for(int i = 0; i < 4; i ++)
    {
        position = glm::vec2(384 + cos(glm::radians(i * 90.0f + gemRotation)) * 90, 284 + sin(glm::radians(i * 90.0f + gemRotation)) * 90);

        Renderer->setUVs(gemSprite.getUV());
        Renderer->DrawSprite(texture, position, 0.0f, glm::vec2(32.0f, 32.0f));
    }

    position = glm::vec2(384, 284.0f);

    Renderer->setUVs((*player.sprite.uv)[player.IDLE_RIGHT]);
    Renderer->DrawSprite(texture, position, 0.0f, glm::vec2(32.0f, 32.0f));

    //can probably call this once near the start
    ResourceManager::GetShader("slice").Use().SetMatrix4("projection", camera.getOrthoMatrix());
    Texture2D border = ResourceManager::GetTexture("sprites");
    //different menu index
    switch (menuIndex)
    {
    case 0:
        drawButton(border, 78, 35, glm::vec2(319.0f, 417.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        drawButton(border, 78, 35, glm::vec2(419.0f, 417.0f), glm::vec3(1.0f));
        break;
    case 1:
        drawButton(border, 78, 35, glm::vec2(319.0f, 417.0f), glm::vec3(1.0f));
        drawButton(border, 78, 35, glm::vec2(419.0f, 417.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    }

    Text->RenderText("YOU WIN", 366, 100, 1);
    Text->RenderText("THANKS FOR PLAYING", 319, 130, 1);

    Text->RenderText("Continue", 325, 424, 1.0f, glm::vec3(0.0f));
    Text->RenderText("Quit", 443, 424, 1.0f, glm::vec3(0.0f));
}

void DrawGemMenu()
{
    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", camera.getOrthoMatrix());

    Texture2D texture = ResourceManager::GetTexture("sprites");
    int remainingGems = NUMBER_OF_GEMS - player.collectedGems;

    for(int i = 0; i < remainingGems; i ++)
    {
        glm::vec2 position = glm::vec2(384, i * 40 + 340);

        Renderer->setUVs(pickupUVs[GEM_INDEX][4]);
        Renderer->DrawSprite(texture, position, 0.0f, glm::vec2(32.0f, 32.0f));
    }

    std::string message = intToString(remainingGems) + " gems remaining";
    Text->RenderText(message, 333, 300, 1);
}

void DrawGameOverMenu()
{
    ResourceManager::GetShader("slice").Use().SetMatrix4("projection", camera.getOrthoMatrix());
    Texture2D border = ResourceManager::GetTexture("sprites");
    switch (menuIndex)
    {
    case 0:
        drawButton(border, 112, 35, glm::vec2(284.0f, 390.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        drawButton(border, 78, 35, glm::vec2(438.0f, 390.0f), glm::vec3(1.0f));
        break;
    case 1:
        drawButton(border, 112, 35, glm::vec2(284.0f, 390.0f), glm::vec3(1.0f));
        drawButton(border, 78, 35, glm::vec2(438.0f, 390.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    }
    Text->RenderText("GAME OVER", 360, 300, 1);
    Text->RenderText("Try again", 305, 397, 1.0f, glm::vec3(0.0f));
    Text->RenderText("Quit", 462, 397, 1.0f, glm::vec3(0.0f));

}

void DrawPauseMenuInHub()
{
    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", camera.getOrthoMatrix());

    Texture2D texture = ResourceManager::GetTexture("PauseScreen");

    Renderer->setUVs();
    Renderer->DrawSprite(texture, glm::vec2(0.0f), 0.0f, glm::vec2(800.0f, 600.0f));

    ResourceManager::GetShader("slice").Use().SetMatrix4("projection", camera.getOrthoMatrix());
    Texture2D border = ResourceManager::GetTexture("sprites");

    switch (menuIndex)
    {
    case PAUSE_MENU_FROM_HUB_UNPAUSE:
    {
        drawButton(border, 78, 35, glm::vec2(324.0f, 410.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        drawButton(border, 43, 35, glm::vec2(424.0f, 410.0f), glm::vec3(1.0f));
        break;

    }
    case PAUSE_MENU_FROM_HUB_QUIT:
    {
        drawButton(border, 78, 35, glm::vec2(324.0f, 410.0f), glm::vec3(1.0f));
        drawButton(border, 43, 35, glm::vec2(424.0f, 410.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    }
    }
    Text->RenderText("Unpause", 330, 417, 1.0f, glm::vec3(0.0f));
    Text->RenderText("Quit", 430, 417, 1.0f, glm::vec3(0.0f));
}

void DrawPauseMenu()
{
    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", camera.getOrthoMatrix());

    Texture2D pTexture = ResourceManager::GetTexture("PauseScreen");

    Renderer->setUVs();
    Renderer->DrawSprite(pTexture, glm::vec2(0.0f), 0.0f, glm::vec2(800.0f, 600.0f));

    ResourceManager::GetShader("slice").Use().SetMatrix4("projection", camera.getOrthoMatrix());
    Texture2D border = ResourceManager::GetTexture("sprites");

    glm::vec3 returnToHubColor;
    if (player.currentLives > 0)
    {
        returnToHubColor = glm::vec3(1.0f);
    }
    else
    {
        returnToHubColor = glm::vec3(0.1f);
    }
    switch (menuIndex)
    {
    case PAUSE_MENU_UNPAUSE:
    {
        drawButton(border, 78, 35, glm::vec2(244.0f, 410.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        drawButton(border, 119, 35, glm::vec2(344.0f, 410.0f), returnToHubColor);
        drawButton(border, 43, 35, glm::vec2(485.0f, 410.0f), glm::vec3(1.0f));
        break;

    }
    case PAUSE_MENU_HUB:
    {
        drawButton(border, 78, 35, glm::vec2(244.0f, 410.0f), glm::vec3(1.0f));
        drawButton(border, 119, 35, glm::vec2(344.0f, 410.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        drawButton(border, 43, 35, glm::vec2(485.0f, 410.0f), glm::vec3(1.0f));
        break;
    }
    case PAUSE_MENU_QUIT:
    {
        drawButton(border, 78, 35, glm::vec2(244.0f, 410.0f), glm::vec3(1.0f));
        drawButton(border, 119, 35, glm::vec2(344.0f, 410.0f), returnToHubColor);
        drawButton(border, 43, 35, glm::vec2(485.0f, 410.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    }
    }
    Text->RenderText("Unpause", 250, 417, 1.0f, glm::vec3(0.0f));
    Text->RenderText("Return to Hub", 350, 417, 1.0f, glm::vec3(0.0f));
    Text->RenderText("Quit", 491, 417, 1.0f, glm::vec3(0.0f));
}

void DrawTitleScreen()
{
    drawBackground();
    //   TileManager::tileManager.showBackground(Renderer, camera);
    TileManager::tileManager.showForeground(Renderer, camera);

    DrawDoors();

    ResourceManager::GetShader("slice").Use().SetMatrix4("projection", camera.getOrthoMatrix());
    Texture2D border = ResourceManager::GetTexture("sprites");
    switch (menuIndex)
    {
    case 0:
        drawButton(border, 78, 35, glm::vec2(304.0f, 390.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        drawButton(border, 78, 35, glm::vec2(419.0f, 390.0f), glm::vec3(1.0f));
        break;
    case 1:
        drawButton(border, 78, 35, glm::vec2(304.0f, 390.0f), glm::vec3(1.0f));
        drawButton(border, 78, 35, glm::vec2(419.0f, 390.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    }
    Text->RenderText("Archer Game", 304, 290, 2);
    Text->RenderText("Play", 325, 397, 1.0f, glm::vec3(0.0f));
    Text->RenderText("Quit", 443, 397, 1.0f, glm::vec3(0.0f));
}

void DrawUI()
{
    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", camera.getOrthoMatrix());
    glm::vec2 position;

    Texture2D texture = ResourceManager::GetTexture("sprites");

    for(int i = 0; i < player.health.currentHealth; i ++)
    {
        position = glm::vec2(i * 40 + 32, 568.0f);

        Renderer->setUVs(pickupUVs[HEALTH_INDEX][0]);
        Renderer->DrawSprite(texture, position, 0.0f, glm::vec2(32.0f, 32.0f));
    }

    position = glm::vec2(192, 568.0f);

    Renderer->setUVs((*player.sprite.uv)[player.IDLE_RIGHT]);
    Renderer->DrawSprite(texture, position, 0.0f, glm::vec2(32.0f, 32.0f));
    Text->RenderText("X " + intToString(player.currentLives), 227, 573, 1);

    position = glm::vec2(310, 568.0f);

    Renderer->setUVs(pickupUVs[STARS_START_INDEX][0]);
    Renderer->DrawSprite(texture, position, 0.0f, glm::vec2(32.0f, 32.0f));
    Text->RenderText("X " + intToString(player.currentStars), 345, 573, 1);
}

void HandlePostProcessing()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer

    ResourceManager::GetShader("postprocessing").Use();
    ResourceManager::GetShader("postprocessing").SetFloat("time", SDL_GetTicks() / 1000.0f);
    ResourceManager::GetShader("postprocessing").SetInteger("shake", shake);
    ResourceManager::GetShader("postprocessing").SetFloat("alpha", transitionAlpha);

    // Render textured quad
    glActiveTexture(GL_TEXTURE0);
    Texture.Bind();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void DrawTargets()
{
    for (int i = 0; i < targets.size(); i++)
    {
        targets[i].draw(Renderer);
    }
}

void DrawExplosions()
{
    for (int i = 0; i < explosions.size(); i++)
    {
        Texture2D texture = ResourceManager::GetTexture("sprites");
        Renderer->setUVs(explosions[i].sprite.getUV());
        Renderer->DrawSprite(texture, explosions[i].sprite.getPosition(), 0.0f, explosions[i].sprite.getSize());
    }
}

void DrawEnemies()
{
    for (int i = 0; i < activeEnemies.size(); i++)
    {
        glm::vec2 position = activeEnemies[i]->sprite.getPosition();

        Renderer->setUVs(activeEnemies[i]->sprite.getUV());
        Texture2D texture = ResourceManager::GetTexture("sprites");

        Renderer->DrawSprite(texture, position, 0.0f, activeEnemies[i]->sprite.getSize());

        /* for(int c = 0; c < activeEnemies[i]->colliders.size(); c++)
         {
             glm::mat4 model = glm::mat4();

             glm::vec4 adfwea = activeEnemies[i]->colliders[c].dimensions;
             glm::vec4 hb = activeEnemies[i]->colliders[c].dimensions + glm::vec4(position.x, position.y, 0, 0);

             model = glm::translate(model, glm::vec3(hb.x, hb.y, 0.0f));

             model = glm::scale(model, glm::vec3(hb.z, hb.w, 0.0f));

             ResourceManager::GetShader("shape").Use().SetVector3f("shapeColor", glm::vec3(0.0f, 1.0f / (c + 1), 0.0f));

             ResourceManager::GetShader("shape").Use().SetMatrix4("model", model);
             glBindVertexArray(shapeVAO);
             glDrawArrays(GL_TRIANGLES, 0, 6);
             glBindVertexArray(0);
         }*/
    }


}

void DrawMovingObjects()
{
    for (int i = 0; i < movingObjects.size(); i++)
    {
        glm::vec2 position = movingObjects[i]->sprite.getPosition();
        glm::vec2 size = movingObjects[i]->sprite.getSize();
        //would be ideal if I could get around this check. want to update while off screen but not draw.
        if (camera.onScreen(position, size))
        {
            glm::vec4 uvs = movingObjects[i]->sprite.getUV();

            ResourceManager::GetShader("slice").SetVector2f("u_dimensions", 8.0f / size.x, 8.0f / size.y);
            ResourceManager::GetShader("slice").SetVector2f("u_border", 8.0f / 32.0f, 8.0f / 32.0f);
            ResourceManager::GetShader("slice").SetVector4f("bounds", uvs.x, uvs.y, uvs.z, uvs.w);

            Texture2D texture = ResourceManager::GetTexture("sprites");
            outlineRenderer->DrawSprite(texture, position, 0.0f, size);
        }
    }
}

void DrawArrows()
{
    //  ResourceManager::GetShader("shape").Use();
    glm::mat4 model;

    model = glm::mat4();
    for (int i = 0; i < arrows.size(); i++)
    {
        if (arrows[i].sprite.active)
        {
            //Because of how the arrow sprite is drawn, this allows the shading to appear on the correct side regardless of if the arrow is being fired left or right.
            if(abs(arrows[i].sprite.rotation) < 1.57f)
            {
                Renderer->setUVs(arrowUVs[0]);
            }
            else
            {
                Renderer->setUVs(arrowUVs[1]);
            }
            Texture2D texture = ResourceManager::GetTexture("sprites");
            Renderer->DrawSprite(texture, arrows[i].sprite.getPosition(), arrows[i].sprite.rotation, glm::vec2(32, 8));

            model = glm::mat4();

            glm::vec4 hb = arrows[i].sprite.getHitBox();

            glm::vec2 size = arrows[i].sprite.getSize();

            /*  //model = arrows[i].sprite.getTranslation();
              model = glm::translate(model, glm::vec3(hb.x, hb.y, 0.0f));

              model = glm::translate(model, glm::vec3(hb.z * 0.5f, hb.w * 0.5f, 0.0f)); // Move origin of rotation to center of quad
              model = glm::rotate(model, arrows[i].sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
              model = glm::translate(model, glm::vec3(-hb.z * 0.5f, -hb.w * 0.5f, 0.0f)); // Move origin back

              model = glm::scale(model, glm::vec3(hb.z, hb.w, 0.0f));

              ResourceManager::GetShader("shape").Use().SetVector3f("shapeColor", glm::vec3(0.0f, 1.0f, 0.0f));

              ResourceManager::GetShader("shape").Use().SetMatrix4("model", model);
              glBindVertexArray(shapeVAO);
              glDrawArrays(GL_TRIANGLES, 0, 6);
              glBindVertexArray(0);

              glm::vec4 phb = arrows[i].phb;
              model = glm::mat4();

              model = glm::translate(model, glm::vec3(phb.x, phb.y, 0.0f));

              model = glm::translate(model, glm::vec3(phb.z * 0.5f, phb.w * 0.5f, 0.0f)); // Move origin of rotation to center of quad
              model = glm::rotate(model, arrows[i].sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
              model = glm::translate(model, glm::vec3(-phb.z * 0.5f, -phb.w * 0.5f, 0.0f)); // Move origin back

              model = glm::scale(model, glm::vec3(phb.z, phb.w, 0.0f));

              ResourceManager::GetShader("shape").SetVector3f("shapeColor", glm::vec3(0.0f, 1.0f, 1.0f));

              ResourceManager::GetShader("shape").Use().SetMatrix4("model", model);
              glBindVertexArray(shapeVAO);
              glDrawArrays(GL_TRIANGLES, 0, 6);
              glBindVertexArray(0);*/
        }
    }
}


void DrawProjectiles()
{
    for (int i = 0; i < projectiles.size(); i++)
    {
        if (projectiles[i].sprite.active)
        {
            //Renderer->setUVs(projectiles[i].sprite.getUV());
            Renderer->setUVs(projectileUVs[0][projectiles[i].sprite.currentFrame]);
            Texture2D texture = ResourceManager::GetTexture("sprites");
            Renderer->DrawSprite(texture, projectiles[i].sprite.getPosition(), projectiles[i].sprite.rotation, glm::vec2(32, 16));
        }
    }
}

void DrawPickups()
{
    for (int i = 0; i < activePickups.size(); i++)
    {
        Pickup* pickup = activePickups[i];
        glm::vec4 hb = pickup->sprite.getHitBox();
        if (camera.onScreen(glm::vec2(hb.x, hb.y), glm::vec2(hb.z, hb.w)))
        {
            Renderer->setUVs(pickup->sprite.getUV());
            Texture2D texture = ResourceManager::GetTexture("sprites");
            Renderer->DrawSprite(texture, pickup->sprite.getPosition(), 0.0f, glm::vec2(32, 32));
        }
    }
}

void DrawDoors()
{
    for (int i = 0; i < doors.size(); i++)
    {
        Door* door = &doors[i];

        //only doors in the hub will be locked
        if (door->locked)
        {
            Renderer->setUVs(doorUVs[LOCKED_DOOR_INDEX]);
        }
        else
        {
            Renderer->setUVs(doorUVs[UNLOCKED_DOOR_INDEX]);
        }
        Texture2D texture = ResourceManager::GetTexture("sprites");
        Renderer->DrawSprite(texture, door->sprite.getPosition(), 0.0f, glm::vec2(32, 32));
    }
}

void drawButton(Texture2D& border, int width, int height, glm::vec2 position, glm::vec3 color)
{
    ResourceManager::GetShader("slice").SetVector2f("u_dimensions", 8.0f / width, 8.0f / height);
    ResourceManager::GetShader("slice").SetVector2f("u_border", 8.0f / 32.0f, 8.0f / 32.0f);
    ResourceManager::GetShader("slice").SetVector4f("bounds", buttonUV.x, buttonUV.y, buttonUV.z, buttonUV.w);

    outlineRenderer->DrawSprite(border, position, 0.0f, glm::vec2(width, height), glm::vec4(color.x, color.y, color.z, 1.0f));
}


void completeLevel()
{
    clear();
    clearPickups();
    if(currentLevel == NUMBER_OF_LEVELS)
    {
        if(player.collectedGems == NUMBER_OF_GEMS)
        {
            //open last door
            if(nextLevel < LAST_LEVEL)
            {
                nextLevel ++;
            }
            state = RETURN_TO_HUB;
            player.setUp();
            loadHub(hub, currentLevel, nextLevel);
        }
        else
        {
            state = GEM_MESSAGE;
            //state = gem message
        }
    }
    else if(currentLevel == LAST_LEVEL)
    {
        state = GAME_COMPLETE;
    }
    else
    {
        state = RETURN_TO_HUB;
        if(currentLevel == nextLevel)
        {
            nextLevel ++;
        }
        else if(nextLevel == LAST_LEVEL - 1)
        {
            if(player.collectedGems == NUMBER_OF_GEMS)
            {
                //open last door
                nextLevel ++;
            }
        }
        player.setUp();
        loadHub(hub, currentLevel, nextLevel);
    }
    levelComplete = false;
}

//Called on player death
void restartLevel()
{
    clearProjectiles();

    for(int i = 0; i < collectedPickups.size(); i ++)
    {
        collectedPickups[i]->sprite.active = true;
        activePickups.push_back(collectedPickups[i]);

        collectedPickups[i] = collectedPickups.back();
        collectedPickups.pop_back();
        i--;
    }
    for(auto& iter : spawnedPickups)
    {
        if(iter.first != mapName)
        {
            for(int i = 0; i < iter.second.size(); i++)
            {
                delete iter.second[i];
            }
        }
    }

    spawnedPickups.clear();
    spawnedPickups[mapName] = activePickups;

    //Hopefully prevents pickup animations from getting out of sync...
    for(int i = 0; i < activePickups.size(); i ++)
    {
        activePickups[i]->sprite.currentFrame = 0;
        activePickups[i]->sprite.timeForFrame = 0.0f;
    }

    /*clear();
    clearPickups();

    loadMap(mapName, levelID);*/

    player.setUp();
    player.setSprite(playerStart);

    camera.setPosition(player.sprite.getPosition());
    camera.update();

    for(int i = 0; i < movingObjects.size(); i ++)
    {
        movingObjects[i]->reset();
    }

    for(int i = 0; i < activeEnemies.size(); i ++)
    {
        activeEnemies[i]->spawnArea.entered = false;
        removeEnemy(i);
        i--;
    }

    for(int i = 0; i < targets.size(); i ++)
    {
        targets[i].reset();
    }

    spawnAllOnScreenEnemies();
}

void clear()
{
    TileManager::tileManager.clearTiles();

    clearProjectiles();

    clearEnemies();

    clearMovingObjects();

    activePickups.clear();

    collectedPickups.clear();

    doors.clear();

    targets.clear();

    explosions.clear();

}

void clearEnemies()
{
    for (int i = 0; i < activeEnemies.size(); i++)
    {
        delete activeEnemies[i];
        activeEnemies[i] = nullptr;
    }
    for (int i = 0; i < inactiveEnemies.size(); i++)
    {
        delete inactiveEnemies[i];
        inactiveEnemies[i] = nullptr;
    }
    inactiveEnemies.clear();
    activeEnemies.clear();
}

void removeEnemy(int i)
{
    activeEnemies[i]->sprite.active = false;
    inactiveEnemies.push_back(activeEnemies[i]);
    activeEnemies[i] = activeEnemies.back();
    activeEnemies.pop_back();
}

void clearPickups()
{
    for(auto& iter : spawnedPickups)
    {
        for(int i = 0; i < iter.second.size(); i++)
        {
            delete iter.second[i];
        }
    }
    spawnedPickups.clear();
}


void clearMovingObjects()
{
    for (int i = 0; i < movingObjects.size(); i++)
    {
        delete movingObjects[i];
        movingObjects[i] = nullptr;
    }
    movingObjects.clear();
}

void clearProjectiles()
{
    for (int i = 0; i < arrows.size(); i++)
    {
        arrows[i].sprite.setParent(nullptr);
        arrows[i].sprite.active = false;
    }

    for (int i = 0; i < projectiles.size(); i++)
    {
        projectiles[i].sprite.active = false;
    }
}

void close()
{
    delete Renderer;
    delete Text;

    for (int i = 0; i < soundEffects.size(); i++)
    {
        for (int c = 0; c < soundEffects[i].size(); c++)
        {
            Mix_FreeChunk(soundEffects[i][c]);
            soundEffects[i][c] = nullptr;
        }
    }
    Mix_FreeMusic(bgm);
    Mix_FreeMusic(gameOverTheme);
    Mix_FreeMusic(levelCompleteTheme);
    bgm = nullptr;
    gameOverTheme = nullptr;
    levelCompleteTheme = nullptr;

    clear();
    clearPickups();
    collectedGems.clear();

    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_Quit();
    IMG_Quit();
    Mix_Quit();
    Mix_CloseAudio();
}

void moveMenu(int i)
{
    menuIndex+=i;
    Mix_PlayChannel(-1, soundEffects[MENU_SOUNDS_INDEX][0], 0);
}


Mix_Chunk * loadSound(const std::string& fileName)
{
    Mix_Chunk* sound = Mix_LoadWAV(fileName.c_str());
    if (sound == nullptr)
    {
        printf("Mix_LoadWAV error: %s\n", Mix_GetError());
    }
    return sound;
}

Mix_Music* loadMusic(const std::string& fileName)
{
    Mix_Music* music = Mix_LoadMUS( fileName.c_str() );
    if(music == nullptr)
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    return music;
}

std::string intToString(int i)
{
    stringstream s;
    s << i;
    return s.str();
}
