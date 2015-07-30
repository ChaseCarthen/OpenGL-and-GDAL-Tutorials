#Tutorial 4: Projecting GIS Imagery onto a Terrain with Projective Mapping

**Introduction**
-----
In this tutorial, I will be talking how to create to use projective mapping to place images onto the terrain we produced in tutorial 4. For this tutorial we will be using drycreek2.tif.

Drycreek2.tif was created with:
```bash
gdalwarp -t_srs epsg:26911 drycreek.tif drycreek2.tif # This brings Dry Creek into zone 11. 
```

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

You will need to create a seperate framebuffer for the projectors.

We now have enough information to put our projector somewhere and project imagery into the world assuming that we are using deffered shading, but it would be different with forward (more on this later). I will be assuming that all world positions, diffuse colors, and normal are stored into textures for the lighting phase of deffered shading. Given the texture with world positions and diffuse colors we can project a texture into the world. We will need to create a framebuffer that accumulates the results into one render target

Here is the frag shader that we will be needed to show a texture being projected into the world:
```glsl
#version 330 
uniform sampler2D gPositionMap; 
uniform sampler2D gTextureMap; // the texture to be used for holding the projections
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

// We want the render target location to be zero with one framebuffer.
layout (location = 0) out vec4 TexOut; 

void main()
{
  vec2 TexCoord = CalcTexCoord();

  vec3 pos = texture(gPositionMap,TexCoord).xyz;
  vec3 texmap = texture(gTextureMap,TexCoord).xyz;
  
  // calculate the tex gen matrix and apply it to the world position
  vec4 test = (tex * projection * view * vec4(pos,1.0));
  vec2 uv = test.xy;
  if( test.w > 0 &&  uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1 )
  {
    TexOut = vec4(texture(proj_tex,uv.xy).xyz,.8);
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

Another type of dataset that is not necessarily a satellite image is are raw floating datasets. I have provided an example dataset of this type called em.1000.tif in the data folder. Another thing about this dataset is that possess multiple bands and those multiple bands can correspond to real world data. In this case this dataset corresponds to ftp://ftp.nwrc.ars.usda.gov/ipw/Marks%20et%20al%201999/man1/isnobal.html in the Energy & mass flux image. This dataset consists of ten bands and represents one timestep in a year for a model called isnobal or a energy-balance snowmelt model.

The following code below will give an example of how to pull this data out (tutorial 2 should make this more familiar) and a process of normalization will be used to bring the values between 0 and 1. Here is some example code that achieves pulling out the data and the normalization:

```c++
void generateTexture(string fname, GLuint& tex, int bandnum, string& projection,double& xorigin, double& yorigin, int& width, int& height, double& xres, double& yres)
{

  if (bandnum <= 0 )
  {
    bandnum = 1;
  }
  GDALDataset *poDataset;
  GDALAllRegister();
  poDataset = (GDALDataset*) GDALOpen(fname.c_str(), GA_ReadOnly);
  if (poDataset == NULL)
  {
    cout << "OUCH!" << endl;
    //exit(0);
    return;
  }
  projection = string(poDataset->GetProjectionRef());
  width = GDALGetRasterXSize(poDataset);
  height = GDALGetRasterYSize(poDataset);

  GDALRasterBand  *poBand;
  int             nBlockXSize, nBlockYSize;
  int             bGotMin, bGotMax;
  double          adfMinMax[2];
  int bands = poDataset->GetRasterCount();
  bandnum = bandnum % bands + 1;
  if (bandnum > bands)
  {
    bandnum = 1;
  }
  poBand = poDataset->GetRasterBand( bandnum );
  poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
  printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
          nBlockXSize, nBlockYSize,
          GDALGetDataTypeName(poBand->GetRasterDataType()),
          GDALGetColorInterpretationName(
            poBand->GetColorInterpretation()) );

  float max = adfMinMax[0] = poBand->GetMinimum( &bGotMin );
  float min = adfMinMax[1] = poBand->GetMaximum( &bGotMax );
  if ( ! (bGotMin && bGotMax) )
    GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);


  float *pafScanline;
  min = adfMinMax[0];
  max = adfMinMax[1];

  pafScanline = (float *) CPLMalloc(sizeof(float) * 512 * 512);
  vector<vector<float>> out = vector<vector<float>>(height, vector<float> (width, 0));
  //vector<vector<unsigned char>> texs = vector<vector<unsigned char>>(height,vector<unsigned char> (width,0));
  unsigned char texs[512 * 512];
  
  // Here I am downsampling the float array to 512 x 512.  Not necessarily needed.
  poBand->RasterIO(GF_Read, 0, 0, width, height, pafScanline, 512, 512, GDT_Float32, 0, 0);
  float no = poBand->GetNoDataValue();

  for (int i = 0; i < 512; i++)
  {
    for (int j = 0; j < 512; j++)
    {
      //cout << i << j << endl << pafS;
      if (pafScanline[i * width + j] != no)
      {
        // set tex val -- normalize values to a range between 0 and 1
        texs[i * 512 + j] = (unsigned char)(255 * abs((pafScanline[i * 512 + j] - min) / (max - min)));
      }
      else
      {
        // Set zero val == setting any no data values to zero
        texs[i * 512 + j] = 0;
      }
    }
  }
  CPLFree(pafScanline);
  double xright,ybottom;
  ComputeGeoProperties(poDataset, width, height, xorigin, yorigin, xright, ybottom, xres, yres);
  width = height = 512;
  xres = (xright - xorigin)/512.0f;
  yres = (yorigin - ybottom)/512.0f;

  // Create a texture
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // Our normalized values are stored in the red channel
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, texs);
  GDALClose( (GDALDatasetH) poDataset);

  return;
}
```

With the normalized ranges computed we can now in a shader do the following to represent the data:
```glsl
colorout = mix(lowcolor,highcolor,normalizedvalue.a); // all you need to do is pass the values into the shader through the texture
```

**Part B: Floating Point Dataset or Image Masks**
I have discussed how it would be possible to load images or floating point datasets. However, what if one wishes to mask a part of the data that is being visualized. The key thing to do here would be to load a mask image that will mask the key vital areas. 

Steps for applying a mask:
1. Load the mask in the same way for the float data
2. Pass the mask into the shader
3. Draw only where the mask has values

```glsl
// assuming the mask values are stored in the red channel
float maskval = texture(masksampler,uv).r;

