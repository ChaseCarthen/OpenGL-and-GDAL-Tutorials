#version 150
uniform vec2 pos;
uniform mat4 mvp;
in vec3 poses;

void main(void)
{
   gl_Position = mvp*vec4(poses,1);
}