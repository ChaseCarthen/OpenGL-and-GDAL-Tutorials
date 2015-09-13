![equation](http://yuml.me/diagram/scruffy/class/edit/%2F%2F Cool Class Diagram, [Customer|-forname:string;surname:string|doShiz()]<>-orders*>[Order], [Order]++-0..*>[LineItem], [Order]-[note:Aggregate root{bg:wheat}])

#Tutorial 4-1 - Projecting onto Geometry
**Introduction**
----
In this tutorial we will cover how to create a projector that will project images onto any geometry rendered in OpenGL. This tutorial will essentially be creating projector similar to a movie projector. This projector will be later used to splat satellite imagery onto a terrain or DEM. Our goal is to project a flat image onto the terrain.

**Required & Supplemental Readings**

1. NVIDIA PDF
2. A tutorial similiar to this one in Direct x

**Libraries Required**

1. glm
2. gdal or some other image library.

**What is a projector?**
-----
A projector is very much like a movie projector in the fact that it projects an image or movie onto a surface. The only big difference is that the projector we are making is not made of physical components. A projector in OpenGL requires a view matrix to place it somewhere in the world along with its view direction and a projection matrix that determines the type of matrix for the clipping plane. Of course with a projector an image is needed to be projected onto the terrain which will be discussed later in this tutorial. 

**Steps for Rendering the projector.** 

1. Place the projector
2. Apply projection to the projector
3. Check if geometry is inside the projector's frustrum
4. Apply projecter's texture to the geometry

**Different Types of Projection**

Orthographic and perspective projections are two types of projections that are commonly known, but there are others. In this tutorial I will be specifically using only an orthographic projection. Here is an example of the frustrum that is made by a perspective and orthographic projection.

![image of orthographic and perspective projection](http://i.stack.imgur.com/zyGF1.gif)

The main difference between an orthographic and perspective projection can be seen from their frustrums. A point at some position in the orthographic frustrum is mapped back to the end of the frustrum volume (or the screen) and has a straight line. This means that any shape will be flattened and any 3D object you will get a flat view of it which is thanks to rectangular cube form this projection has. However, with a perspective projection  a 3D object will appear to have a depth because of the pyramid shape of its frustrum. 

Both of these projections can be used for a projector and being used to splat images onto the terrain. The projector that I am discussing will always be pointing down and projecting some image given to it. Since images are flat, the orthogrpahic projection would be the best choice for projecting imagery onto the terrain. Another good reason for using an orthographic projection is because the shape of the frustum in terms of its width and height always remain the same with the length. It will be easier to project a image of a certain size.

**Projecting onto Geometry**

Now that projections have been made clear lets go over how images are projected onto the terrain. I will be referencing from the OpenGL tutorial that I have placed in the Required Readings section. This will require some math to be covered for projecting an image onto geometry. One key that I would like to note is that the math required displaying geometry onto the screen is relatively the same as projecting onto the terrain. Now to go over some math and code.

**Some Math and Code to make a projector work.**

In order for the projector to place something onto the terrain, we first need to have a view matrix that will place the projector somewhere in the world like a camera. The second we will need is the projection matrix which will be orthographic. For the view matrix I will use glm's lookat to construct the view matrix and glm's ortho to construct the orthographic matrix.

Here is some code that constructs the view and projection matrix
```c++
// The view matrix

// The projection matrix

```

Now that we have our projection and view matrix constructed we can check if the geometry is within the projector's frustrum. This step is important for insuring that the projector only casts a texture in the correct spot. In order to do this we need to project our projector's coordinate space onto the geometry. The following formula will achieve this:

Some Math

Explain the formula and uv mapping for textures.. (if needed)

Show the shader and cpu code.



**Example Application**
----



**Concluding Notes and Next Tutorial**
-----
