#version 330

in vec2 TexCoord0; 
in vec3 Normal0; 
in vec4 WorldPos0; 

layout (location = 0) out vec4 WorldPosOut; 
layout (location = 1) out vec4 DiffuseOut; 
layout (location = 2) out vec3 NormalOut; 
layout (location = 3) out vec3 TexCoordOut; 


void main() 
{ 
    WorldPosOut = WorldPos0; 
    DiffuseOut = vec4(.7,.45,.01,1); 
    NormalOut = normalize(Normal0); 
    TexCoordOut = vec3(TexCoord0, 0.0); 
}
