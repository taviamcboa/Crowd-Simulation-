struct VSInput
{
    vec3 Position;
    vec2 TexCoord; 
	float MarkerID; 
};

interface VSOutput
{
	vec2 Position; 
    vec2 TexCoord;
	flat float MarkerID; //flat: no interpolation 
};

uniform sampler2D gAgentVoronoiMap;  
uniform sampler2D gEularianMap;     
uniform sampler2D gMarkerVoronoiMap; 
uniform int gDebug; 

//for debug purposes
writeonly uniform imageBuffer gMarkerBuffer; //current Output

const vec4 bits = vec4(256 * 256 * 256, 256* 256, 1* 256, 0); // 0 for alpha channel 

shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
	VSout.Position = VSin.Position.xy; 
    VSout.TexCoord = VSin.TexCoord;   
	VSout.MarkerID = VSin.MarkerID;   
    gl_Position = vec4(2*(VSin.Position.x-0.5), 2*(VSin.Position.y-0.5), VSin.Position.z, 1.0); 
};

shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
    vec4 eularian = texture(gEularianMap, FSin.TexCoord.xy); 

	if(eularian != vec4(0,0,0,0))
	{
	   discard; 
	}

	//ivec2 tex_size = textureSize(gAgentVoronoiMap, 0); 

	vec4 markerAgent= texture(gAgentVoronoiMap, FSin.TexCoord.xy).rgba; 
	vec4 marker = texture(gMarkerVoronoiMap, FSin.TexCoord.xy).rgba; 

	uvec2 markerAgentID; 
	uvec2 markerID; 
	
	markerAgentID = uvec2(markerAgent.r * 256 * 256 * 256 + markerAgent.g * 256 * 256 + markerAgent.b * 256);
	markerID = uvec2(marker.r * 256 * 256 * 256 + marker.g * 256 * 256 + marker.b * 256);
	
	unsigned int markeragentID_offset = uvec2(floor(markerAgentID.x/256.0f)).x; 
	unsigned int markerID_offset = uvec2(floor(markerID.x/256.0f)).x; 

	markerAgentID = uvec2(markerAgentID.x - markeragentID_offset); 
	markerID = uvec2(markerID.x - markerID_offset); 

	//if(markerAgentID.x > 255 && markerAgentID.x < 511)
	//{
		//markerAgentID = uvec2(markerAgentID.x - 1); 
	//}
	//if(markerAgentID.x > 511)
	//{
		//markerAgentID = uvec2(markerAgentID.x - 2); 
	//}

	imageStore(gMarkerBuffer, int(FSin.MarkerID), vec4(markerID.x, markerAgentID.x, gl_FragCoord.xy));
	
	FragColor = vec4(texture(gAgentVoronoiMap, FSin.TexCoord.xy).rgb, 1.0);   

	if(markerAgentID == 0)
	{
	   FragColor = vec4(1,1,1,0); 
	}
	if(markerAgentID == 127)
	{
	   FragColor = vec4(0.5,0,0,0); 
	}
	if(markerAgentID == 128)
	{
	   FragColor = vec4(0,1,0,0); 
	}
	if(markerAgentID == 150)
	{
	   FragColor = vec4(0,1,1,0); 
	}
	if(markerAgentID == 511)
	{
	   FragColor = vec4(1,0,0,0); 
	}
	if(markerAgentID == 512)
	{
	   FragColor = vec4(0,1,0,0); 
	}
	if(markerAgentID == 513)
	{
	   FragColor = vec4(1,1,0,0); 
	}
	if(markerAgentID == 383)
	{
	   FragColor = vec4(1,1,1,0); 
	}
	if(markerAgentID == 384)
	{
	   FragColor = vec4(0,1,0,0); 
	}

	FragColor = vec4(texture(gAgentVoronoiMap, FSin.TexCoord.xy).rgb, 1.0);   
};

program MarkerSamplingTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};