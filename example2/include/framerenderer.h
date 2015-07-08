#include <renderable.h>
#include <gbuffer.h>
#include <buffer.h>
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

private:
	buffer Buffer;
};
#endif