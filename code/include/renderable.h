#include <renderer.h>
//#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_
// A pure virtual interface class to make rendering things easierish.
class renderable
{
public:

	// A nice generic interface function for rendereing things
	virtual void render(glm::mat4& view, glm::mat4& projection) = 0;

	// Need to include the mvp matrix here as well -- we need to update the world somehow
	virtual void update(float dt) = 0;

	// A function that muse be called after opengl has been initialized!!!!!
	virtual void setup() = 0;

	// May include a lighting phase and blend phase here :D when
	// I reach differered rendering.

protected:

	renderer Renderer;
	// Leaving the buffer undefined to be handle by the person making the class

	// everything needs some type of object space.
	glm::mat4 model;

};
#endif
