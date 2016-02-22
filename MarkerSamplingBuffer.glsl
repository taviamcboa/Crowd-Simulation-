layout (binding = 0, std430)  buffer position
{
  vec2 Positions[];
} PositionBuffer;

layout (binding = 1, std430)  buffer markerID
{
  int IDs[]; 
} markerIDBuffer; 

interface VSOutput
{
	vec2 Position; 
    vec2 TexCoord;
	flat float MarkerID; //flat: no interpolation 
};

uniform sampler2D gAgentVoronoiMap;   
uniform sampler2D gEularianMap;     

const vec4 bits = vec4(256 * 256 * 256, 256* 256, 1* 256, 0); // 0 for alpha channel 

shader VSmain(out VSOutput VSout)
{ 
	int markerID = markerIDBuffer.IDs[gl_VertexID].x;
	vec2 pos = PositionBuffer.Positions[markerID].xy; 
	VSout.Position = pos; 
    VSout.TexCoord = pos;   
	VSout.MarkerID = markerID;   
    gl_Position = vec4(2*(pos.x-0.5), 2*(pos.y-0.5), 0.0, 1.0); 
};

shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
    vec4 eularian = texture(gEularianMap, FSin.TexCoord.xy); 
	
    if(eularian != vec4(0,0,0,0))
	{
	   discard; 
	}


    vec4 markerAgent= texture(gAgentVoronoiMap, FSin.TexCoord.xy).rgba; 

	uvec2 markerAgentID; 
	markerAgentID = uvec2(markerAgent.r * 256 * 256 * 256 + markerAgent.g * 256 * 256 + markerAgent.b * 256);
	unsigned int markeragentID_offset = uvec2(floor(markerAgentID.x/256.0f)).x; 
	markerAgentID = uvec2(markerAgentID.x - markeragentID_offset); 

	FragColor = vec4(texture(gAgentVoronoiMap, FSin.TexCoord.xy).rgb, 1.0);   
	//if(markerAgentID == 799)
	//{
	//   FragColor = vec4(0,1,0,0); 
	//}
	//if(markerAgentID == 600)
	//{
	//   FragColor = vec4(1,0,0,0); 
	//}
	//if(markerAgentID == 699)
	//{
	//   FragColor = vec4(1,1,0,0); 
	//}
	//if(markerAgentID == 700)
	//{
	 //  FragColor = vec4(1,1,1,0); 
	//}

};

program MarkerSamplingBufferTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};