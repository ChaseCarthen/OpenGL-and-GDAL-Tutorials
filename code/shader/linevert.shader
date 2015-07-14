#version 330
in vec3 v_position;
//in vec3 v_color;

uniform mat4 mvpMatrix;
uniform mat4 model;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

void main(void)
{
   gl_Position = mvpMatrix * vec4(v_position, 1.0);
  
   TexCoord0 = vec2(0,0);
   Normal0 = (model*vec4(0.0,0.0,0.0,0.0)).xyz;
   WorldPos0 = (model*vec4(v_position,1.0)).xyz;
}
