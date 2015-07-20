#OpenGL and GDAL Tutorials

The following Repo holds simple tutorials to work with GDAL to visualize GIS datasets with OpenGL. All of these tutorials will be made to compile both on Ubuntu (and other distros that are similar) and Mac. 

These examples will include:
* loading Shapes
* loading DEMs
* loading Geotiffs
* A brief demonstration of translating between two different projections
* OpenGL examples

The following tutorials will need these dependencies:
* glm
* GDAL
* OpenGL
* ImageMagick or GraphicsMagick
* SDL2

Supplemental Readings: 
* http://www.gdal.org/
* http://www.gdal.org/gdal_tutorial.html
* http://www.gdal.org/osr_tutorial.html
* http://help.arcgis.com/en/arcgisdesktop/10.0/help/index.html#/What_are_map_projections/003r00000001000000/

When working with your own repo and you wish to push your own geotiffs to your own remote git repo use:
```bash
echo '*.tif -delta' > .gitattributes
git gc
```
ref. http://stackoverflow.com/questions/7102053/git-pull-without-remotely-compressing-objects
