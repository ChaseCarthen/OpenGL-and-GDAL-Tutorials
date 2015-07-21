#Tutorial 4: Projecting GIS Imagery onto a Terrain with Projective Mapping

**Introduction**
-----
In this tutorial, I will be talking how to create to use projective mapping to place images onto the terrain we produced in tutorial 4. 

Required Readings
* https://developer.nvidia.com/system/files/akamai/gamedev/docs/projective_texture_mapping.pdf

Be Aware of
* Deffered Shading
* Alpha Blending
**Projective Texture Mapping**
---
The idea behind projective texture mapping is to produce a set of uvs that will be used for the purpose of projecting an image onto a surface. Projective texture mapping works very similar to the way to the main camera that is commonly used in OpenGL. Projective texture mapping makes use of its own projection and view matrixes like a main camera, except that everything is being projected into the world on some surface. For now we will refer to projective texture mapping in general terms as a projector.

Our first goal is to understand how a projector has its own view and projection matrix with the addition of a scaling matrix that are essential to producing uvs that are projected onto the terrain (I will be calling the combination of these matrixes a texgen matrix). In the Nvidia paper it throws these two equations at the reader for developing the texgen matrix:

**Coordinates are in view space**
![eyelineartexgen.png](eyelineartexgen.png)

**Coordinates are in object space**
![objectlineartexgen.png](objectlineartexgen.png)

Both of the following equations above are equivalent and are just demonstrating two different cases where you either have coordinates in object or view space. The first equation above simply reverts to the second case (make sure you understand the differences between view and object space). For the rest of this tutorial we will focus on the object space case. 

The Nvidia paper specifies that one must use the following equation to transform a object space point into uv coordinates as follows:

![texgen.png](texgen.png)

The Vx-w components are your coordinates in object space, while T<sub>o</sub> is the texgen matrix produce in case two above. As you can see the only thing we need for a projector is its projection matrix, view matrix, scaling matrix, and of course we need some kind of image or data to project. 

One question that may be entering your mind is about the coordinates produced on the right side of the above question or the uv coordinates we need for the projecting and the scaling matrix. The paper specifies that the s,t components are typically divided by q to bring them into a normalized range between 0 and 1 but while making some other considerations such as the projection and view matrix brings our final point into clip space where the window width and height are mapped to [-1/2,1/2],[-1/2,1/2]. The following tutorial does a good job of taking care of these considerations http://www.rastertek.com/dx11tut43.html by multiplying and adding by 1/2 to the produced clip space coordinates (after they have been divided by q). Unfortunately we don't need to that when the scaling matrix already does the dividing by q and multiplying and adding for us (I dare you to find how equivalent these two are (left as an exercise for the reader)). So Nvidia just did something equivalent for scaling and normaling the uvs into a [0,1] range. The point is we don't need to worry about dividing by q (or adding and multiplying 1/2) to s and t thanks to this scaling matrix.

On to the code.
**The Code**
----
How many things do we need to keep track for this projector we are creating to work. The answer is simple: the view matrix, projection matrix, scaling matrix, and the image or data being used for this projector. Here is what we are keeping track of:
```c++
up = glm::vec3(0, 0, -1); // Set our up to -z acis
direction = glm::vec3(0, -1, 0); // this is will face the cam directly up
position = glm::vec3(110, 0, 110);
view = glm::lookAt( position, //Eye Position
	                  position + direction, //Focus point
	                  up); //Positive Y is up
glm::mat4 projection = glm::ortho<float>(-1000, 1000, 1000, -1000, 0.1f, 10000.0f);  // orthographic projections are nice for demonstrating satellite images and data
glm::mat4 scalingMatrix = glm::mat4(1/2,0,0,1/2,
                                    0,1/2,0,1/2,
                                    0,0,1/2,1/2,
                                    0,0,0,1);
gluint SomeImageOrData;
```
**With Deferred Shading**

We now have enough information to put our projector somewhere and project imagery into the world assuming that we are using deffered shading, but it would be different with forward (more on this later). I will be assuming that all world positions, diffuse colors, and normal are stored into textures for the lighting phase of deffered shading. Given the texture with world positions and diffuse colors we can project a texture into the world. We will also need to pass some kind of texture to be projected onto the world. Our projector will be considered a light because it technically is and will be rendered screenwise with a quad.

