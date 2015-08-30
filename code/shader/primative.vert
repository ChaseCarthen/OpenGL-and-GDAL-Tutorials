#version 330
uniform vec2 pos;
uniform mat4 mvp;
in vec3 poses;
out vec3 posmap;
void main(void)
{
   gl_Position = mvp*vec4(poses,1);
   posmap = (vec4(poses,1)).xyz;
}