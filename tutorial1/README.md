#Tutorial 1 : A GDAL Introduction

Suggested readings
* http://www.gdal.org/gdal_tutorial.html
* http://www.gdal.org/ogr_apitut.html

**What is GDAL?**

GDAL stands Geospatial Data Abstraction Library and is used as a library to read GIS (Geographic Information Systems) raster and vector datasets that are reprsentative of many different types of data. These datasets can be Digital Elevation Maps (DEMs) or terrain, Digital Orthophoto Quarter-Quadrangle (DOQQ) or satellite imagery, roads, trees, vegetation maps, and many other types of datasets. These datasets typically have some metadata that places them somewhere in the world and their size in that area. These dataset's are typically represented in a raster or vector form as similar to the image below.

![hello world](https://www.e-education.psu.edu/natureofgeoinfo/sites/www.e-education.psu.edu.natureofgeoinfo/files/image/data_models_buffer.gif "Vector vs. Raster Example")

 As you can see from the image above raster datasets are simply just like an image with pixels with evenly distributed cells, while vector datasets can are represented by points that can be connected or not connected at all. 
 
 **Why are datasets represented either as a vector or raster? **

Vector and raster datasets are both good at representing different types of datasets. For example a road could be represented as a raster dataset, but it is more suit to be a vector dataset because it can be represented with a few points rather than as a raster that may require more pixels than points in a vector dataset. A vegetation map could be represented as a vector dataset, but it doesn't make sense for a vegatation map to be represent as points because a given point can represent a grided area. 

The short answer to the question is that it is left to up to the person who created the dataset. Using a vector representation for some datasets can allow user to define a dataset in a non-grided way and may potentially use less data. A raster dataset is good way of representing dataset in a uniform grid fashion which could be potentially good for datasets like vegetation maps.

**How are these datasets located in the world?**

Typically any particular GIS dataset has information on what part of the world it belongs too. However, one particular twist to GIS dataset is that these coordinate systems often have different origins and are even in different types of coordinate systems. The first thought that anyone thinks of when it comes to world coordinates is Latitude and Longitude that is commonly seen on a GPS. Latitude and Longitude can be used to locate any point on the earth, but what if I want to know the distance between two points. Latitude and Longitude are angular coordinates meaning one can't just simply subtract the two points and get the distance. Instead one must use some geometry to calculate the distance between the two latitude and longitude points (This won't be covered here.).

One other type of coordinate system that is often used to place GIS datasets somewhere in the world is a Universal Transverse Mercator (UTM) coordinate system. UTM projects the world onto a cylinder and is cut into 60 different zones where each zone is 6 degrees wide by Longitude. What makes UTM useful is the fact that one can subtract two points in a UTM zone and get the precise distance between two points in meters.

For more information on UTM read: http://geology.isu.edu/geostac/Field_Exercise/topomaps/utm.htm.

UTM is often used as means of making it easy to calculate the distance between two points on the map given that they are in the same UTM zone. The origin point for UTM zones are typically placed in the center of the zone at the equator. Many GIS datasets are often either in UTM or Lat Long and often it is required to reproject datasets into the same coordinate system when working with two or more datasets. Another issue is that coordinate systems are often subject to different precisions given the way they were derived at the time by cartograpers. 

![UTM and Lat Long map](http://www.dmap.co.uk/utmworld.gif "UTM Lat Long Map")

**On to the code for loading rasters!!!**

Lets open a tif with GDAL and get some helpful information alongside it

Here are the includes you will need for this tutorial.
```c++
#include <iostream>
// Gdal includes that are needed for this tutorial
#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>


```

Now lets open up a GeoTiff with GDAL.
``` c++
// Lets open a Tif
GDALDataset *poDataset;

// Register all gdal drivers -- load every possible driver gdal has
GDALAllRegister();

// lets load a "dataset" which is gdal terminology for your data
poDataset = (GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly);
```
GDAL has several different drivers that are specifically meant for opening different formats of data or for even interfacing with different services. Here is a list of all drivers that GDAL has: http://www.gdal.org/formats_list.html.

Lets get some more beneficial metadata out of the raster dataset such as where the this data is located in the world and the pixel (grid) sizes.

``` c++
// Lets get the dimensions of the tif
int width = GDALGetRasterXSize(poDataset); 
int height = GDALGetRasterYSize(poDataset);
```
GDALGetRaster[X|Y]Size is used to grab the dimensions of the tif.

``` c++ 
// Lets get the coordinate system of the tif
string proj;
proj = string(poDataset->GetProjectionRef());
```
GetProjectionRef is the coordinate system defined for this dataset. It is important for when it is needed for reproject this tif into some other coordinate system or vice versa. For example, one may want to place a road onto this tiff dataset that is a terrain which requires both datasets to be in the same coordinate system. 

Run my example code to get an idea of why this is important.

``` c++
// Lets get the upper right hand corner of the tiff and its resolution
double adfGeoTransform[6];
if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
{
    printf( "Origin = (%.6f,%.6f)\n",
        adfGeoTransform[0], adfGeoTransform[3] );

    printf( "Pixel Size = (%.6f,%.6f)\n",
            adfGeoTransform[1], adfGeoTransform[5] );
    x = adfGeoTransform[0];
    y = adfGeoTransform[3];
    xright = x + adfGeoTransform[1]*width;
    ybottom = y + adfGeoTransform[5]*height;
}
else
{
    // Something went wrong
}
```
Here I am giving an example of the all powerful GDAL geotranform for raster datasets. This Geotransform gives the north east corner of a raster dataset in most cases. This geotransform is in the coordinate system given by the projection in the example above this one. adfGeoTransform[0] and adfGeoTransform[3] give are the north east corner of the dataset. The pixel size is given adfGeoTransform[1] (for east and west) and adfGeoTransform[3] (for north and south). The pixel size represents how wide and long a pixel is for one pixel. For example for a dataset that is 10 x 10 and has a pixel size of 2 x 2 has an area of 400.

The GeoTransform can be used to find bounding box of a dataset or how wide and long it extends out into the world.

**Running the example**

**Example output**

In a later tutorial vector dataset loading will be introduced and the idea of having multiple layers in a tif. 

The next tutorial will feature building and loading a terrain with OpenGL.
