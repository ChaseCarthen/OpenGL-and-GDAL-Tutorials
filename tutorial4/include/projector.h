#include <gishandler.h>
#include <vector>
#include <renderer.h>
#include <renderable.h>
#include <buffer.h>
#include <terrain.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <AssetManager.h>
using namespace std;

#ifndef _PROJECTOR_H_
#define _PROJECTOR_H_
class projector : renderable
{
public:
	projector();
	void setup();
	void update(float dt);
	void render(glm::mat4& view, glm::mat4& projection);
	void setScreenDims(int width,int height)
	{
		SCREEN_WIDTH = width;
		SCREEN_HEIGHT = height;
	};

private:
	int SCREEN_WIDTH,SCREEN_HEIGHT;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 Texgen;
	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 up;
	buffer Buffer;
};
#endif