#Tutorial 4: Projecting GIS Imagery onto a Terrain

Required Readings
* https://developer.nvidia.com/system/files/akamai/gamedev/docs/projective_texture_mapping.pdf

**Projective Texture Mapping**

The idea behind projective texture mapping is to produce a set of uvs that will be used for the purpose of projecting an image onto a surface. Projective texture mapping works very similar to the way to the main camera that is commonly used in OpenGL. Projective texture mapping makes use of its own projection and view matrixes like a main camera, except that everything is being projected into the world on some surface. For now we will refer to projective texture mapping in general terms as a projector.

In the Nvidia paper it throughs two equations at the reader being: 
![eyelineartexgen.png](eyelineartexgen.png)
![objectlineartexgen.png](objectlineartexgen.png)
**Create two types of imagery.**

**Placing the Imagery into the correct spot.**

**Application Example**
----

**Resulting Screenshot**
![Resulting Screenshot](application.png)
**Verification Screenshot**
![qgis screenshot](qgis.png)
![qgis second screenshot](qgis2.png)

**Still under construction.. but to get a head start look at the following https://developer.nvidia.com/system/files/akamai/gamedev/docs/projective_texture_mapping.pdf. **
