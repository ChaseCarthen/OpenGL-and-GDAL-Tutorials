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

**Possible different ways to render shapes**
---------

**Issues, Issues, Issues....**
-------

**How to shape up the visualization.**
-------
