struct VSInput
{
    vec3 Position;
    vec2 TexCoord; 
};

interface VSOutput
{
    vec2 TexCoord;
};

uniform sampler2DArray gGuidTexureArray;     //navigation 
uniform sampler2DArray gNaviTexureArray;       //previous Pass
uniform int groupID; 
uniform bool drawNavi; 

shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
    VSout.TexCoord   = VSin.TexCoord;   
    gl_Position = vec4(VSin.Position.x, VSin.Position.y, VSin.Position.z, 1.0); 
};

shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	vec4 samplenavi = texture(gNaviTexureArray, vec3(FSin.TexCoord.xy, groupID)).xyzw; 
	vec4 sampleguid = texture(gGuidTexureArray, vec3(FSin.TexCoord.xy, groupID)).xyzw; 

	FragColor = (drawNavi == true ? vec4(abs(samplenavi.xy), 0, 0) : vec4(abs(sampleguid.xy), 0, 0)); 
};

program DrawMaskedNaviGuidTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};