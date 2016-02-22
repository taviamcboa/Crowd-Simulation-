layout (binding = 0, std430)  buffer position
{
  vec2 Positions[];

} PositionBuffer;

layout (binding = 1, std430)  buffer buildingid
{
  ivec2 BuildingIDs[];

} BuildingIDBuffer;


interface VSOutput
{
	vec2 Position0; 
	vec2 Position1; 
	vec2 Position2; 
	vec2 Position3; 
	flat ivec2 BuildingIDs; 
};

interface GSOutput
{
	flat ivec2 BuildingIDs; 
}; 

const vec4 color_building[8] = 
{
      vec4(1,0,0,0),
	  vec4(0,1,0,0), 
	  vec4(0,0,1,0), 
	  vec4(1,1,0,0), 
	  vec4(0,1,1,0), 
	  vec4(1,0,1,0), 
	  vec4(0.2,1,0.5,0), 
	  vec4(0.5, 0.2, 0.8, 1)
};

shader VSmain(out VSOutput VSout)
{ 
	VSout.Position0 = PositionBuffer.Positions[4 * gl_VertexID].xy;
	VSout.Position1 = PositionBuffer.Positions[4 * gl_VertexID + 1].xy;
	VSout.Position2 = PositionBuffer.Positions[4 * gl_VertexID + 2].xy;
	VSout.Position3 = PositionBuffer.Positions[4 * gl_VertexID + 3].xy;
	VSout.buildingID = VSin.buildingID; 
};

shader GSmain(in VSOutput GSin[], out GSOutput GSout)
{
	vec2 v0 = GSin[0].Position0; // 0--1
	vec2 v1 = GSin[0].Position1; // 0--1
	vec2 v2 = GSin[0].Position2; // 0--1
	vec2 v3 = GSin[0].Position3; // 0--1

	

	vec2 v0v1_mid = 0.5 * (v0 + v1); 
	
	vec2 v0v1_mid_tu = v0v1_mid - vec2()

	vec2 v1v2_mid = 0.5 * (v1 + v2); 
	vec2 v2v3_mid = 0.5 * (v2 + v3); 
	vec2 v3v0_mid = 0.5 * (v3 + v0); 

	//
	



	//triangle formation
	int index_1, index_2; 
	vec2 vertex_1, vertex_2; 
	for(int i = 0 ; i < edges; i++)
	{
		index_1 = i % edges; 
		index_2 = (i+1) %edges; 

		vertex_1 = v_vertex[index_1] * agentrad + v_center.xy; 
		GSout.GroupID_AgentID = GSin[0].GroupID_AgentID; 
		gl_Position = vec4(2*(vertex_1.x-0.5), 2*(vertex_1.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		vertex_2 = v_vertex[index_2] * agentrad + v_center.xy; 
		GSout.GroupID_AgentID = GSin[0].GroupID_AgentID; 
		gl_Position = vec4(2*(vertex_2.x-0.5), 2*(vertex_2.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		GSout.GroupID_AgentID = GSin[0].GroupID_AgentID; 
		gl_Position = vec4(2*(v_center.x-0.5), 2*(v_center.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		EndPrimitive(); 
	}
}

shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	 FragColor = color_building[ivec2(FSin.buildingID.g).r]; 
};

program DrawUncertaintyTechnique
{
    vs(430) = VSmain();
	gs(430) = GSmain() : in(points), out(triangle_strip, max_vertices = 36);
    fs(430) = FSmain();
};