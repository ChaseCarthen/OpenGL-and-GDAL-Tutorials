#Tutorial 4-1 - Projecting onto Geometry
**Introduction**
----
In this tutorial we will cover how to create a projector that will project images onto any geometry rendered in OpenGL. This tutorial will essentially be creating projector similar to a movie projector. This projector will be later used to splat satellite imagery onto a terrain or DEM. Our goal is to project a flat image onto the terrain.

**Required & Supplemental Readings**

1. https://developer.nvidia.com/system/files/akamai/gamedev/docs/projective_texture_mapping.pdf
2. http://www.rastertek.com/dx11tut43.html

**Libraries Required**

1. glm : http://glm.g-truc.net/0.9.7/index.html
2. gdal : http://www.gdal.org/

**What is a projector?**
-----
A projector is very much like a movie projector in the fact that it projects an image or movie onto a surface. The only big difference is that the projector we are making is not made of physical components. A projector in OpenGL requires a view matrix to place it somewhere in the world along with its view direction and a projection matrix that determines the type of matrix for the clipping plane. Of course with a projector an image is needed to be projected onto the terrain which will be discussed later in this tutorial. 

**Steps for Rendering the projector (simple view).** 

1. Place the projector
2. Check if geometry is inside the projector's frustrum
3. Apply projecter's texture to the geometry

**Different Types of Projection**

Orthographic and perspective projections are two types of projections that are commonly known, but there are others. In this tutorial I will be specifically using only an orthographic projection. Here is an example of the frustrum that is made by a perspective and orthographic projection.

