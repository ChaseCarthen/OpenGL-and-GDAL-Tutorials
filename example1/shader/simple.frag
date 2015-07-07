#version 150
out vec4 col;
in vec2 silly;

void main (void)  
{  
   //if(silly.x <= 0)   
   //col = vec4(0.0, 1, 0.0, 1.0); 
   //else
   //col = vec4(0,0,1,1); 

   col = mix(vec4(1.0,0.0,0.0,.5),vec4(0.0,.9,0.0,.5),silly.x);
}