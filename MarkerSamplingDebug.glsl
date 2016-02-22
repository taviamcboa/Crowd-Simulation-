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

uniform sampler2D gMarkerSamplingMap;  

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
	//vec4 markerAgent= texture(gMarkerSamplingMap, FSin.TexCoord.xy).rgba; 

	vec4 markerAgent = texelFetch(gMarkerSamplingMap, ivec2(gl_FragCoord.xy), 0);

	uvec2 markerAgentID; 
	
	markerAgentID = uvec2(markerAgent.r * 256 * 256 * 256 + markerAgent.g * 256 * 256 + markerAgent.b * 256);
	
	unsigned int markeragentID_offset = uvec2(floor(markerAgentID.x/256.0f)).x; 

	markerAgentID = uvec2(markerAgentID.x - markeragentID_offset); 

	//if(markerAgentID.x > 255 && markerAgentID.x < 511)
	//{
		//markerAgentID = uvec2(markerAgentID.x - 1); 
	//}
	//if(markerAgentID.x > 511)
	//{
		//markerAgentID = uvec2(markerAgentID.x - 2); 
	//}

	imageStore(gMarkerBuffer, int(FSin.MarkerID), vec4(FSin.MarkerID, markerAgentID.x, gl_FragCoord.xy));

	FragColor = markerAgent;
};

program MarkerSamplingDebugTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};