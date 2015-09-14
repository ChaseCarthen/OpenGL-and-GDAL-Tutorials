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
#include <string>

using namespace std;

#ifndef _PROJECTOR_H_
#define _PROJECTOR_H_
class projector : renderable
{
public:
	enum PROJECTOR_TYPE
	{
		IMAGE,
		DATA
	};

	projector();
	void setup();
	void update(float dt);
	void render(glm::mat4& view, glm::mat4& projection);
	void setScreenDims(int width, int height)
	{
		SCREEN_WIDTH = width;
		SCREEN_HEIGHT = height;
	};

	void setFile(std::string fname,PROJECTOR_TYPE type = IMAGE,int band=1)
	{
		filename = fname;
		projtype = type;
		bandnum = band;
	};

	void setmask(std::string maskname)
	{
		maskfile = maskname;
	};

	void SetPosition(glm::vec3 p)
	{
		position = p;
	};

	void SetDimensions(float width, float height)
	{
		projection = glm::ortho<float>(-width / 2.0f, width / 2.0f, height / 2.0f, -height / 2.0f, 0.1f, 10000.0f);
	};

	void SetPosition(float x, float y)
	{
		position.x = x;
		position.z = y;
		position.y = 100;
	};

	void setToMainCoordinateSystem(OGRSpatialReference* main, glm::vec2 mainorigin)
	{

		// translate to main coordinate system
		OGRCoordinateTransformation* transform;
		if (main != NULL)
		{
			transform = OGRCreateCoordinateTransformation( &sr, main );
		}
		else
		{
			return;
		}
		double x = origin.x;
		double y = origin.y;
		double xright,ybottom;
		xright = x + xres*width;
		ybottom = y - yres*height;


		transform->Transform(1,&x,&y);
		transform->Transform(1,&xright,&ybottom);

		double XRES = (xright-x) / width;
		double YRES = (y - ybottom) / height;

		SetDimensions((xright-x),(y - ybottom));
		x = x - mainorigin.x;
		y = mainorigin.y - y;
		position.x = x;
		position.z = y;

	};

	void setTranslucency(float Alpha)
	{
		alpha = Alpha;
	};

	void incTranslucency(float alph)
	{
		alpha += alph;
		if(alpha >= 1)
		{
			alpha = 1;
		}
		cout << alpha << endl;
	};

	void decTranslucency(float alph)
	{
		alpha -= alph;
		if(alpha <= 0)
		{
			alpha = 0;
		}
		cout << alpha << endl;
	};

private:
	OGRSpatialReference sr;
	int SCREEN_WIDTH, SCREEN_HEIGHT;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 Texgen;
	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 up;
	glm::vec2 origin;
	buffer Buffer;
	GLuint tex,masktex;
	string filename;
	string maskfile;
	int width, height;
	double xres, yres;

	float alpha;

	PROJECTOR_TYPE projtype;
	int bandnum;
};
#endif