//#include "includes.h"
#include <vector>
#include <string>
#include <iostream>

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl3.h>
#else //linux as default
#include <GL/glew.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glext.h>
#endif

//#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <buffer.h>

// GDAL includes
//gdal
#include <gdal.h>
#include <gdal_priv.h>
//#include <cpl_conv.h>
#include <ogrsf_frmts.h>
//#include <gdal_priv.h>
//#include <gdal_alg.h>
//#include <gdal_alg_priv.h>
//#include <cpl_conv.h>


#ifndef _GISHANDLER_H_
#define _GISHANDLER_H_
using namespace std;
using namespace glm;
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

bool getRawValuesFromFile(string fname,vector<vector<float>>& vecs,float& min, float& max,float& xres, float& yres, string& projection);

void bilinearintertop(vector<vector<float>>& input, int width, int height);

void  generateTexture(string fname, GLuint& tex, int bandnum);

void createMesh(vector<vector<float>>& input,float xres,float yres,float max, vector<int>& indicies, vector<Vertex>& vertexes);

#endif