if(maskval > 0 )
colorout = mix(lowcolor,highcolor,normalizedvalue.a);
else
discard; // or colorout = vec4(0,0,0,0); The point here is that we don't want to draw where there is no mask value. 
```
**Placing the Imagery into the correct spot.**

Just like the shapes and other gis datasets we need to place everything into the correct spot and at the same time make sure the image is the right size and resolution. The very first thing we need to do is find the width, height, and origin of the a dataset (A small review).

```c++
void ComputeGeoProperties(GDALDataset *poDataset, int width, int height, double& x, double& y, double& xright, double& ybottom, double& xres, double& yres)
{
  double adfGeoTransform[6];
  if ( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
  {
    printf( "Origin = (%.6f,%.6f)\n",
            adfGeoTransform[0], adfGeoTransform[3] );

    printf( "Pixel Size = (%.6f,%.6f)\n",
            adfGeoTransform[1], adfGeoTransform[5] );
    x = adfGeoTransform[0];
    y = adfGeoTransform[3];
    xright = x + adfGeoTransform[1] * (double)(width);
    ybottom = y + adfGeoTransform[5] * (double)(height);
  }
  else
  {
    return;
  }

  string proj;
  proj = string(poDataset->GetProjectionRef());

  OGRSpatialReference sr2;
  char* test = &proj[0];
  sr2.importFromWkt(&test);

  // Get Geography Coordinate System
  OGRSpatialReference* geog2 = sr2.CloneGeogCS();

  // Now to create coordinate transform function
  OGRCoordinateTransformation* poTransform2 = OGRCreateCoordinateTransformation( &sr2, geog2 );

  // Compute corners
  double x2=0;
  //poTransform2->Transform(1, &x2, &y);
  //poTransform2->Transform(1, &xright, &ybottom);

  // Transform to utm on both corners
  //transformToUtm(x, y,&sr2);
  //transformToUtm(xright, ybottom,&sr2);

  // Lets compute the resolution -- Despite the one provided by the geotransform
  double absoluteW = xright - x;
  double absoluteH = y - ybottom;

  // now lets compute the average resolution of the DEM
  xres = absoluteW / (width);
  yres = absoluteH / (height);
  cout << "XRES: " << xres << "YRES: " << yres<< endl;
    cout << "XRIGHT" <<xright << " " << ybottom << endl;
  cout << "x" << x << " " << y << endl;
  cout << width << endl;
  cout << height << endl;
  cout << y << endl;
  //exit(0);
};
```

With all of this information we can now place the projector above the spot we wish to project the image and set it proper boundaries for the projection. First a question for the reader: What type of projection is appropriate for projecting the image onto the terrain? The appropriate answer would be it depends on how you do it and how easy you want to make your life. Here is an image of a perspective and orthographic projection:

![image of orhtographic and perspective projection](http://i.stack.imgur.com/zyGF1.gif)

As you can see the perspective projection on the left that has a pyramid shape would be difficult to get the correct projected image onto the terrain because you have to spend time finding the correct elevation to put the projector. While the orthographic projection can be placed at any height and still be the same thanks to it projecting a plane. Here is some code that places the projector into the correct spot and sits the projection correctly.

```c++

	void SetDimensions(float width, float height)
	{
		projection = glm::ortho<float>(-width / 2.0f, width / 2.0f, height / 2.0f, -height / 2.0f, 0.1f, 10000.0f);
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

		// set projector width and height
		SetDimensions((xright-x),(y - ybottom));
		x = x - mainorigin.x;
		y = mainorigin.y - y;
		
		// the projectors y is at 3000 or remotely above the terrain so that the frustrum will go through the terrain
		position.x = x;
		position.z = y;


		cout << "projector position" << x << " " << y;
		//exit(0);
	};
	
void projector::render(glm::mat4& view2, glm::mat4& projection2)
{
	// remains the same as above
	view = glm::lookAt( position, //Eye Position
	                    position + direction, //Focus point
	                    up);
	                    .
	                    .
	                    .
	                    .
}	         
```
As you can see the process of placing the projector into the correct spot and making the texture the right resolution are a combination of tutorials 2 and 3. One small note about glm::lookAt is that the up is placed on the negative z axis and the projector direction is made to look at negative y axis which is equilvalent to looking straight down. Make the frustrum collides with your terrain.

**How to visualize everything correctly.**

Steps for rendering (Deferred Shading):

1. Do Geometry Pass
2. Do Projection Pass into projector framebuffer.
3. Do Lighthing Pass

The steps above are straight forward and need very little explanation. We want our projected result to blend with anything from the lighting pass. We need to make sure that the projected image will appear through the lighting and other projectors (they may overlap). The best way is to blend the output of the projectors into one texture (one way is to use discard in the shader or to use gl_blend(discard is easier).

```c++
// switch to Multiple render targets
// Set proper depth buffer
// clear color to black.
// use gbuffer fragment buffer 
//geometry pass

// use pbuffer fragment buffer
glblendequation(GL_FUNC_ADD);
glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);

