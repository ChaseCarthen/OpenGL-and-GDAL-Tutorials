#version 150
uniform vec2 pos;
in vec3 poses;

void main(void)
{
   //vec4 a = gl_Vertex;
   //a.x = a.x * 0.5;
   //a.y = a.y * 0.5;

   gl_Position = vec4(poses,1);
   //gl_Position = gl_ModelViewProjectionMatrix * a;
   //gl_Position = vec4(pos, 0.0, 1.0);
}
