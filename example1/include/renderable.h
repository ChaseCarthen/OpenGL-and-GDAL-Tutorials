#include <renderer.h>
#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_
// A pure virtual interface class to make rendering things easierish.
class renderable
{
public:
	//renderable()
	//{};

	// A nice generic interface function for rendereing things
	virtual void render() = 0;

	// Need to include the mvp matrix here as well -- we need to update the world somehow
	virtual void update(float dt) = 0;

	// A function that muse be called after opengl has been initialized!!!!!
	virtual void setup() = 0;

	// May include a lighting phase and blend phase here :D when
	// I reach differered rendering.

protected:

	renderer Renderer;
	// Leaving the buffer undefined to be handle by the person making the class

};
#endif