// Projector pass -- use discard and glblend with another framebuffer -- make sure to clear it to black
// render the projectors -- to blend them in case overlap make sure clear color is black

// Switch to default buffer -- blend the light
glBlendEquation(GL_FUNC_ADD);
glBlendFunc(GL_ONE, GL_ONE);
// Do lighting phase
glDisable(GL_BLEND);
```

We need to step up a framebuffer that will contain our changes with each framebuffer projection.
First lets setup the framebuffer and attach one render texture to contain the changes.
Here is some code that contains this setup:

```c++
GLuint projtex;
GLuint m_fbo;

glGenFramebuffers(1, &m_fbo);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

// Create the gbuffer textures
glGenTextures(1, projtex);
glGenTextures(1, &m_depthTexture);

// bind the texture
glBindTexture(GL_TEXTURE_2D, projtex);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

// Lets store everything into a texture with rgba channels
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i+4, GL_TEXTURE_2D, m_textures[i], 0);


// bind a dpeth buffer -- technically not needed 
glBindTexture(GL_TEXTURE_2D, m_depthTexture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT4 };
glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

if (Status != GL_FRAMEBUFFER_COMPLETE) {
     //printf("FB error, status: 0x%x\n", Status);
     return false;
}

// restore default FBO
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
```

As you can see I am only binding one texture to this framebuffer, which means everything that will be render will go into this one texture. Within this texture we will need to blend all of the results of the projectors into this one framebuffer. 

First we will need to setup up blending where we will blend based on the alpha values of the projectors so that we can control how well two projectors blend:
```c++
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
glEnable(GL_BLEND);