![image of orthographic and perspective projection](http://i.stack.imgur.com/zyGF1.gif)

The main difference between an orthographic and perspective projection can be seen from their frustrums. A point at some position in the orthographic frustrum is mapped back to the end of the frustrum volume (or the screen) and has a straight line. This means that any shape will be flattened and any 3D object you will get a flat view of it which is thanks to rectangular cube form this projection has. However, with a perspective projection  a 3D object will appear to have a depth because of the pyramid shape of its frustrum. 

Both of these projections can be used for a projector and being used to splat images onto the terrain. The projector that I am discussing will always be pointing down and projecting some image given to it. Since images are flat, the orthogrpahic projection would be the best choice for projecting imagery onto the terrain. Another good reason for using an orthographic projection is because the shape of the frustum in terms of its width and height always remain the same with the length. It will be easier to project a image of a certain size.

**Projecting onto Geometry**

Now that projections have been made clear lets go over how images are projected onto the terrain. I will be referencing from the OpenGL tutorial that I have placed in the Required Readings section. This will require some math to be covered for projecting an image onto geometry. One key that I would like to note is that the math required displaying geometry onto the screen is relatively the same as projecting onto the terrain. Now to go over some math and code.

**Some Math and Code to make a projector work.**

In order for the projector to place something onto the terrain, we first need to have a view matrix that will place the projector somewhere in the world like a camera. The second we will need is the projection matrix which will be orthographic. For the view matrix I will use glm's lookat to construct the view matrix and glm's ortho to construct the orthographic matrix. Also the projector must be constructed in such a way that it always points down.

Here is some code that constructs the view and projection matrix
```c++
// The view matrix
glm::vec3 up = glm::vec3(0, 0, -1); // We could set the projector's up to be (0,1,0), but then we would run into problems with glm.
glm::vec3 direction = glm::vec3(0, -1, 0); // face the projector down
glm::vec3 position = glm::vec3(110, 0, 110); // set some initial position
glm::mat4 view = glm::lookAt( position, //Eye Position
	                    position + direction, //Focus point
	                    up); //Positive Y is up

// The projection matrix
glm::mat4 projection = glm::ortho<float>(
-1000, // left of projection
1000, // right of projection
1000, // top of projection
-1000, // bottom of projection
0.9f, // z Near
10000.0f // z Far
);
```

Now that we have our projection and view matrix constructed we can check if the geometry is within the projector's frustrum. This step is important for insuring that the projector only casts a texture in the correct spot. In order to do this we need to project our projector's coordinate space onto the geometry. The following formula from the Nvidia paper will achieve this:

![Projective Texture Mapping equation](equation.png)

This matrix formulated by this calculation will transform a geometry's points into the texture's space. The V<sub>p</sub> is the view matrix and P<sub>p</sub> is the projection matrix constructed for the projector. The first matrix inside in the equation is a scaling matrix and the last matrix is the model matrix used to bring the geometry into world space. By using the transform matrix formulated by these equations we get uv coordinates that are projected onto the geometry. Here is what the generated texture coordinates will look after the this transformation matrix is applied to a point in the geometry: 

<img src="uvs.png" width="200">

From working with textures commonly a two dimensional point is used, but in this case there is a four dimensional point. For this tutorial we only care about three of the dimensions: s,t, and q. The s and t dimensions are the uv coordinates that are commonly used for sampling textures and the q is used for checking to make sure the image is projected onto only the geometry(it is also used scaling). With this coordinate formed we can project an image onto some geometry or a surface. Here is some code that achieves that on the cpu side:
```c++
glm::mat4 scalingmatrix = glm::mat4(1.0 / 2.0, 0, 0, 1.0 / 2.0,
	                            0, 1.0 / 2.0, 0, 1.0 / 2.0,
	                            0, 0, 1.0 / 2.0, 1.0 / 2.0,
	                            0, 0, 0, 1.0);
// Using the view and projection matrix from the previous code.
glm::mat4 model = glm::mat4(1); // identity matrix .. not placing the geometry point somewhere
glm::vec4 somePoint; // Here is some point in the geometry 
glm::vec4 texturePoint = scalingmatrix * projection * view * model * point;

// Now a key thing if you have read the nvidia paper, then you know that the point has to be scale with q
// but this is taken care with the scaling matrix.
// Now to see if we can render it. ( x == s, y == t, w == q) 
// We need to make sure q is positive (we don't want double projection) 
// We also want to make sure x and y are between 0 and 1 because we only want to draw inside the frustrum.
if(texturePoint.w >= 0 && texturePoint.x >= 0 && texturePoint.y <= 1 && texturePoint.x >= 0 && texturePoint.y <= 1)
{
   DrawSomething();
}
```

This code in principal will work if we weren't using OpenGL. I have given as an example to show the concept of how the math works. Now lets go over the shader code required to make this possible. First let's pass in the required matrixes and required points(via deferred shading):

```glsl
#version 330 // Opengl version we are using 3.3
mat4 scaleprojview; 
sampler2D positionMap; // Our geometry points that have there model matrix already applied (model * point)

out vec4 outColor; // Where we are going to render too.
sampler2D outTex; // Our output texture
```

You will need to pass these matricies in and precompute the positions (or you could do it a forward rendering way more on this later).  

I will be assuming that all of the points of geometries are stored in a texture and have been precomputed (More on this later). We will be rendering the projector across the screen with a screen quad and to do that we will need the uv coordinates relative to the screen. Now lets compute the transformed points and do the same code as the CPU version.

```glsl
// Thank you oglvdev
uniform vec2 gScreenSize;
// Get screen coordinate uvs
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize; // Cal
}
void main()
{
    // Get Screen's uvs
    vec2 TexCoord = CalcTexCoord();
    
    // Get point from texture
    vec4 point = texture(positionMap,TexCoord);
    
    // calculate texture point projected onto the terrain
    vec4 projTexCoord = scaleprojview * point;
    
    // see if point is side frustrum and projected at this point
    if(projTexCoord.w >= 0 && projTexCoord.x >= 0 &&& projTexCoord.x <= 1 && projTexCoord.y >= 0 &&& projTexCoord.y <= 1)
    {
       outColor = texture(outTex,projTexCoord.xy;
    }
    
}

```

The above shader code will go point by point and produce an image projected onto the geometry. This code will work provided that you are using deferred shading and precompute the world positions into a texture. This will happen after the lighting phase in deferred shading.

**Steps for Rendering the Projector (OpenGL view)**
1. Place the projector
2. Compute world positions 
3. Compute scaled positions into projected texture space.
4. Determine if the texture needs to be drawed
5. Draw depending on 4.

**Some Extra Comments On Rendering**


**Example Application**
----



**Concluding Notes and Next Tutorial**
-----
