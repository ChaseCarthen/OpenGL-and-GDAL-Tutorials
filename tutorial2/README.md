#Tutorial 2 - Building and Loading a DEM (Terrain)

**Introduction**
---------
In tutorial 2, the process of creating a mesh from a Digital Elevation Map (DEM) and visualizing it will be made clear. This tutorial will give one enough information to produce the mesh with its verticies and normals. Any extra lighting and effects will be left to the user, but one directional will be applied to the terrain to show that it works.

**Required Readings and Tutorials**
Required Tutorials
* Deferred Shading: http://ogldev.atspace.co.uk/www/tutorial35/tutorial35.html

Required Readings
* Deferred vs Forward Shading : http://gamedevelopment.tutsplus.com/articles/forward-rendering-vs-deferred-rendering--gamedev-12342
* http://www.gdal.org/gdal_tutorial.html


**Loading the Data**
------
The very first thing we need to do is load some data to be visualized. I have provided some data to be visualized in the data directory. I will be using a DEM representation of an area in Boise, Idaho called Dry Creek. 

Lets create the GDAL dataset.
```c++
//vector<float> temp = vector<float>()
GDALDataset *poDataset;

// Register all gdal drivers
GDALAllRegister();

// lets load a "dataset" which is gdal terminology for your data
poDataset= (GDALDataset*) GDALOpen("drycreek.tif",GA_ReadOnly);

```
GDAL datasets can consist of multiple layers of 2D datasets called bands. These bands consist of different information that is relative to the dataset that it is part of. In this case drycreek.tif only has one band which consists of the height information at each pixel in the raster. Each of these bands are guranteed to have the same height and width.

Here is an example of how to get the number of bands, width, and height in a dataset and grabbing the first band that is required:

``` c++
int numBands = poDataset->GetRasterCount();

// dimensions of our datasets
int width = poBand->GetXSize(); 
int height = poBand->GetYSize();

// Get first raster band
GDALRasterBand  *poBand;

poBand = poDataset->GetRasterBand( 1 );
```


Lets get the float data out of the first band of the tif and store it into another array in a form I like, while filling any no data value with a zero.
```c++


// Something to hold the data --- can be any storaged. 
float *pafScanline;
pafScanline = (float *) CPLMalloc(sizeof(float)*width*height);

// RasterIO will load the data
auto err = poBand->RasterIO(GF_Read,0,0,width,height,pafScanline,width,height,GDT_Float32,0,0);

// Here is something I like to do by my own preference
vector<vector<float>> out = vector<vector<float>>(height,vector<float> (width,0));

// move everything to a vector -- slow memory but okay for what we are doing
for(int i = 0; i < height; i++)
{
  for(int j = 0; j < width; j++)
  {
    if(pafScanline[(height-1-i)*width+j] > 0)
      out[i][j] = pafScanline[(height-1-i)*width+j];
    else
      out[i][j] = 0;
  }
}
```

**Creating the Terrain Mesh**
-----
With the dataset now loaded we can create a mesh with the data.

Currently we have something like this (but with the dots sticking out different various heights):

<img src="http://www.appladesign.net/domains/create/documents/dots_266.png" width="400">

The goal of creating a mesh is literally to connect these dots in a triangle fashion that will result in the following:

<img src="http://orig01.deviantart.net/2f81/f/2007/264/e/a/terrain_mesh_by_sordith.jpg" width="400">

The example that we will be considering will create the triangles in a more uniform fahsion where each group of four dots will have two triangles and normals for each point.

Here is some code that constructs a mesh
```c++
void createMesh(vector<vector<float>>& input,float xres, float yres,float max,vector<int>& indicies, vector<Vertex>& vertexes)
{
  cout << "MAX: " << max;
  cout << "XRES " << xres << " YRES" << yres << endl;
  // Our vertex information along with normals contained inside
  std::vector<Vertex> vectors = vector<Vertex>();

  // Lets do some index rendering because it will save us some memory to some degree
  vector<int> indexs = vector<int>();

  // Time to construct a height map based on the xres and yres
  for(int i = 0; i < input.size()-1; i++)
  {
    for(int j = 0; j < input[i].size()-1; j++)
    {  

        float UL = (float)(input[i][j])/(float)(max); // Upper left
        float LL = (float)(input[i+1][j])/(float)(max); // Lower left
        float UR = (float)(input[i][j+1])/(float)(max); // Upper right
        float LR = (float)(input[i+1][j+1])/(float)(max); // Lower right
        //cout << input[i][j] << " " << max <<endl;

        if(UL <= 0)
        {
          UL = 0;
        }
        //else
          //UL = .5;
        if(UR <= 0)
        {
          UR = 0;
        }
        //else
          //UR = .5;
        if(LR <= 0)
        {
          LR = 0;
        }
        //else
         // LR = .5;
        if(LL <= 0)
        {
          LL = 0;
        }
        //else
         // LL = .5;

        
        vec3 ULV = {i*xres,UL*max,j*yres};
        vec3 LLV = {(i+1)*xres,LL*max,j*yres};
        vec3 URV = {i*xres,UR*max,(j+1)*yres};
        vec3 LRV = {(i+1)*xres,LR*max,(j+1)*yres};

        // compute smoothed normal
        vec3 a = ComputeNormal(ULV,i,j,input.size(),input[i].size(),input,max,xres,yres);
        vec3 b = ComputeNormal(LLV,i+1,j,input.size(),input[i].size(),input,max,xres,yres);
        vec3 c = ComputeNormal(URV,i,j+1,input.size(),input[i].size(),input,max,xres,yres);
        vec3 d = ComputeNormal(LRV,i+1,j+1,input.size(),input[i].size(),input,max,xres,yres);

        vectors.push_back(Vertex{ {i*xres,UL,j*yres}, a, {(float)i/(float)input.size(),(float)j/(float)input[i].size()} } );
        vectors.push_back(Vertex{ {(i+1)*xres,LL,j*yres}, b, {(float)(i+1)/(float)input.size(),(float)j/(float)input[i].size()} } );
        vectors.push_back(Vertex{ {i*xres,UR,(j+1)*yres}, c, {(float)i/(float)input.size(),(float)(j+1)/(float)input[i].size()} } );
        vectors.push_back(Vertex{ {(i+1)*xres,LR,(j+1)*yres}, d, {(float)(i+1)/(float)input.size(),(float)(j+1)/(float)input[i].size()} } );
        //cout << vectors[vectors.size()-1].normal.z << endl;

        indexs.push_back(vectors.size() - 4);
        indexs.push_back(vectors.size() - 1);
        indexs.push_back(vectors.size() - 2);
        indexs.push_back(vectors.size() - 4);
        indexs.push_back(vectors.size() - 3);
        indexs.push_back(vectors.size() - 1);
        
    }
    //break;
  }
```



**Visualizing the Terrain**
-----

**Building and Running the Tutorial**
-----

**Screenshot**


**Next Tutorial**
-------
