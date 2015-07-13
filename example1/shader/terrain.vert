#version 150
uniform vec2 pos;
in vec3 poses;
out vec2 silly;
//in mat4 model;
uniform mat4 mvp;
uniform float Max;
uniform float Min;
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
   //gl_Position = gl_ModelViewProjectionMatrix * a;
   //gl_Position = vec4(pos, 0.0, 1.0);
}
