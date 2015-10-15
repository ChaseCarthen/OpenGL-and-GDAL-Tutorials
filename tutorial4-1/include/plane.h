#include <renderer.h>
#include <renderable.h>
#include <buffer.h>
#include <AssetManager.h>
#include <vector>

using namespace std;

#ifndef _PLANE_H_
#define _PLANE_H_

class plane : renderable
{
public:
	plane();
	void buildPlane(int x, int y, int width, int height);
    void setup();
	void update(float dt);
	void render(glm::mat4& view, glm::mat4& projection);
private:
	buffer Buffer;
	buffer Buffer2;
	vector<float> verts;
	vector<int> inds;
};
#endif