#version 330

in vec3 posmap;

layout (location = 0) out vec3 pos;
layout (location = 1) out vec4 col;

void main (void)  
{  
   pos = posmap.xyz;
   col = vec4(1,1,1,1);
}