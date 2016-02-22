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
	flat int MarkerID; 
};


shader VSmain(out VSOutput VSout)
{ 
	int markerID = markerIDBuffer.IDs[gl_VertexID].x;
	vec2 pos = PositionBuffer.Positions[markerID].xy; 
	gl_Position = vec4(2*(pos.x-0.5), 2*(pos.y-0.5), 0.0, 1.0); 
	VSout.MarkerID = markerID;
};

shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	int ID = FSin.MarkerID; 
	ivec3 comp = ivec3(256 * 256 * 256, 256 * 256, 256);


	FragColor = vec4(1, 1, 1, 0.0); 
};

program DrawMarkersBufferTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};