Here is the frag shader that we will be needed to show a texture being projected into the world:
```glsl
#version 330 
uniform sampler2D gPositionMap; 
uniform sampler2D gColorMap; 
uniform sampler2D gNormalMap; // not used

uniform sampler2D proj_tex; // our texture to be projected

uniform mat4 projection;
uniform mat4 view;
uniform mat4 tex;

// Thank you oglvdev
uniform vec2 gScreenSize;
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}

layout (location = 0) out vec4 DiffuseOut; 

void main()
{
  vec2 TexCoord = CalcTexCoord();

  vec3 pos = texture(gPositionMap,TexCoord).xyz;
  vec3 diffuse = texture(gColorMap,TexCoord).xyz;
  
  // calculate the tex gen matrix and apply it to the world position
  vec4 test = (tex * projection * view * vec4(pos,1.0));
  vec2 uv = test.xy;
  if( test.w > 0 &&  uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1)
  {
    // 
    DiffuseOut = vec4(texture(proj_tex,uv.xy).xyz,.8);
  }
  else
  {
    // discard anything not inside the uv rectangle
    discard;
  }
}
```

**With Forward Shading**
This is left as an exercise for the reader. The reader will find this to be easy with a small amount of objects, but with less objects it may be feasible. The reader would need to pass in all projector for each object that is being rendered.


**Creating two types of imagery.**
-----

We now know how to place a projector and to pick a projection to project an image onto the terrain. For the purposes of this tutorial, I will be introducing how to project a satellite image and raw floating pointing data.

**Satellite Imagery**

In order to generate satellite imagery, we need to first open an image and get the bytes out of the image. All of which we will be using gdal for. Here is some code that will do that:
```c++
void generateImageTexture(string fname, GLuint& tex, string& projection,double& xorigin, double& yorigin, int& width, int& height, double& xres, double& yres)
{

  GDALDataset *poDataset;
  GDALAllRegister();
  poDataset = (GDALDataset*) GDALOpen(fname.c_str(), GA_ReadOnly);
  if (poDataset == NULL)
  {
    cout << "OUCH!" << endl;
    //exit(0);
    return;
  }
  cout << "Data size: " << GDALGetRasterXSize(poDataset) << " " << GDALGetRasterYSize(poDataset) << endl;
  width = GDALGetRasterXSize(poDataset);
  height = GDALGetRasterYSize(poDataset);

  GDALRasterBand  *poBand;
  int             nBlockXSize, nBlockYSize;
  int             bGotMin, bGotMax;
  double          adfMinMax[2];
  int numbands = poDataset->GetRasterCount();
  cout << numbands << endl;
  if(numbands != 4)
  {
    cerr << "NOT FOUR BANDS!!!" << endl;
    GDALClose( (GDALDatasetH) poDataset);
    return;
  }

  // yay stack allocation -- replace with dynamic in the future
  unsigned char** data;
  unsigned char* packeddata;

  data = new unsigned char*[numbands];
  packeddata = new unsigned char[numbands*width*height];

  for (int i = 0; i < numbands; i++ )
  {
      data[i] = new unsigned char[width*height];
      cout << i << endl;
      GDALRasterBand  *poBand;
      poBand = poDataset->GetRasterBand( i+1 );
      // Used unsiged chars for data type
      poBand->RasterIO(GF_Read, 0, 0, width, height, data[i], width, height, GDT_Byte, 0, 0);
      cout << (int)data[i][0] << endl;
  }

  // Pack the bytes into an order R,G,B,A,R,G,B,A,.....,R,G,B,A
  for(int i =0; i < width*height; i++)
  {
    packeddata[i*4] = data[0][i];
    packeddata[i*4+1] = data[1][i];
    packeddata[i*4+2] = data[2][i];
    packeddata[i*4+3] = data[3][i];
  }

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &packeddata[0]);

  // Clean up some allocated data
  delete []packeddata;

  for(int i = 0; i < numbands; i++)
  {
    delete []data[i];
  }
  delete []data;
  projection = string(poDataset->GetProjectionRef());
  double xright,ybottom;
  ComputeGeoProperties(poDataset, width, height, xorigin, yorigin, xright, ybottom, xres, yres);
  GDALClose( (GDALDatasetH) poDataset);

  return;
}
```

As you will see in the code, I get all four bytes of the satellite imagery data and push them into a OpenGL Texture 2D. The Satellite Imagery four bands are litterally red,green,blue, and alpha for this image. Passing this into the shaders should be enough to show something. This code also assumes that the satellite image only has four bands, so I am living it as an exercise for those who want to use higher quality landsat images.

**Floating Point Datasets**


**Placing the Imagery into the correct spot.**

**Application Example**
----

**Resulting Screenshot**
![Resulting Screenshot](application.png)
**Verification Screenshots**
![qgis screenshot](qgis.png)
![qgis second screenshot](qgis2.png)
