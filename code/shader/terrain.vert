#version 330
//uniform vec2 pos;
layout (location = 0) in vec3 poses;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

out vec2 silly;
uniform mat4 mvp;
uniform mat4 model;
uniform float Max;
uniform float Min;

out vec2 TexCoord0;
out vec3 Normal0;
out vec4 WorldPos0;

void main(void)
{
   //vec4 a = gl_Vertex;
   //a.x = a.x * 0.5;
   //a.y = a.y * 0.5;

   vec3 p = poses;
   p.y *= Max;
   silly.x = (p.y - Min) / (Max-Min);
   if(silly.x < 0)
   silly.x = 0;

   gl_Position = mvp*vec4(p,1);
   TexCoord0 = TexCoord;
   Normal0 = (model*vec4(Normal,0.0)).xyz;
   WorldPos0 = vec4((model*vec4(p,1.0)).xyz,1.0); 
}
