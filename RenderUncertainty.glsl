struct VSInput
{
    vec3 Position;
    vec2 TexCoord; 
};

interface VSOutput
{
    vec2 TexCoord;
};

uniform sampler2D gEularianMap;    
uniform sampler2D gUncertaintyMap;  


shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
    VSout.TexCoord  = VSin.TexCoord;   
    gl_Position = vec4(VSin.Position.x, VSin.Position.y, VSin.Position.z, 1.0); 
};


shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	vec4 color1 = texture(gEularianMap, FSin.TexCoord.xy).rgba; 
	vec4 color2 = texture(gUncertaintyMap, FSin.TexCoord.xy).rgba; 
    FragColor = vec4(color2 - color1); 
};

program RenderUncertaintyTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};