#include <renderable.h>
#include <buffer.h>
#include <gishandler.h>
#include <string>
#include <vector>
#include <AssetManager.h>
using namespace std;

#ifndef _TERRAIN_H_
#define _TERRAIN_H_

// A terrain class for loading DEMs from GIS data with GDAL
class terrain : public renderable
{
public:
	terrain();
	terrain(string filename);
	//void SampleTerrain();
	void setup();
	void update(float dt);
	void render(glm::mat4& view, glm::mat4& projection);
	void cleanup();
	void SetFile(string filename);
private:
	float** terrainData;
	buffer elements;
	buffer terrainpoints;
	string fname, projection;
	float min, max, xres, yres;
	vector<vector<float>> vecs;
	vector<Vertex> vertexes;
	vector<int> indicies;
};

#endif
