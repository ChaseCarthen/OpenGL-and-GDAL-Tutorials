#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl3.h>
#else //linux as default
#include <GL/glew.h>
#endif

#include <buffer.h>
#include <iostream>
#include <renderable.h>
using namespace std; // Should remove this in the future..
// A simple trinagle class to draw a single triangle
#ifndef _TRIANGLE_TEST_H_
#define _TRIANGLE_TEST_H_
class triangle : public renderable
{

public:
	triangle();

	void setup();
	void render(glm::mat4& view, glm::mat4& projection);
	void update(float dt);
	
private:
	float verticies[9];
	buffer Buffer;
};
#endif