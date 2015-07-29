#include <renderable.h>
#include <gbuffer.h>
#include <buffer.h>
#include <AssetManager.h>

#ifndef _FRAMERENDERER_H_
#define _FRAMERENDERER_H_
class framerenderer : renderable
{
public:
	framerenderer();
		// A nice generic interface function for rendereing things
	void render(glm::mat4& view, glm::mat4& projection);

	// Need to include the mvp matrix here as well -- we need to update the world somehow
	void update(float dt);

	// A function that muse be called after opengl has been initialized!!!!!
	void setup();

	void setScreenDims(int width,int height)
	{
		SCREEN_WIDTH = width;
		SCREEN_HEIGHT = height;
	};

	void SetCameraPos(glm::vec3 pos)
	{
		cameraPos = pos;
	};
	void setHasProj(int val)
	{
		proj = val;
	};
private:
	buffer Buffer;
	int SCREEN_WIDTH,SCREEN_HEIGHT;
	glm::vec3 cameraPos;
	int proj;
};
#endif