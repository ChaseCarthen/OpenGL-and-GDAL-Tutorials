#Tutorial 4-1 - Projecting onto Geometry
**Introduction**
----
In this tutorial we will cover how to create a projector that will project images onto any geometry rendered in OpenGL. This tutorial will essentially be creating projector similar to a movie projector. This projector will be later used to splat satellite imagery onto a terrain or DEM. Our goal is to project a flat image onto the terrain.

**Required & Supplemental Readings**

1. NVIDIA PDF
2. A tutorial similiar to this one in Direct x


**What is a projector?**
-----

**Different Types of Projection**

Orthographic and perspective projections are two types of projections that are commonly known, but there are others. In this tutorial I will be specifically using only an orthographic projection. Here is an example of the frustrum that is made by a perspective and orthographic projection.

![image of orthographic and perspective projection](http://i.stack.imgur.com/zyGF1.gif)

The main difference between an orthographic and perspective projection can be seen from their frustrums. A point at some position in the orthographic frustrum is mapped back to the end of the frustrum volume (or the screen) and has a straight line. This means that any shape will be flattened and any 3D object you will get a flat view of it which is thanks to rectangular cube form this projection has. However, with a perspective projection  a 3D object will appear to have a depth because of the pyramid shape of its frustrum. 

Both of these projections can be used for a projector and being used to splat images onto the terrain. The projector that I am discussing will always be pointing down and projecting some image given to it. Since images are flat, the orthogrpahic projection would be the best choice for projecting imagery onto the terrain. Another good reason for using an orthographic projection is because the shape of the frustum in terms of its width and height always remain the same with the length. It will be easier to project a image of a certain size.

**Projecting onto Geometry**

Some text.

**Some Math to make a projector work.**

Some text.

**Steps for Rendering the projector.** 
-----

Some text.

**Example Application**
----



**Concluding Notes and Next Tutorial**
-----
