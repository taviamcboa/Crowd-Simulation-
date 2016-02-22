struct VSInput
{
    vec3 Position;
    vec2 TexCoord; 
};

interface VSOutput
{
    vec2 TexCoord;
};

uniform sampler2D gMarkerSamplingMap1;    
uniform sampler2D gMarkerSamplingMap2;  


shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
    VSout.TexCoord   = VSin.TexCoord;   
    gl_Position = vec4(VSin.Position.x, VSin.Position.y, VSin.Position.z, 1.0); 
};


shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	vec4 color1 = texture(gMarkerSamplingMap1, FSin.TexCoord.xy).rgba; 
	vec4 color2 = texture(gMarkerSamplingMap2, FSin.TexCoord.xy).rgba; 
    FragColor = vec4(color2.rgb, color1.a); 
};

program TextureSubTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};