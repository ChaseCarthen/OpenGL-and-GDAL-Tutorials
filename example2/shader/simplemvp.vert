#version 150
uniform vec2 pos;
in vec3 poses;
out vec2 silly;
//in mat4 model;
uniform mat4 mvp;
void main(void)
{
   //vec4 a = gl_Vertex;
   //a.x = a.x * 0.5;
   //a.y = a.y * 0.5;
   vec3 pos2 = poses;
   
   pos2.y *= 2142.35;
   pos2.y -= 2022.34;
   pos2.y /= 100;

   pos2.y *= .2;
   silly = pos2.yz;
   if(silly.x < 0)
   {
   		silly.x *= - 1.0;
   		silly.x /= 10.0;
   }
   gl_Position = mvp*vec4(pos2,1);
   //gl_Position = gl_ModelViewProjectionMatrix * a;
   //gl_Position = vec4(pos, 0.0, 1.0);
}
