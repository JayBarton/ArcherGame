/*
Code modified from https://github.com/Barnold1953/GraphicsTutorials/tree/master/Bengine
*/

#include "Camera.h"
#include <math.h>

Camera::Camera(int screenW, int screenH, int levelW, int levelH) : screenWidth(screenW),
    screenHeight(screenH), levelWidth(levelW), levelHeight(levelH), cameraScale(1.0f), position(0.0f, 0.0f),
    cameraMatrix(1.0f), needsMatrixUpdate(true)
{
    orthographicMatrix = glm::ortho(0.0f , float(screenWidth) ,
                                    float(screenHeight) , 0.0f , -1.0f, 1.0f);
    halfWidth = screenWidth/2;
    halfHeight = screenHeight/2;

	boxX = halfWidth - boxOffset;
	boxY = halfHeight;
	boxW = halfWidth + boxOffset;
	boxH = halfHeight + halfHeight;
	border = glm::ivec4(halfWidth - 8, halfHeight - 100, halfWidth + 8, halfHeight);
}

Camera::~Camera()
{
    //dtor
}

void Camera::update()
{
    if(needsMatrixUpdate)
    {
        if(position.x  - (halfWidth / cameraScale) < 0)
        {
            position.x = (halfWidth / cameraScale) ;
        }
        if(position.x  + (halfWidth / cameraScale) > levelWidth)
        {
            position.x = (levelWidth - halfWidth / cameraScale) ;
        }

        if(position.y  - (halfHeight / cameraScale) < 0)
        {
            position.y = (halfHeight / cameraScale) ;
        }
        if(position.y  + (halfHeight / cameraScale) > levelHeight)
        {
            position.y = (levelHeight - halfHeight / cameraScale) ;
        }


       // glm::vec3 translate(-position.x + halfWidth, -position.y + halfHeight, 0.0f);
       //Round the position to prevent oddities when rendering at decimal positions
        glm::vec3 translate(round(-position.x) + halfWidth, round(-position.y) + halfHeight, 0.0f);

        glm::vec3 scale(cameraScale, cameraScale, 0.0f);

        cameraMatrix = glm::translate(orthographicMatrix, translate);
        cameraMatrix = glm::scale(glm::mat4(1.0f), scale) * cameraMatrix;
        needsMatrixUpdate = false;
    }
}

bool Camera::outSideBox(glm::vec2 p)
{
	bool outSide = false;

	glm::vec2 pScreen = worldToScreen(p);
	if (pScreen.x > boxW || pScreen.x < boxX || pScreen.y >boxH || pScreen.y < boxY)
	{
		outSide = true;
	}

	return outSide;
}

bool Camera::onScreen(glm::vec2 p, glm::vec2 size, glm::vec2 offset /*= glm::vec2(0)*/)
{
    bool on = true;

    glm::vec2 pScreen = worldToScreen(p);

    glm::vec4 screenBounds(0 - offset.x, 0 - offset.y, screenWidth + offset.x, screenHeight + offset.y);

	if (pScreen.x  + size.x < screenBounds.x || pScreen.x > screenBounds.z
     || pScreen.y  > screenBounds.w || pScreen.y + size.y < screenBounds.y)
	{
		on = false;
	}
	return on;
}


glm::vec2 Camera::screenToWorld(glm::vec2 screenCoords)
{
	screenCoords -= glm::vec2(screenWidth / 2.0f, screenHeight / 2.0f);
	//Scale the coordinates
	screenCoords /= cameraScale;
	//Translate with the camera position
	screenCoords += position;

	return screenCoords;
}

glm::vec2 Camera::worldToScreen(glm::vec2 screenCoords)
{

	//invert y
//	screenCoords.y = screenHeight - screenCoords.y;
	//Make it so the zero is the center
	screenCoords += glm::vec2(screenWidth /2.0f, screenHeight/2.0f );
	//Scale the coordinates
	screenCoords /= cameraScale;
	//Translate with the camera position
	screenCoords -= position;

	return screenCoords;

}

void Camera::Follow(glm::vec2 p, float speed, float delta)
{
	glm::vec2 pScreen = worldToScreen(p);
	if (pScreen.x > boxW)
	{
		position.x += speed * delta;
		needsMatrixUpdate = true;
	}
	else if (pScreen.x < boxX)
	{
		position.x -= speed * delta;
		needsMatrixUpdate = true;
	}
	if (pScreen.y > boxH)
	{
		position.y += speed * delta;
		needsMatrixUpdate = true;
	}
	else if (pScreen.y < boxY)
	{
		position.y -= speed * delta;
		needsMatrixUpdate = true;
	}
}

void Camera::Follow(glm::vec2 p)
{
	glm::vec2 pScreen = worldToScreen(p);
	if (pScreen.x > border.z)
	{
		position.x += pScreen.x - border.z;
		needsMatrixUpdate = true;
	}
	else if (pScreen.x < border.x)
	{
		position.x -= border.x - pScreen.x;
		needsMatrixUpdate = true;
	}
	if (pScreen.y > border.w)
	{
		position.y += pScreen.y - border.w;
		needsMatrixUpdate = true;
	}
	else if (pScreen.y < border.y)
	{
		position.y -= border.y - pScreen.y;
		needsMatrixUpdate = true;
	}
}
