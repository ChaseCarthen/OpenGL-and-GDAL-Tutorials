#version 150
out vec4 col;
in vec2 silly;

void main (void)  
{     
   col = vec4(silly.x/1000.0, 0.0, silly.y/1000.0, 1.0);  
}
