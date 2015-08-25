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
	void setup();
	void update(float dt);
	void render(glm::mat4& view, glm::mat4& projection);
	void cleanup();
	void SetFile(string filename);
	float SampleTerrain(glm::vec2 point);
	float SampleTerrain2(glm::vec2 point);
	glm::vec2 GetOrigin()
	{
		return origin;
	};
	OGRSpatialReference* GetProjection()
	{
		return &sr;
	}
private:
	float** terrainData;
	buffer elements;
	buffer terrainpoints;
	string fname, projection;
	float min, max, xres, yres;
	vector<vector<float>> vecs;
	vector<Vertex> vertexes;
	vector<int> indicies;
	OGRSpatialReference sr;
	glm::vec2 origin;
	glm::vec2 end;
	glm::vec2 worldOrigin;
	glm::vec2 worldDims;
	int width,height;
};

#endif
