#include <renderable.h>
#include <buffer.h>
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

// A terrain class for loading DEMs from GIS data with GDAL
class terrain : public renderable
{
public:
	terrain();
	//void SampleTerrain();
	void setup();
	void update(float dt);
	void render();
	
private:
	float** terrainData;
	buffer Buffer;
};

#endif
