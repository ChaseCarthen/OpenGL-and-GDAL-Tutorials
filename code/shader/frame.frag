#version 330 
uniform sampler2D gPositionMap; 
uniform sampler2D gColorMap; 
uniform sampler2D gNormalMap; 
uniform sampler2D gProjectorMap;

uniform vec3 dirlight;
uniform vec3 color;
// Intensitys
uniform float ambient;
uniform float diffuse;
uniform float specular; 
uniform float specularPower; // specular power

uniform int hasproj; // has projectors?

// Cameras position in the world
uniform vec3 EyePos;

// Thank you oglvdev
uniform vec2 gScreenSize;
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}
layout (location = 0) out vec4 FragColor;
void main()
{
   	vec2 TexCoord = CalcTexCoord();
   	vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
   	vec3 Color = texture(gColorMap, TexCoord).xyz;
   	vec3 Normal = texture(gNormalMap, TexCoord).xyz;
   	Normal = normalize(Normal);

    float DiffuseFactor = dot(Normal,-dirlight);
    vec3 AmbientColor = color * ambient;
    vec3 DiffuseColor = vec3(0,0,0);
    vec3 SpecularColor = vec3(0, 0, 0);

    if(DiffuseFactor > 0)
    {
    	DiffuseColor = color*DiffuseFactor*diffuse;

    	// specular calculations
    	vec3 VertexToEye = normalize(EyePos - WorldPos);
    	vec3 LightReflect = normalize(reflect(dirlight,Normal));
    	float Specular = dot(VertexToEye,LightReflect);
    	if(Specular > 0)
    	{
    		Specular = pow(Specular,specularPower);
    		SpecularColor = color* specular* Specular;
    	}
    }
    vec4 projcolor = texture(gProjectorMap,TexCoord);
    if(projcolor.a > 0.0  && hasproj < 0 )
    {
      //FragColor=vec4(0,1,0,1);
      FragColor = projcolor;//vec4(projcolor.xyz,.7);
    }
    else
    {
   	  FragColor = vec4(Color* (AmbientColor + DiffuseColor + SpecularColor),.7);
    }
    
}