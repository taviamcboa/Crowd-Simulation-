
#define             READY_UNCERTAINTY  1
#define             IN_UNCERTAINTY     2
#define             DEAD_UNCERTAINTY   3
#define             READY_FORWARD_DEST 1
#define             VISIT_DEST         2
#define             PASSING_TUNNEL     3

layout (binding = 0, std430)  buffer position
{
  vec2 Positions[];
} PositionBuffer;

layout (binding = 1, std430)  buffer gidaid
{
  ivec2 IDs[]; 
} iGIDAIDBuffer; 

layout (binding = 2, std430)  buffer aRads
{
  float rads[]; 
} RadiusBuffer; 
layout (binding = 3, std430)  buffer uncertaintystatus
{
  flat ivec2 uncertaintystatus[]; // .R: STATU
} uncertaintyStatusBuffer;

interface VSOutput
{
	vec3 Position; 
	flat ivec2 GridXY; 
	flat ivec2 GroupID_AgentID; 
	float Radius; 
};

interface GSOutput
{
	flat ivec2 GroupID_AgentID; 
	flat ivec2 GridXY; 
}; 

ivec2 convert_pos2_grid(const in vec2 position)
{
	ivec2 grid; 
	int grid_x = int(position.x * 128); 
	int grid_y = int(position.y * 128); 
	grid = ivec2(grid_x, grid_y); 
	return grid;
}

const int edges = 12; 
const float agentrad = 0.00390625 * 0.8;//0.004558 * 0.8; //7.2928/1600

layout (binding = 0, rgba8) uniform image2D gUncertaintyImage; 

const vec4 color_group[8] = 
{
      vec4(1,0,0,1),
	  vec4(0,1,0,1), 
	  vec4(0,0,1,1), 
	  vec4(1,1,0,1), 
	  vec4(1.0,0.0,0.9,1),
	  vec4(0.7,0.8,0.3,1),
	  vec4(0,1,1,1), 
	  vec4(1,0,1,1)
};

shader VSmain(out VSOutput VSout)
{ 
	int agentID = iGIDAIDBuffer.IDs[gl_VertexID].y;
	int groupID = iGIDAIDBuffer.IDs[gl_VertexID].x; 

	float _radius = RadiusBuffer.rads[agentID].x; 
	vec2 _position = PositionBuffer.Positions[agentID].xy;

	ivec2 _gridxy = convert_pos2_grid(_position);

	VSout.GridXY.xy = _gridxy;
	VSout.Position.xyz = vec3(_position.xy, 0);
	VSout.GroupID_AgentID = ivec2(groupID, agentID); 
	VSout.Radius = _radius;
};

shader GSmain(in VSOutput GSin[], out GSOutput GSout)
{
	vec3 v_center = GSin[0].Position; // 0--1
	ivec2 v_gridxy = GSin[0].GridXY;
	float degree_start = 0.0; 
	float radian_start; 

	float degree_max  = 360;  
	float degree_stride = degree_max / edges; 
	vec2 v_vertex[edges]; 
	for(int i =0 ; i< edges; i++)
	{
		radian_start = radians(degree_start); 

		v_vertex[i].x = cos(radian_start); 
		v_vertex[i].y = sin(radian_start); 

		degree_start += degree_stride; 
	}

	//triangle formation
	int index_1, index_2; 
	vec2 vertex_1, vertex_2; 
	for(int i = 0 ; i < edges; i++)
	{
		index_1 = i % edges; 
		index_2 = (i+1) %edges; 

		vertex_1 = v_vertex[index_1] * agentrad + v_center.xy; 
		GSout.GroupID_AgentID = GSin[0].GroupID_AgentID; 
		GSout.GridXY = v_gridxy;
		gl_Position = vec4(2*(vertex_1.x-0.5), 2*(vertex_1.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		vertex_2 = v_vertex[index_2] * agentrad + v_center.xy; 
		GSout.GroupID_AgentID = GSin[0].GroupID_AgentID; 
		GSout.GridXY = v_gridxy;
		gl_Position = vec4(2*(vertex_2.x-0.5), 2*(vertex_2.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		GSout.GroupID_AgentID = GSin[0].GroupID_AgentID; 
		GSout.GridXY = v_gridxy;
		gl_Position = vec4(2*(v_center.x-0.5), 2*(v_center.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		EndPrimitive(); 
	}
}

shader FSmain(in GSOutput FSin, out vec4 FragColor)
{
     ivec2 _GridXY = FSin.GridXY; 
	 vec4 uncertainty_value = imageLoad(gUncertaintyImage, _GridXY); 
	 FragColor = color_group[FSin.GroupID_AgentID.r]; 
	 int agentID = FSin.GroupID_AgentID.g; 
	 int status = uncertaintyStatusBuffer.uncertaintystatus[agentID].r;
	 if(status == IN_UNCERTAINTY)
		FragColor = vec4(1,1,1,1); 
	 if(status == VISIT_DEST)
		FragColor = vec4(1,1,1,1); 
};

program DrawAgentsBufferTechnique
{
    vs(430) = VSmain();
	gs(430) = GSmain() : in(points), out(triangle_strip, max_vertices = 36);
    fs(430) = FSmain();
};