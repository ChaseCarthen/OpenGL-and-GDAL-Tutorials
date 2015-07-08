#version 330 
uniform sampler2D gPositionMap; 
uniform sampler2D gColorMap; 
uniform sampler2D gNormalMap; 



// Thank you oglvdev
uniform vec2 gScreenSize;
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}
out vec4 FragColor;
void main()
{
   	vec2 TexCoord = CalcTexCoord();
   	vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
   	vec3 Color = texture(gColorMap, TexCoord).xyz;
   	vec3 Normal = texture(gNormalMap, TexCoord).xyz;
   	Normal = normalize(Normal);

   	//FragColor = vec4(Color, 1.0) * CalcPointLight(WorldPos, Normal);
   	FragColor = vec4(1.0,1.0,1.0,1.0);
}