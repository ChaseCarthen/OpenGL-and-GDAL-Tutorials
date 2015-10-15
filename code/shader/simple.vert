#version 150
uniform vec2 pos;
in vec3 poses;

void main(void)
{
   gl_Position = vec4(poses,1);
}
