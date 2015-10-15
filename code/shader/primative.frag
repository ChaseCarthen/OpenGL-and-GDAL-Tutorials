#version 330

in vec3 posmap;

layout (location = 0) out vec4 pos;
layout (location = 1) out vec4 col;

void main (void)  
{  
   pos = vec4(posmap.xyz,1.0);
   col = vec4(1,1,1,1);
}