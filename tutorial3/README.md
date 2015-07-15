#Tutorial 3: Shapes 
-------
![shapes](http://www.swintonfitzwilliam.org/wp-content/uploads/2013/09/shapes-2.jpg)

Just like the image shown above  we will be loading shapes, but of a more complex type. Like I discussed in the first tutorial shapes are vector detasets. Shapefiles are typically used for representing roads, weatherstations, boundairies of watersheds, and others.

**Steps for loading a shape**

1. Load the shape
2. Find all shapes within the data.
3. Deal with projecting the shapefile into your visualization
4. Visualize the shape(s)

**Getting the Shape Information**
------
The following headers will be need for getting information from shapefiles.
```c++
#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
```

We will be using the OGR portion of GDAL to get information of .shp files or a.k.a. the ESRI shapfile format. I will be using boundDCEW from my data section of this repo. 

Lets open a shapefile.
```c++
ds = OGRSFDriverRegistrar::Open( "boundDCEW.shp", FALSE );
```

Now that we have a shapefile open lets get it's projection information for future uses by grabbing the first layer in the shapefile.
```c++
    // Grab the first layer
    OGRLayer* layer = ds->GetLayer(0);

    // Grab the spatial reference and create a coordinate transform function
    sr = layer->GetSpatialRef();
```

A layer within a shapefile is very similar to a band in a GDAL dataset. Within a shapefile layer there are many features that can contain anything from metadata to the actual shapedata itself or the geometry. In this case we only care for the geometry within the dataset. As you will see a shapefile consists of many different geometry types from this loading code:
```c++
    OGRFeature *poFeature;

    layer->ResetReading();
    while ( (poFeature = layer->GetNextFeature()) != NULL )
    {
        OGRFeatureDefn *poFDefn = layer->GetLayerDefn();
        int iField;

        OGRGeometry *poGeometry;

        poGeometry = poFeature->GetGeometryRef();
        if ( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
        {
            OGRPoint *poPoint = (OGRPoint *) poGeometry;
            cout << "POINT!!!" << endl;
            double x = poPoint->getX();
            double y = poPoint->getY();
            points.push_back(vector<glm::vec2>());
            points[points.size() - 1].push_back(glm::vec2(x, y));
        }
        else if (poGeometry != NULL
                 && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
        {
            OGRLineString* ls = (OGRLineString*)poGeometry;
            points.push_back(vector<glm::vec2>());
            for (int i = 0; i < ls->getNumPoints(); i++ )
            {
                OGRPoint p;
                ls->getPoint(i, &p);

                // This function can transform a larget set of points.....
                double x = p.getX();
                double y = p.getY();
                points[points.size() - 1].push_back(glm::vec2(x, y));
                //poTransform->Transform (1, &x, &y);
                //cout << "POINT" << p.getX() << " " << p.getY() << endl;
            }
        }
        else if (poGeometry != NULL
                 && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
        {

            OGRLineString* ls = (OGRLineString*)poGeometry->getBoundary();
            points.push_back(vector<glm::vec2>());
            cout << "POLYGON" << ls->getNumPoints() << endl;
            //exit(0);
            for (int i = 0; i < ls->getNumPoints(); i++ )
            {
                OGRPoint p;
                ls->getPoint(i, &p);

                // This function can transform a larget set of points.....
                double x = p.getX();
                double y = p.getY();
                points[points.size() - 1].push_back(glm::vec2(x, y));
                //poTransform->Transform (1, &x, &y);
            }
        }
        OGRFeature::DestroyFeature( poFeature );
    }
```

**What type of Shapes are there**
--------
Since we are dealing with a  vector dataset, there can be points that are connected or not connected all. A bunch of connected points can form lines, different shapes, and different polygons. While in vice versa there can be single points that are not connected to anything. In a shapefile it is possible for the points to be 2D or 3D. Let's take a second look at the shape loading code above and create a data structure to hold the points:

```c++

// Use this datastructure to store your points the idea is to capture what is a line and not a line within
// this can be done by checking the size of the vector<glm::vec2>
// if it is one, then it is a point, but otherwise you know it is a line or polygon
vector<vector<glm::vec2>> points = vector<vector<glm::vec2>>();

if ( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
{
    // here s ingle point
    OGRPoint *poPoint = (OGRPoint *) poGeometry; // single point
    cout << "POINT!!!" << endl;
    // You can get the values from single and many points as follows 
    double x = poPoint->getX();
    double y = poPoint->getY();
    points.push_back(vector<glm::vec2>());
    points[points.size() - 1].push_back(glm::vec2(x, y));
}
// Both use line strings but require different ways to pull the line string out
else if (poGeometry != NULL
                 && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
{
    OGRLineString* ls = (OGRLineString*)poGeometry; // a point of points that are connected
    points.push_back(vector<glm::vec2>());
    // Heres how to pull out multiple points
    for (int i = 0; i < ls->getNumPoints(); i++ )
    {
        OGRPoint p;
        ls->getPoint(i, &p);

        // This function can transform a larget set of points.....
        double x = p.getX();
        double y = p.getY();
        points[points.size() - 1].push_back(glm::vec2(x, y));
    }
}
else if (poGeometry != NULL
                 && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
{
    // You have to get the boundary in the case of polygons 
    OGRLineString* ls = (OGRLineString*)poGeometry->getBoundary();
        ..
}
```

I have only scrathed surface of pulling data out of shapes, but it is enough to pull out all of the points in a shapefile with one layer. It does not cover pulling out the metadata.

**Possible different ways to render shapes**
---------
Shapes are interesting in the fact that they can be points, lines, or polygons. They can be visualized in 2D as an image or in 3D in several different various ways. This tutorial will show you how to make lines and polygons into ribbons. For example a single line would be equivalent to a single ribbon or a line that has line height. Single points will be visualized as a rectangle facing in one direction (in my code they are facing the z axis).

**Building the Shapes**
``` c++
for (int i = 0; i < points.size(); i++)
{
    first = true;
    for (int j = 0; j < points[i].size(); j++)
    {
        double x = points[i][j].x;
        double y = points[i][j].y;

        transform->Transform (1, &x, &y);
        glm::vec2 orig = glm::vec2(x, y);
        x = x - origin.x;
        y = origin.y - y;

        Vertex temp = {{(float)x, (float)t.SampleTerrain(orig), (float)y}, {(float)1, (float)0, (float)0}, {(float)1, (float)1}};
        if (first)
        {
            vertexs.push_back(temp);
            temp.position.y += 40;
            vertexs.push_back(temp);
            first = false;
            if (j == points[i].size() - 1)
            {
                temp.position.y -= 40;
                temp.position.z += 40;
                vertexs.push_back(temp);
                temp.position.y += 40;
                vertexs.push_back(temp);
                indicies.push_back(vertexs.size() - 3);
                indicies.push_back(vertexs.size() - 1);
                indicies.push_back(vertexs.size() - 4);
                indicies.push_back(vertexs.size() - 2);
                indicies.push_back(vertexs.size() - 4);
                indicies.push_back(vertexs.size() - 1);
                continue;
            }
        }
        if (j + 1 < points[i].size())
        {
            x = points[i][j + 1].x;
            y = points[i][j + 1].y;

            transform->Transform (1, &x, &y);
            
            orig = glm::vec2(x, y);
            x = x - origin.x;
            y = origin.y - y;

            Vertex temp2 = {{(float)x, (float)t.SampleTerrain(orig), (float)y}, {(float)1, (float)0, (float)0}, {1, 1}};
            vertexs.push_back(temp2);
            temp2.position.y += 40;
            vertexs.push_back(temp2);
            
            // Triangulate!
            indicies.push_back(vertexs.size() - 3);
            indicies.push_back(vertexs.size() - 1);
            indicies.push_back(vertexs.size() - 4);
            indicies.push_back(vertexs.size() - 2);
            indicies.push_back(vertexs.size() - 4);
            indicies.push_back(vertexs.size() - 1);
        }


    }
}
```
**Issues, Issues, Issues.**
-------

**How to shape up the visualization.**
-------