glBlendEquation(GL_FUNC_ADD);

glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

// Render projectors

// Can't forgot to disable blending afterwords
glDisable(GL_BLEND);

//Result default frame buffer  
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

// set render texture to your desired texture target    
glActiveTexture(GL_TEXTURE0 + 4);
glBindTexture(GL_TEXTURE_2D, projtex);
```

After the blending is enable now we can render all of the projectors but the key thing we need for these projectors are the positions of the verticies from the geometry pass in deffered shading ( Refer to ogldev for more details). The projectors will be rendered for the full screen or into a quad that is the same size of the screen.

Rendering code for one projector:
```c++
// Set the texture we are rendering to one specific target.
glActiveTexture(GL_TEXTURE0 + 5);
glBindTexture(GL_TEXTURE_2D, tex);

// if there is a mask tex enable it
if(masktex)
{
   glActiveTexture(GL_TEXTURE0 + 6);
   glBindTexture(GL_TEXTURE_2D,masktex);
}

Renderer.useProgram();
// bind the buffer of our vbos
Buffer.bindBuffer();

Renderer.enableVertexAttribPointer("Position");
Renderer.setGLVertexAttribPointer("Position", 2, GL_FLOAT, GL_FALSE, sizeof(float), 0);

// enable three textures
Renderer.setUniformInteger("gPositionMap", 0);
Renderer.setUniformInteger("gTextureMap", 4); // the texture map that we have rendered into previously .. we need this for blending
Renderer.setUniformInteger("proj_tex", 5); // the texture we are placing onto the terrain or some object
Renderer.setUniformInteger("mask_tex",6); // the texture we are using to mask the projected texture

float SCREEN_SIZE[2] = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
Renderer.setUniformFloatArray2("gScreenSize", 1, SCREEN_SIZE); // pass screen size to shaders
Renderer.setUniformMatrix4x4("projection", projection);
Renderer.setUniformMatrix4x4("view", view);
Renderer.setUniformMatrix4x4("tex", Texgen); // pass scaling matrix to shaders

Renderer.renderRaw(GL_TRIANGLES, 12); // Render a quad
Renderer.disableVertexAttribPointer("Position"); // disable a vao.
// Don't forgot to disable blending
glDisable(GL_BLEND);
```
**Application Example**
----
To run the example:
```bash
cd build
make
../bin/out
```

Here is one screenshot from running my example code and two verification screenshots from qgis.
**Resulting Screenshot**
![Resulting Screenshot](application.png)
**Verification Screenshots**
![qgis screenshot](qgis.png)
![qgis second screenshot](qgis2.png)

**Issues**

As you will notice in the screenshots the projector overlaps onto of the shapes which can be undesireable in some cases. The simple fix for this to identify all objects in the same and store their identifies into a texture. With the identity known a simple if statement is needed for telling the projector not to render on a shape identity.

**Final Note**

This should give you enough to create your very own projector for projecting GIS datasets.
